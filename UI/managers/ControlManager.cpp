#include <RapidXML/rapidxml.hpp>
#include <RapidXML/rapidxml_utils.hpp>
#include <map>
#include <algorithm>
#include <list>
#include <fstream>
#include <iostream>

#include "ControlManager.hpp"
#include "../Widget.hpp"
#include "../Gui.hpp"


namespace ng {

#define CACHE_SPACING 5

cache_entry ControlManager::getControlCache(Control* control) {
	return { control->type,
	  control->z_index,
	  control->visible,
	  control->interactible,
	  control->isWidget,
	  control->m_rect,
	  control };
}
	  
ControlManager::ControlManager(Widget* c) : next_z_index(0), this_widget(c), this_engine(0), coords(0,0) {}

ControlManager::ControlManager(Gui* c) : next_z_index(0), this_engine(c), this_widget(0), coords(0,0) {}


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
	if(z_index <= cache[a].z_index) return a;
	else if(z_index >= cache[b].z_index) return b;
	// a ----- m ----- b
	while(true) {
		if(z_index < cache[m].z_index) {
			b = m;
			m = (a+b) >> 1;
			if(b == m) {
				return m;
			}
		} else {
			a = m;
			m = (a+b) >> 1;
			if(a == m) {
				return m;
			}
		}
	}
	return m;
	
	
	class BinaryCompare {
		public:
		bool operator()(int v, cache_entry& e) {
			return v < e.z_index;
		}
		bool operator()(cache_entry& e, int v) {
			return e.z_index < v;
		}
	};
	return std::distance(cache.begin(), std::lower_bound(cache.begin(), cache.end(), z_index, BinaryCompare()));
}

void ControlManager::printCacheZIndex() {
	int i=0;
	for(cache_entry& e : cache) {
		std::cout << "[" << i << "]" << e.z_index << " ";
		i++;
	}
	std::cout << "\n";
}

void ControlManager::sendToBack(Control* control) {
	int new_z = cache.front().z_index - CACHE_SPACING;
	int old = binary_search(control->z_index);
	cache_entry e = cache[old];
	for(int i = old; i > 0; i--) {
		cache[i] = cache[i-1];
	}
	cache[0] = e;
	if(cache.size() > 1 && cache[0].z_index == cache[1].z_index) {
		rescale_z_indices(CACHE_SPACING);
	}
}

void ControlManager::setZIndex(Control* control, int new_z_index) {
	if(new_z_index == control->z_index) return;
	
	if(cache.size() == 1) {
		cache.front().z_index = new_z_index;
		control->z_index = new_z_index;
		return;
	}
	
	
	if(new_z_index < cache.front().z_index) {
		sendToBack(control);
		return;
	}
	
	int old = binary_search(control->z_index);
	int put_after = binary_search(new_z_index);
	if(old == put_after || old == put_after+1) return;
	
	int new_z;
	if(new_z_index >= cache.back().z_index) {
		new_z = cache.back().z_index + CACHE_SPACING;
	} else {
		new_z = (cache[put_after].z_index + cache[put_after+1].z_index) >> 1;
	}
	
	cache_entry e;
	e = cache[old];
	e.z_index = new_z;
	control->z_index = new_z;
	
	if(old < put_after) {
		// shift left
		for(int i = old; i < put_after; i++) {
			cache[i] = cache[i+1];
		}
		cache[put_after] = e;
	} else { // if (old > put_at)
		// shift right
		put_after++;
		for(int i = old; i > put_after; i--) {
			cache[i] = cache[i-1];
		}
		cache[put_after] = e;
	}
	
	if(cache[put_after].z_index == cache[put_after-1].z_index) {
		rescale_z_indices(CACHE_SPACING);
	}
	
}

