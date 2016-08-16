#include "ControlManager.hpp"
#include "../Widget.hpp"
#include "SDL/Drawing.hpp"

#include <map>
#include <algorithm>
#include <list>
#include <fstream>

namespace ng {
	
namespace XmlLoader {
	void LoadXml(GuiEngine* engine, Widget* widget, std::string xml_filename);
	void LoadXml(GuiEngine* engine, Widget* widget, std::istream& xml_stream);
	void RegisterControl(std::string tag, std::function<Control*()> control_constructor);
	Control* createControlByXmlTag(const char* tag);
}
	
#define CACHE_SPACING 5

#define CONTROL_CACHE(control) \
	{ control->type, \
	  control->z_index, \
	  control->visible, \
	  control->interactible, \
	  control->isWidget, \
	  control->custom_check, \
	  control->m_rect, \
	  control }
	  
ControlManager::ControlManager(Widget* c) : next_z_index(0), this_widget(c), this_engine(0) {
}
ControlManager::ControlManager(GuiEngine* c) : next_z_index(0), this_engine(c), this_widget(0) {
}

ControlManager::~ControlManager() {
	cache.clear();
	int len = controls.size();
	for(int i=0; i < len; i++) {
		delete controls[i];
	}
	controls.clear();
}

int ControlManager::binary_search(int z_index) {
	int size = cache.size();
	if(size == 0) return -1;
	int a=0,b=size-1,m=size>>1;
	// check edges
	if(cache[a].z_index == z_index) return a;
	else if(cache[b].z_index == z_index) return b;
	// a ----- m ----- b
	while(true) {
		if(z_index < cache[m].z_index) {
			b = m;
			m = (a+m) >> 1;
			if(b == m)
				return m;
		} else {
			a = m;
			m = (b+m) >> 1;
			if(a == m)
				return m;
		}
	}
	return m;
}



void ControlManager::setZIndex(Control* control, int new_z_index) {
	
	cache_entry e;
	int old = binary_search(control->z_index);
	int put_at = binary_search(new_z_index);

	
	int new_z;
	if(put_at > 0) {
		if(new_z_index > cache[cache.size()-1].z_index) {
			put_at++;
			new_z = cache[put_at].z_index + CACHE_SPACING;
		} else if(put_at != cache.size()-1) {
			new_z = (cache[put_at].z_index + cache[put_at+1].z_index) >> 1;
		} else {
			put_at--;
			new_z = (cache[put_at].z_index + cache[put_at+1].z_index) >> 1;
		}
	} else if(put_at == 0) {
		new_z = cache[0].z_index >> 1;
	}
	
	e = cache[old];
	e.z_index = new_z;
	control->z_index = new_z;
	
	if(old == put_at) {
		return;
	} else if(old < put_at) {
		// shift left
		for(int i = old; i < put_at; i++) {
			cache[i] = cache[i+1];
		}
		cache[put_at] = e;
	} else { // if (old > put_at)
		// shift right
		for(int i = old; i > put_at; i--) {
			cache[i] = cache[i-1];
		}
		cache[put_at] = e;
	}
	
	if(put_at > 0 && put_at < cache.size()-1) {
		if(new_z == cache[put_at+1].z_index ||
		   new_z == cache[put_at-1].z_index) {
		   rescale_z_indices(CACHE_SPACING);
	   }
	} else if(put_at == 0) {
		if(new_z == cache[put_at+1].z_index) {
			rescale_z_indices(CACHE_SPACING);
		}
	} else {
		if(new_z == cache[put_at-1].z_index) {
			rescale_z_indices(CACHE_SPACING);
		}
	}
	
}

void ControlManager::updateCache(Control* control, Control::CacheUpdateFlag flag) {
	int index = binary_search(control->z_index);
	
	if(flag == Control::CacheUpdateFlag::position) {
		cache[index].rect = control->m_rect;
	} else if(flag == Control::CacheUpdateFlag::all) {
		cache[index] = CONTROL_CACHE(control);
	} else if(flag == Control::CacheUpdateFlag::attributes) {
		cache_entry &e = cache[index];
		e.interactible = control->interactible;
		e.visible = control->visible;
	}
}

void ControlManager::rescale_z_indices(int spacing) {
	int size = cache.size();
	int cur = 0;
	for(int i=0; i < size; i++, cur+=spacing) {
		cache[i].z_index = cur;
		cache[i].control->z_index = cur;
	}
	next_z_index = cur;
}


void ControlManager::addControlToCache(Control* control) {
	int z_index = control->z_index;
	if(z_index == 0 or z_index >= next_z_index) {
		control->z_index = next_z_index;
		cache.push_back(CONTROL_CACHE(control));
		next_z_index += CACHE_SPACING;
	} else {
		int put_at = binary_search(z_index);
		if(put_at == cache.size()-1)
			cache.push_back(CONTROL_CACHE(control));
		else
			cache.insert(cache.begin()+put_at, CONTROL_CACHE(control));
	}
	
	controls.push_back(control);
}

void ControlManager::removeControlFromCache(Control* control) {
	int idx = binary_search(control->z_index);
	cache.erase(cache.begin()+idx);
	
	if(control->isWidget) {
		Widget* w = static_cast<Widget*>(control);
		w->set_engine(0);
	}
	
	for(auto j = controls.begin(); j != controls.end(); j++) {
		if(*j == control) {
			controls.erase(j);
			break;
		}
	}
}

void ControlManager::ApplyAnchoring() {
	std::map<Point,std::list<Control*>> grouping;
	int wres, hres;
	if(this_widget) {
		const Rect &r = this_widget->GetRect();
		wres = r.w;
		hres = r.h;
	} else {
		Drawing::GetResolution(wres, hres);	
	}
	for(Control* control : controls) {
		const Anchor &a = control->GetAnchor();
		const Rect &r = control->GetRect();
		const Point &coord = a.coord;
		control->SetRect( 0, 0, a.sx == 0 && a.sW == 0 ? r.w : wres * a.sW + a.sx,
								a.sy == 0 && a.sH == 0 ? r.h : hres * a.sH + a.sy );
		Point p(a.W * wres, 
				a.H * hres);
		if(a.isrelative) {
			grouping[p].push_back(control);
		} else {
			control->SetPosition(p.x + a.w * r.w + a.x, p.y + a.h * r.h + a.y);
		}
		
		if(control->IsWidget()) reinterpret_cast<Widget*>(control)->ApplyAnchoring();
	}
	
	for(auto &kv : grouping) {
		const Point &pt = kv.first;
		std::list<Control*> &l = kv.second;
		
		l.sort([](Control* a, Control* b) -> bool {
			const Point &p1 = a->GetAnchor().coord;
			const Point &p2 = b->GetAnchor().coord;
			return p1.y < p2.y || (p1.y == p2.y && p1.x < p2.x);
		});
		
		int lastx = 0, lasty = 0;
		int min_x = pt.x, min_y = pt.y;
		int max_x = min_x, max_y = min_y;
		for(Control* c : l) {
			const Anchor& a = c->GetAnchor();
			const Rect &r = c->GetRect();
			const Point &p = a.coord;
			if(p.y != lasty || (
			(a.ax >= 0 && max_x + a.x + r.w > wres) ||
			(a.ax < 0 && max_x - a.x - r.w < 0))) {
				lasty = p.y;
				lastx = 0;
				max_x = min_x;
				min_y = max_y;
			}
			int yc = (a.ay >= 0) ? a.y : (- r.h - a.y);
			if(a.ax >= 0) {
				c->SetPosition(max_x + a.x, min_y + yc);
				max_x += a.x + r.w;
			} else {
				c->SetPosition(max_x - a.x - r.w, min_y + yc);
				max_x -= a.x + r.w;
			}
				
			
			if(a.ay >= 0) {
				max_y = std::max<int>(min_y + r.h, max_y);
			} else {
				max_y = std::min<int>(min_y - r.h, max_y);
			}
			lastx++;
		}
	}
}

void ControlManager::LoadXml(std::string xml_filename) {
	XmlLoader::LoadXml(this_engine, this_widget, xml_filename);
}

void ControlManager::LoadXml(std::istream& stream) {
	XmlLoader::LoadXml(this_engine, this_widget, stream);
}

void ControlManager::RegisterControl(std::string tag, std::function<Control*()> control_constructor) {
	XmlLoader::RegisterControl(tag, control_constructor);
}

Control* ControlManager::CreateControl(std::string tag) {
	return XmlLoader::createControlByXmlTag(tag.c_str());
}

} // namespace ng


