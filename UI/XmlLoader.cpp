#include <RapidXML/rapidxml.hpp>
#include "XmlLoader.hpp"
#include <istream>
#include <fstream>
#include "Gui.hpp"

#ifdef USE_SFML
	
#elif USE_SDL
	#include "controls/Button.hpp"
	#include "controls/ScrollBar.hpp"
	#include "controls/Container.hpp"
	#include "controls/ComboBox.hpp"
	#include "controls/GridContainer.hpp"
	#include "controls/TextBox.hpp"
	#include "controls/RadioButton.hpp"
	#include "controls/ListBox.hpp"
	#include "controls/Label.hpp"
	#include "controls/TrackBar.hpp"
	#include "controls/Canvas.hpp"
	#include "controls/CheckBox.hpp"
	#include "controls/WidgetMover.hpp"
#endif

namespace ng {
namespace XmlLoader {
	using std::cout;
	using std::endl;
	using namespace rapidxml;
	void LoadXml(GuiEngine& engine, std::string filename) {
		std::fstream f;
		f.open(filename);
		LoadXml(engine, f);
		f.close();
	}
	
	
	
	void processListBoxItems(ListBox* lb, xml_node<>* node) {
		for(xml_node<>* n = node->first_node("item"); n; n=n->next_sibling()) {
			lb->AddItem(n->value());
		}
	}
	void processComboBoxItems(ComboBox* lb, xml_node<>* node) {
		for(xml_node<>* n = node->first_node("item"); n; n=n->next_sibling()) {
			lb->AddItem(n->value());
		}
	}
	
	std::map<std::string, std::function<Control*()>> m_extended_tags;
	std::map<std::string, Control*> m_control_templates;
	void RegisterControl(std::string tag, std::function<Control*()> control_constructor) {
		if(m_extended_tags.find(tag) != m_extended_tags.end()) return;
		m_extended_tags[tag] = control_constructor;
	}
	Control* tryExtendedTags(const char* tag) {
		auto it1 = m_control_templates.find(tag);
		if(it1 != m_control_templates.end())
			return it1->second->Clone();
			
		auto it = m_extended_tags.find(tag);
		if(it != m_extended_tags.end()) return it->second();
		return 0;
	}
	
	Control* createControlByXmlTag(const char* tag) {
		#define TAGTYPE(tag, type) \
			case hash(tag): \
				if(m_control_templates.find(tag) != m_control_templates.end()) { \
					control = m_control_templates[tag]->Clone(); \
				} else { \
					control = new type(); \
				} \
				break; 
		
		Control* control;
		switch(hash(tag)) {
			TAGTYPE("control", Control);
			TAGTYPE("widget", Widget);
			// TAGTYPE("dialog", Dialog);
			TAGTYPE("button", Button);
			TAGTYPE("textbox", TextBox);
			TAGTYPE("canvas", Canvas);
			TAGTYPE("listbox", ListBox);
			TAGTYPE("radiobutton", RadioButton);
			TAGTYPE("container", Container);
			TAGTYPE("combobox", ComboBox);
			TAGTYPE("label", Label);
			TAGTYPE("trackbar", TrackBar);
			TAGTYPE("scrollbar", ScrollBar);
			TAGTYPE("widgetmover", WidgetMover);
			TAGTYPE("checkbox", CheckBox);
			TAGTYPE("gridcontainer", GridContainer);
			default: return tryExtendedTags(tag);
		}
		return control;
	}
	
	#define PUT_CONTROL \
		if(engine) engine->AddControl(control); \
		else widget->AddControl(control);
	