void ControlManager::updateCache(Control* control, CacheUpdateFlag flag) {
	int index = binary_search(control->z_index);
	
	if(flag == CacheUpdateFlag::position) {
		cache[index].rect = control->m_rect;
	} else if(flag == CacheUpdateFlag::all) {
		cache[index] = getControlCache(control);
	} else if(flag == CacheUpdateFlag::attributes) {
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

void ControlManager::BreakRow() {
	coords.x = 0;
	coords.y++;
}

void ControlManager::addControlToCache(Control* control) {
	int z_index = control->z_index;
	
	if(z_index == 0 or z_index >= next_z_index) {
		control->z_index = next_z_index;
		cache.push_back(getControlCache(control));
		next_z_index += CACHE_SPACING;
	} else {
		int put_after = binary_search(z_index);
		if(put_after == cache.size()-1) {
			cache.push_back(getControlCache(control));
		} else {
			cache.insert(cache.begin()+put_after, getControlCache(control));
		}
	}
	
	if(control->layout.coord == Point(0,0)) {
		control->layout.coord = coords;
		coords.x++;
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
	
	if(cache.empty()) {
		coords = {0,0};
	}
}

void ControlManager::RemoveControls() {
	cache.clear();
	controls.clear();
}

std::string getTabs(int depth) {
	depth--;
	if(depth <= 0) return "";
	std::string s(depth*2, '\t');
	for(int i=0; i < s.size(); i+=2) {
		s[i] = '|';
	}
	return s;
}
#define dbg(x) 
void ControlManager::ProcessLayout(bool asRoot) {
	bool w_auto = false, h_auto = false;
	int wres, hres;
	bool any_auto = false;
	int wmax = 0;
	dbg(static int depth = 0;)
	dbg(depth++;)
	if(this_widget) {
		const Rect &r = this_widget->GetRect();
		const Layout &a = this_widget->GetLayout();
		
		// resolve wres
		if(a.w_func != Layout::SizeFunction::none) {
			dbg(std::cout << getTabs(depth) << "w_func is: " << a.w_func << "\n";)
			w_auto = true;
		}
		
		
		if(asRoot) {
			wres = r.w;
			wmax = r.w;
		} else {
			wres = clip(r.w, this_widget->min.w, this_widget->max.w);
			wmax = this_widget->max.w;
		}
		
		
		
		// this_widget->m_rect.w = wres;
		dbg(std::cout << getTabs(depth) << "setting w: " << wres << "\n";)
		
		// resolve hres
		if(a.h_func != Layout::SizeFunction::none) {
			dbg(std::cout << getTabs(depth) << "w_func is: " << a.h_func << "\n";)
			h_auto = true;
		}
		
		if(asRoot) {
			hres = r.h;
		} else {
			hres = clip(r.h, this_widget->min.h, this_widget->max.h);
		}
		
		// this_widget->m_rect.h = hres;
		dbg(std::cout << getTabs(depth) << "setting h: " << hres << "\n";)
		
		this_widget->SetRect(r.x, r.y, wres, hres);
		
		any_auto = w_auto || h_auto;
		
		dbg(std::cout << getTabs(depth) << "ProcessLayout (Widget [" << this_widget->GetId() << "])\n";)
	} else {
		if(this_engine) {
			this_engine->backend.screen->GetResolution(wres, hres);
		}
		wmax = wres;
		dbg(std::cout << getTabs(depth) << "ProcessLayout (GUI)\n";)
	}
	
	// sort controls by coords
	std::sort(controls.begin(), controls.end(), [](Control* ca, Control* cb) {
		const Point &p1 = ca->GetLayout().coord;
		const Point &p2 = cb->GetLayout().coord;
		return p1.y < p2.y || (p1.y == p2.y && p1.x < p2.x);
	});
	
	int lasty = 0;
	int min_x = 0, min_y = 0;
	int max_x = min_x, max_y = min_y;
	
	// corner points for auto
	Point c2 = Point(0,0);
	for(Control* c : controls) {
		
		const Layout &a = c->GetLayout();
		const Rect &r = c->GetRect();
		const Point &p = a.coord;
		
		int w,h;
		
		if(c->IsWidget()) {
			// calculate min, max of this control if its widget
			Widget* widget = reinterpret_cast<Widget*>(c);
			
			widget->min.w = a.w_min[0] + a.w_min[1] * wres;
			widget->max.w = a.w_max[0] + a.w_max[1] * wres;
			
			widget->min.h = a.h_min[0] + a.h_min[1] * hres;
			widget->max.h = a.h_max[0] + a.h_max[1] * hres;
			dbg(std::cout << getTabs(depth) << "inside widget [" << widget->GetId()
					  << "] " << widget->min.w 
					  << ", " << widget->max.w 
					  << ", " << widget->min.h 
					  << ", " << widget->max.h << "\n";)
					  
			dbg(std::cout << getTabs(depth) << "+-> Entering widget [" << widget->GetId() << "]\n";)
			widget->ProcessLayout(false);
		} else {
			
			// w = (a.w_min[0] == 0 && a.w_min[1] == 0) ? r.w : (wres * a.w_min[1]) + a.w_min[0];
			// h = (a.h_min[0] == 0 && a.h_min[1] == 0) ? r.h : (hres * a.h_min[1]) + a.h_min[0];
			w = (wres * a.w_min[1]) + a.w_min[0];
			h = (hres * a.h_min[1]) + a.h_min[0];
			
			w = std::max(0, w);
			h = std::max(0, h);
			
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
			// TODO: defer if layout and needed
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
		dbg(std::cout << getTabs(depth) << "has any_auto\n";)
		const Rect &r = this_widget->GetRect();
		const Layout &a = this_widget->GetLayout();
		
		int w = r.w;
		int h = r.h;
		int x = r.x;
		int y = r.y;
		
		if(w_auto) {
			if(a.w_func == Layout::SizeFunction::fit) {
				w = c2.x;				
			} else if(a.w_func == Layout::SizeFunction::expand) {
				w = std::max<int>(w, c2.x);
			}
			x += a.w*w;
		}
		
		if(h_auto) {
			if(a.h_func == Layout::SizeFunction::fit) {
				h = c2.y;				
			} else if(a.h_func == Layout::SizeFunction::expand) {
				h = std::max<int>(h, c2.y);
			}
			y += a.h*h;
		}
		
		// std::cout << "ProcessLayout: " << this_widget->GetId() << ": " << w << ", " << h << "\n";
		// TODO: clip w, h with min, max
		this_widget->SetRect(x, y, w, h);
	}
	
	dbg(
	if(this_widget)
		std::cout << getTabs(depth-1) << "<----- Leaving widget [" << this_widget->GetId() << "]\n";
	else
		std::cout << getTabs(depth-1) << "<----- Leaving Gui\n";
		)
	// TODO: process deferred stuff
	dbg(depth--;)
}


Control* ControlManager::get(const std::string& id) {
	for(auto c : controls) {
		if(c->id == id) {
			return (Control*)c;
		}
		
		if(c->isWidget) {
			Control* p = static_cast<Widget*>(c)->get(id);
			if(p) {
				return p;
			}
		}
	}
	return 0;
}

} // namespace ng



#include "../AllControls.hpp"
#include <cstring>

namespace ng {
namespace XmlLoader {
	using std::cout;
	using std::endl;
	
	std::map<std::string, std::function<Control*()>> m_extended_tags;
	Control* m_common_control = 0;
	std::map<std::string, Control*> m_control_templates;
	
	void RegisterControl(std::string tag, std::function<Control*()> control_constructor) {
		if(m_extended_tags.find(tag) != m_extended_tags.end()) { 
			return;
		}
		m_extended_tags[tag] = control_constructor;
	}
	
	Control* tryExtendedTags(const char* tag) {
		auto it1 = m_control_templates.find(tag);
		if(it1 != m_control_templates.end()) {
			return it1->second->Clone();
		}
			
		auto it = m_extended_tags.find(tag);
		if(it != m_extended_tags.end()) {
			return it->second();
		}
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
		} else {
			
			const char* c = strrchr(tag, '/');
			if(!c) c = tag;
			else c++;
			switch(hash(c)) {
				TAGTYPE("control", Control);
				TAGTYPE("widget", Widget);
				TAGTYPE("dialog", Dialog);
				TAGTYPE("button", Button);
				TAGTYPE("textbox", TextBox);
				TAGTYPE("canvas", Canvas);
				TAGTYPE("listbox", ListBox);
				TAGTYPE("radiobutton", RadioButton);
				TAGTYPE("container", Container);
				TAGTYPE("tabcontainer", TabContainer);
				TAGTYPE("combobox", ComboBox);
				TAGTYPE("label", Label);
				TAGTYPE("trackbar", TrackBar);
				TAGTYPE("scrollbar", ScrollBar);
				TAGTYPE("checkbox", CheckBox);
				TAGTYPE("terminal", Terminal);
				TAGTYPE("form", Form);
				TAGTYPE("progressbar", ProgressBar);
				
				
				TAGTYPE("tooltip", Label);
				
				// special types for widget
				TAGTYPE("widgetmover", WidgetMover);
				TAGTYPE("widgetresizer", WidgetResizer);
				
				default:
					control = tryExtendedTags(tag);
			}
			
			if(m_common_control) {
				m_common_control->CopyStyle(control);
			}
		}
		return control;
	}
	
	static void loadCommon(rapidxml::xml_node<>* node) {
		m_common_control = new Control();
		m_common_control->SetId("common_control");
		for(rapidxml::xml_attribute<> *attr = node->first_attribute(); attr; attr = attr->next_attribute()) {
			std::string style = std::string(attr->name());
			std::string value = std::string(attr->value());
			m_common_control->SetStyle(style, value);
		}
	}
	
	static void loadTemplates(std::string parent, rapidxml::xml_node<>* node) {
		for(; node; node = node->next_sibling()) {
			if(!strcmp(node->name(), "all")) {
				loadCommon(node);
				continue;
			}
			
			Control *control = createControlByXmlTag(node->name());
			if(!control) continue;

			for(rapidxml::xml_attribute<> *attr = node->first_attribute(); attr; attr = attr->next_attribute()) {
				std::string style = std::string(attr->name());
				std::string value = std::string(attr->value());
				control->SetStyle(style, value);
			}
			if(control->IsWidget()) {
				loadTemplates(parent + node->name() + "/", node->first_node());
			}
			m_control_templates[parent + node->name()] = control;
		}
	}
	
	
} // end of namespace XmlLoader

	void ControlManager::loadXmlFirst(rapidxml::xml_node<>* node) {

		Control* control = nullptr;
		Layout layout;
		for(; node; node = node->next_sibling()) {
			if(!strcmp(node->name(), "theme") || !strcmp(node->name(), "template")) {
				if(node->first_attribute() && !strcmp(node->first_attribute()->name(), "prefix") ) {
					XmlLoader::loadTemplates(node->first_attribute()->value(), node->first_node());
				} else {
					XmlLoader::loadTemplates("", node->first_node());
				}
				continue;
			}
			
			parseAndAddControl(node, layout);
		}
	}
	
	void ControlManager::parseXmlFile(rapidxml::file<>* f) {
		rapidxml::xml_document<> doc;
		try {			
			doc.parse<0>(f->data());
		} catch(rapidxml::parse_error& err) {
			std::cout << "XML parsing error: " << err.what() << std::endl;
			return;
		}
		loadXmlFirst(doc.first_node()->first_node());
	}
	
	void ControlManager::AddControl( Control* control ) {
		addControlToCache(control);
	}

	void ControlManager::parseAndAddControl(rapidxml::xml_node<char>* node, Layout& layout) {
		if(!node) return;
		
		if(!strcmp(node->name(), "br")) {
			layout.coord.x = 0;
			layout.coord.y++;
			return;
		}
		
		
		Control* control = XmlLoader::createControlByXmlTag(node->name());
		if(!control) {
			return;
		}
		
		for(rapidxml::xml_attribute<> *attr = node->first_attribute(); attr; attr = attr->next_attribute()) {
			std::string style = std::string(attr->name());
			std::string value = std::string(attr->value());
			control->SetStyle(style, value);
		}
		
		Layout a = control->GetLayout();
		a.coord = layout.coord;
		a += layout;
		control->SetLayout(a);
		
		control->parseXml(node->first_node());

		// std::cout << "adding control " << control->GetId() << "\n";
		AddControl(control);
			
		layout.coord.x++;
	}
	
	void ControlManager::LoadXml(std::string xml_filename) {
		rapidxml::file<> f(xml_filename.c_str());
		parseXmlFile(&f);
		ProcessLayout();
	}

	void ControlManager::LoadXml(std::istream& stream) {
		rapidxml::file<> f(stream);
		parseXmlFile(&f);
		ProcessLayout();
	}
	
	void ControlManager::RegisterControl(std::string tag, std::function<Control*()> control_constructor) {
		XmlLoader::RegisterControl(tag, control_constructor);
	}
	
	Control* ControlManager::CreateControl(std::string tag, std::string id) {
		Control* ctrl = XmlLoader::createControlByXmlTag(tag.c_str());
		if(ctrl) {
			ctrl->SetId(id);
		}
		return ctrl;
	}
	
	void ControlManager::sendToFront(Control* control) {
		control->SetZIndex(cache.back().z_index+1);
	}
}

