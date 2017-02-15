#include "ControlManager.hpp"
#include "../Widget.hpp"
#include "SDL/Drawing.hpp"
#include "../Gui.hpp"

#include <map>
#include <algorithm>
#include <list>
#include <fstream>

#include <iostream>

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
	if(controls.empty()) return;
	bool w_auto = false, h_auto = false;
	int wres, hres;
	bool any_auto = false;
	int wmax = 0;
	if(this_widget) {
		const Rect &r = this_widget->GetRect();
		const Anchor &a = this_widget->GetAnchor();
		
		// resolve wres
		if(a.w_func != Anchor::SizeFunction::none) {
			w_auto = true;
		}
		wres = std::min<int>(std::max<int>(this_widget->min.w, r.w), this_widget->max.w);
		
		wmax = this_widget->max.w;
		
		this_widget->m_rect.w = wres;
		
		// resolve hres
		if(a.h_func != Anchor::SizeFunction::none) {
			h_auto = true;
		}
		
		hres = std::min<int>(std::max<int>(this_widget->min.h, r.h), this_widget->max.h);
		
		this_widget->m_rect.h = hres;
		
		any_auto = w_auto || h_auto;
	} else {
		Drawing::GetResolution(wres, hres);
		wmax = wres;
	}
	
	// sort controls by coords
	std::sort(controls.begin(), controls.end(), [](Control* ca, Control* cb) {
		const Point &p1 = ca->GetAnchor().coord;
		const Point &p2 = cb->GetAnchor().coord;
		return p1.y < p2.y || (p1.y == p2.y && p1.x < p2.x);
	});
	
	int lasty = 0;
	int min_x = 0, min_y = 0;
	int max_x = min_x, max_y = min_y;
	
	// corner points for auto
	Point c2=Point(0,0);
	for(Control* c : controls) {
		
		const Anchor &a = c->GetAnchor();
		const Rect &r = c->GetRect();
		const Point &p = a.coord;
		
		int w,h;
		// calculate min, max of this control if its widget
		if(c->IsWidget()) {
			Widget* widget = reinterpret_cast<Widget*>(c);
			
			widget->min.w = a.w_min[0] + a.w_min[1] * wres;
			widget->max.w = a.w_max[0] + a.w_max[1] * wres;
			
			widget->min.h = a.h_min[0] + a.h_min[1] * hres;
			widget->max.h = a.h_max[0] + a.h_max[1] * hres;
		}
		
		if(c->IsWidget()) {
			Widget* widget = reinterpret_cast<Widget*>(c);
			widget->ApplyAnchoring();
		} else {
			
			w = (a.w_min[0] == 0 && a.w_min[1] == 0) ? r.w : (wres * a.w_min[1]) + a.w_min[0];
			h = (a.h_min[0] == 0 && a.h_min[1] == 0) ? r.h : (hres * a.h_min[1]) + a.h_min[0];
			c->SetRect( 0, 0, w, h );
			
		}

		// control forced on next line or overflow
		if( !a.absolute_coordinates && (p.y != lasty || (max_x + a.x + r.w > wmax)) ) {
			lasty = p.y;
			
			// starts at x beginning (min_x)
			max_x = min_x;
			
			// at new row
			min_y = max_y;
		}
		
		// std::cout << c->GetId() << ": " << r.x << ", " << r.y << ", " << r.w << ", " << r.h << "\n";
		
		// x + w * width + W * parent_width
		int xc = a.x + (a.w * r.w) + (a.W * wres);
		
		// y + h * height + H * parent_height
		int yc = a.y + (a.h * r.h) + (a.H * hres);
		
		if(a.absolute_coordinates) {
			// TODO: defer if anchor and needed
			c->SetPosition(xc, yc);
		} else {
			c->SetPosition(max_x + xc, min_y + yc);
			
			max_x += xc + r.w;
			max_y = std::max<int>(min_y + yc + r.h, max_y);
		}
		
		if(any_auto) {
			// auto max
			if(w_auto) {
				c2.x = std::max<int>(c2.x, r.x + r.w);
			}
			
			if(h_auto) {
				c2.y = std::max<int>(c2.y, r.y + r.h);
			}
		}
	}
	
	// if widget with auto sizes, must calculate it
	if(this_widget && any_auto) {
		const Rect &r = this_widget->GetRect();
		const Anchor &a = this_widget->GetAnchor();
		
		int w = r.w;
		int h = r.h;
		int x = r.x;
		int y = r.y;
		
		if(w_auto) {
			if(a.w_func == Anchor::SizeFunction::fit) {
				w = c2.x;				
			} else if(a.w_func == Anchor::SizeFunction::expand) {
				w = std::max<int>(w, c2.x);
			}
			x += a.w*w;
		}
		
		if(h_auto) {
			if(a.h_func == Anchor::SizeFunction::fit) {
				h = c2.y;				
			} else if(a.h_func == Anchor::SizeFunction::expand) {
				h = std::max<int>(h, c2.y);
			}
			y += a.h*h;
		}
		
		// TODO: clip w, h with min, max
		this_widget->SetRect(x, y, w, h);
	}
	
	// TODO: process deferred stuff
	
}