	Anchor parseRect(std::string s) {
		Anchor a{{0,0},0};
		float *n = &a.x;
		float *nx = n;
		std::string digit = "0";
		enum Part {
			x,y,w,h
		};
		a.ax = a.ay = 1;
		float num;
		bool ispercent = false;
		Part part = Part::x;
		for(const char *c = s.c_str(); ; c++) {
			if(*c && isdigit(*c) || *c == '.')
				digit += *c;
			switch(*c) {
				case ',':
					part = (Part)((int)part+1);
				case '+':
				case '-':
				case '\0':
					if(part == Part::x) n = &a.x;
					if(part == Part::y) n = &a.y;
					else if(part == Part::w) n = &a.sx;
					else if(part == Part::h) n = &a.sy;
					num = std::stof(digit);
					digit = (*c == '-' ? "-0" : "0");
					if(ispercent) num *= .01;
					*nx += num;
					ispercent = false;
					break;
				case '%':
					ispercent = true;
					if(part == Part::x) n = &a.W;
					else if(part == Part::y) n = &a.H;
					else if(part == Part::w) n = &a.sW;
					else if(part == Part::h) n = &a.sH;
					break;
				case 'L': break;
				case 'R':
					a.ax = -1;
					a.W = 1;
					a.w = -1;
					break;
				case 'T': break;
				case 'U': break;
				case 'M':
					if(part == Part::x) {
						a.w = -0.5;
						a.W = 0.5;
					} else if(part == Part::y) {
						a.h = -0.5;
						a.W = 0.5;
					}
				case 'D':
				case 'B':
					a.ay = -1;
					a.H = 1;
					a.h = -1;
					break;
				case 'x': n = &a.x; break;
				case 'y': n = &a.y; break;
				case 'w': n = &a.w; break;
				case 'h': n = &a.h; break;
				case 'W': n = &a.W; break;
				case 'H': n = &a.H; break;
				case 'r': a.isrelative = true; break;
				case 'a': {
					int adv = digit == "-0" ? -1 : 1;
					digit = "0";
					if(part == Part::x) a.ax = adv;
					if(part == Part::y) a.ay = adv;
					break;
				}
			}
			nx = n;
			if(*c == 0)
				break;
			
		}
		// cout << a << endl;
		return a;
	}
	void loadTheme(xml_node<>* node) {
		for(; node; node = node->next_sibling()) {
			Control *control = createControlByXmlTag(node->name());
			if(!control) continue;
			for(xml_attribute<> *attr = node->first_attribute(); attr; attr = attr->next_attribute()) {
				std::string style = std::string(attr->name());
				std::string value = std::string(attr->value());
				control->SetStyle(style, value);
			}
		}
	}
	void loadXmlRecursive(GuiEngine* engine, Widget* widget, xml_node<>* node, const Anchor &anchor) {
		Control* control = nullptr;
		
		for(; node; node = node->next_sibling()) {
			if(!strcmp(node->name(), "anchor")) {
				Anchor anchor1{{0,0},0};
				bool relative = false;
				for(auto *attr = node->first_attribute(); attr; attr = attr->next_attribute()) {
					
					if(!strcmp(attr->name(), "pos")) {
						anchor1 = parseRect(attr->value());
					} else if(!strcmp(attr->name(), "relative")) {
						if(!strcmp(attr->value(),"true")) {
							relative = true;
						}
					}
				}
				anchor1.isrelative ^= relative;
				loadXmlRecursive(engine, widget, node->first_node(), anchor1);
				
			} else if(!strcmp(node->name(), "theme")) {
				loadTheme(node->first_node());
			}
			
			control = createControlByXmlTag(node->name());
			if(!control) continue;
			
			for(xml_attribute<> *attr = node->first_attribute(); attr; attr = attr->next_attribute()) {
				std::string style = std::string(attr->name());
				std::string value = std::string(attr->value());
				if(style == "rect") {
					Anchor a = parseRect(value);
					a += anchor;
					control->SetAnchor(a);
					control->SetRect(a.x, a.y, a.sx, a.sy);
					continue;
				}
				control->SetStyle(style, value);
			}
			
			if(!strcmp(node->name(), "listbox")) processListBoxItems((ListBox*)control, node);
			if(!strcmp(node->name(), "combobox")) processComboBoxItems((ComboBox*)control, node);
			
			if(node->first_node() && control->IsWidget()) {
				Anchor anchor1{{0,0},0};
				loadXmlRecursive(0, (Widget*)control, node->first_node(), anchor1);
			}
			PUT_CONTROL
		}
	}
	void LoadXml(GuiEngine& engine, std::istream& input_xml) {
		xml_document<> doc;
		
		int data_size;
		input_xml.seekg(0,input_xml.end);
		data_size = input_xml.tellg();
		input_xml.seekg(0,input_xml.beg);
		
		char *data = new char[data_size+1];
		data[data_size] = 0;
		input_xml.read(data, data_size);
		
		try {			
			doc.parse<0>(data);
		} catch(parse_error& err) {
			std::cout << "XML parsing error: " << err.what() << std::endl;
			return;
		}
		Anchor anchor{{0,0},0};
		loadXmlRecursive(&engine, 0, doc.first_node("gui")->first_node(), anchor);
		
		delete[] data;
	}
}
}
