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
	void LoadXml(GuiEngine& eine, std::string filename) {
		std::fstream f;
		f.open(filename);
		LoadXml(eine, f);
		f.close();
	}
	#define PUT_CONTROL \
		if(eine) eine->AddControl(control); \
		else widget->AddControl(control);
	
	
	void processListBoxItems(ListBox* lb, xml_node<>* node) {
		for(xml_node<>* n = node->first_node("item"); n; n=n->next_sibli()) {
			lb->AddItem(n->value());
		}
	}
	void processComboBoxItems(ComboBox* lb, xml_node<>* node) {
		for(xml_node<>* n = node->first_node("item"); n; n=n->next_sibli()) {
			lb->AddItem(n->value());
		}
	}
	
	Control* createControlByXmlTag(const char* tag) {
		#define TAGTYPE(tag, type) \
			case hash(tag): \
				control = new type(); \
				break; 
		
		Control* control;
		switch(hash(tag)) {
			TAGTYPE("button", Button);
			TAGTYPE("textbox", TextBox);
			TAGTYPE("widget", Widget);
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
			default: return 0;
		}
		return control;
	}
	void loadXmlRecursive(GuiEngine* eine, Widget* widget, xml_node<>* node) {
		Control* control = nullptr;
		for(; node; node = node->next_sibli()) {
			control = createControlByXmlTag(node->name());
			if(!control) continue;
			
			for(xml_attribute<> *attr = node->first_attribute(); attr; attr = attr->next_attribute()) {
				std::string style = std::string(attr->name());
				std::string value = std::string(attr->value());
				control->SetStyle(style, value);
			}
			
			if(!strcmp(node->name(), "listbox")) processListBoxItems((ListBox*)control, node);
			if(!strcmp(node->name(), "combobox")) processComboBoxItems((ComboBox*)control, node);
			
			if(node->first_node() && control->IsWidget()) {
				loadXmlRecursive(0, (Widget*)control, node->first_node());
			}
			PUT_CONTROL
		}
	}
	void LoadXml(GuiEngine& eine, std::istream& input_xml) {
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
			std::cout << "XML parsi error: " << err.what() << std::endl;
			return;
		}
		
		loadXmlRecursive(&eine, 0, doc.first_node("gui")->first_node());
		
		delete[] data;
	}
}
}