#include <RapidXML/rapidxml.hpp>
#if USE_SDL
	#include "../controls/Button.hpp"
	#include "../controls/ScrollBar.hpp"
	#include "../controls/Container.hpp"
	#include "../controls/ComboBox.hpp"
	#include "../controls/GridContainer.hpp"
	#include "../controls/TextBox.hpp"
	#include "../controls/RadioButton.hpp"
	#include "../controls/ListBox.hpp"
	#include "../controls/Label.hpp"
	#include "../controls/TrackBar.hpp"
	#include "../controls/Canvas.hpp"
	#include "../controls/CheckBox.hpp"
	#include "../controls/WidgetMover.hpp"
	#include "../controls/Terminal.hpp"
#endif

namespace ng {
namespace XmlLoader {
	using std::cout;
	using std::endl;
	using namespace rapidxml;
	
	
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
			TAGTYPE("terminal", Terminal);
			default: return tryExtendedTags(tag);
		}
		return control;
	}
	
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
						a.H = 0.5;
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
			m_control_templates[node->name()] = control;
		}
	}
	
	void loadXmlRecursive(GuiEngine* engine, Widget* widget, xml_node<>* node, Anchor anchor) {
		Control* control = nullptr;
		Point c{0,0};
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
				anchor1.isrelative |= relative;
				loadXmlRecursive(engine, widget, node->first_node(), anchor1);
			} else if(!strcmp(node->name(), "theme")) {
				loadTheme(node->first_node());
			} else if(!strcmp(node->name(), "br")) {
				c.x = 0;
				c.y++;
			}
			
			control = createControlByXmlTag(node->name());
			if(!control) continue;
			
			for(xml_attribute<> *attr = node->first_attribute(); attr; attr = attr->next_attribute()) {
				std::string style = std::string(attr->name());
				std::string value = std::string(attr->value());
				if(style == "rect") {
					Anchor a = parseRect(value);
					a += anchor;
					a.coord = c;
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
			
			if(engine) {
				engine->AddControl(control);
			} else if(widget) {
				widget->AddControl(control);
			}
			c.x++;
		}
	}
	
	void LoadXml(GuiEngine* engine, Widget* widget, std::istream& input_xml) {
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
		loadXmlRecursive(engine, widget, doc.first_node("gui")->first_node(), {{0,0},0});
		
		delete[] data;
	}
	void LoadXml(GuiEngine* engine, Widget* widget, std::string filename) {
		std::fstream f;
		f.open(filename);
		LoadXml(engine, widget, f);
		f.close();
	}
	
}

}