void ControlManager::LoadXml(std::string xml_filename) {
	XmlLoader::LoadXml(this_engine, this_widget, xml_filename);
	ApplyAnchoring();
}

void ControlManager::LoadXml(std::istream& stream) {
	XmlLoader::LoadXml(this_engine, this_widget, stream);
	ApplyAnchoring();
}

void ControlManager::RegisterControl(std::string tag, std::function<Control*()> control_constructor) {
	XmlLoader::RegisterControl(tag, control_constructor);
}

Control* ControlManager::CreateControl(std::string tag, std::string id) {
	Control* ctrl = XmlLoader::createControlByXmlTag(tag.c_str());
	if(ctrl)
		ctrl->SetId(id);
	return ctrl;
}

} // namespace ng


#include <RapidXML/rapidxml.hpp>
#include "../AllControls.hpp"

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
				control = new type(); \
				break;
			
		Control* control;
		if(m_control_templates.find(tag) != m_control_templates.end()) {
			control = m_control_templates[tag]->Clone();
			// cout << "control: " << tag << " found\n";
		} else {
			
			const char* c = strrchr(tag, '/');
			if(!c) c = tag;
			else c++;
			
			switch(hash(c)) {
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
				TAGTYPE("checkbox", CheckBox);
				TAGTYPE("terminal", Terminal);
				TAGTYPE("form", Form);
				
				
				TAGTYPE("tooltip", Label);
				// special types for widget
				TAGTYPE("widgetmover", WidgetMover);
				TAGTYPE("widgetresizer", WidgetResizer);
				
				default: return tryExtendedTags(tag);
			}
		}
		return control;
	}
	
	void loadTheme(std::string parent, xml_node<>* node) {
		// cout << "loading theme: " << parent << endl;
		for(; node; node = node->next_sibling()) {
			Control *control = createControlByXmlTag(node->name());
			if(!control) continue;
			for(xml_attribute<> *attr = node->first_attribute(); attr; attr = attr->next_attribute()) {
				std::string style = std::string(attr->name());
				std::string value = std::string(attr->value());
				control->SetStyle(style, value);
			}
			if(control->IsWidget()) {
				loadTheme(parent + node->name() + "/", node->first_node());
			}
			// cout << "theme: " << parent + node->name() << endl;
			m_control_templates[parent + node->name()] = control;
		}
	}
	
	// TODO: remove anchor tag
	void loadXmlRecursive(GuiEngine* engine, Widget* widget, xml_node<>* node, Anchor anchor) {
		Control* control = nullptr;
		Point c{0,0};
		for(; node; node = node->next_sibling()) {
			/*
			if(!strcmp(node->name(), "anchor")) {
				Anchor anchor1{{0,0},0};
				bool relative = false;
				for(auto *attr = node->first_attribute(); attr; attr = attr->next_attribute()) {
					
					if(!strcmp(attr->name(), "pos")) {
						anchor1 = Anchor::parseRect(attr->value());
					} else if(!strcmp(attr->name(), "relative")) {
						if(!strcmp(attr->value(),"true")) {
							relative = true;
						}
					}
				}
				anchor1.isrelative |= relative;
				loadXmlRecursive(engine, widget, node->first_node(), anchor1);
				continue;
			} else */
			if(!strcmp(node->name(), "theme")) {
				if(node->first_attribute() && !strcmp(node->first_attribute()->name(), "prefix") ) {
					loadTheme(node->first_attribute()->value(), node->first_node());
				} else {
					loadTheme("", node->first_node());
				}
				continue;
			} else if(!strcmp(node->name(), "br")) {
				c.x = 0;
				c.y++;
				continue;
			}
			
			control = createControlByXmlTag(node->name());
			if(!control) continue;
			
			for(xml_attribute<> *attr = node->first_attribute(); attr; attr = attr->next_attribute()) {
				std::string style = std::string(attr->name());
				std::string value = std::string(attr->value());
				control->SetStyle(style, value);
			}
			
			Anchor a = control->GetAnchor();
			a.coord = c;
			a += anchor;
			control->SetAnchor(a);
			
			// TODO: generalize making controls with items
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

