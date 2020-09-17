#include <RapidXML/rapidxml.hpp>
#include <RapidXML/rapidxml_utils.hpp>

#include <map>
#include <algorithm>
#include <list>
#include <fstream>
#include <iostream>

#include "ControlManager.hpp"
#include "../Control.hpp"
#include "../Gui.hpp"

#include "../AllControls.hpp"
#include <cstring>
#include "ResourceManager.hpp"

namespace ng {

#define CACHE_SPACING 5

cache_entry ControlManager::getControlCache(Control* control) {
	return {
	  .type = control->type,
	  .z_index = control->z_index,
	  .visible = control->visible,
	  .isWidget = true,
	  .interactible = control->interactible,
	  .rect = control->m_rect,
	  .control = control };
}

ControlManager::ControlManager(Control* c) : next_z_index(0), this_widget(c), coords(0,0) {}



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
	
	// not used
	/*
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
	*/
}

void ControlManager::printCacheZIndex() {
	int i=0;
	for(cache_entry& e : cache) {
		std::cout << "[" << i << "]" << e.z_index << " ";
		i++;
	}
	std::cout << "\n";
}

void ControlManager::sendToFront(Control* control) {
	control->SetZIndex(cache.back().z_index+1);
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
	if(new_z_index > cache.back().z_index) {
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
	// control->z_index = 0;
	// if(cache.empty() or z_index == 0 or z_index >= next_z_index) {
		control->z_index = next_z_index;
		cache.push_back(getControlCache(control));
		next_z_index += CACHE_SPACING;
	/*
	} else {
		int put_after = binary_search(z_index);
		if(z_index < cache[put_after].z_index) {
			cache.insert(cache.begin(), getControlCache(control));
			return;
		}
		if(put_after == cache.size()-1) {
			cache.push_back(getControlCache(control));
		} else {
			cache.insert(cache.begin()+put_after, getControlCache(control));
		}
		
	}*/
	
	// TODO: fix
	// setZIndex(control, z_index);
	
	if(control->layout.coord == Point(0,0)) {
		control->layout.coord = coords;
		coords.x++;
	}
	
	controls.push_back(control);
}

void ControlManager::removeControlFromCache(Control* control) {
	int idx = binary_search(control->z_index);
	auto cbkp = cache;
	cache.erase(cache.begin()+idx);
	for(auto &c : cache) {
		if(c.control == control) {
			std::cout << "fail to remove from cache " << idx << " : " << control->z_index << " : " << c.z_index << " : " << cache.size() << "\n";
			for(auto &c : cbkp) {
				std::cout << c.z_index << " ";
			}
			std::cout << "\n";
			for(auto &c : cache) {
				std::cout << c.z_index << " ";
			}
			std::cout << "\n";
		}
	}
	control->set_engine(0);
	
	for(auto j = controls.begin(); j != controls.end(); j++) {
		if(*j == control) {
			controls.erase(j);
			break;
		}
	}
	
	for(auto j = controls.begin(); j != controls.end(); j++) {
		if(*j == control) {
			std::cout << "fail to erase " << control->GetId() << "\n";
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

std::map<std::string, std::function<Control*()>> ControlManager::m_extended_tags;

void ControlManager::RegisterControl(std::string tag, std::function<Control*()> control_constructor) {
	if(m_extended_tags.find(tag) != m_extended_tags.end()) { 
		return;
	}
	m_extended_tags[tag] = control_constructor;
}

Control* ControlManager::tryExtendedTags(std::string tag) {
	auto it = m_extended_tags.find(tag);
	if(it != m_extended_tags.end()) {
		return it->second();
	}
	return 0;
}

Control* ControlManager::createControlByXmlTag(std::string tag) {
	#define TAGTYPE(tag, type) \
		case hash(tag): \
			control = new type();\
			control->setType(tag);\
			break;

	Control* control;
	
	switch(hash(tag.c_str())) {
		TAGTYPE("control", Control);
		TAGTYPE("widget", Control);
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
		TAGTYPE("graph", Graph);
		
		TAGTYPE("tooltip", Label);
		
		// special types for widget
		TAGTYPE("widgetmover", WidgetMover);
		TAGTYPE("widgetresizer", WidgetResizer);
		
		TAGTYPE("contextmenu", ContextMenu);
		
		default:
			control = tryExtendedTags(tag);
	}
	return control;
}

static int style_tag_counter = 2;


std::vector<GroupStyle> ControlManager::group_styles(1);



void ControlManager::parseStyle(rapidxml::xml_node<>* node, std::vector<Styling>& push_where, int style_group_tag, Layout& layout) {
	Styling styling;
	bool has_for = false;
	for(auto attr = node->first_attribute(); attr; attr=attr->next_attribute()) {
		
		if(!strcmp(attr->name(), "for")) {
			has_for = true;
			styling.style_for = std::string(attr->value());
		} else {
			styling.attributes.push_back(std::make_pair(attr->name(), attr->value()));
		}
	}
	
	auto node_parent = node;
	for(node=node->first_node(); node; node=node->next_sibling()) {
		if(!strcmp(node->name(), "style")) {
			parseStyle(node, styling.child_styles, style_group_tag, layout);
		}
		
		if(!strcmp(node->name(), "br")) {
			layout.coord.x = 0;
			layout.coord.y++;
			// return 0;
		}
		
	}
	
	Styling to_push;
	Styling *s = &styling;
	if(has_for) {
		s = &to_push;
		std::vector<std::string> vals;
		split_string(styling.style_for, vals, '/');
		styling.style_for = vals.back();
		vals.pop_back();
		
		for(auto& v : vals) {
			
			s->style_for = v;
			s->child_styles.emplace_back();
			s = &s->child_styles.front();
		}
		
		*s = std::move(styling);
		mergeStyle(to_push, push_where);
	}
	
	for(node=node_parent->first_node(); node; node=node->next_sibling()) {
		auto *c = parseAndAddControl(node, s->child_styles, style_group_tag, layout);
		if(c) {
			c->ApplyStyle(*s);
			c->ForEachControl([&s](Control* c) { c->ApplyStyle(*s); });
		}
	}
}

void ControlManager::mergeStyle(Styling& styling, std::vector<Styling>& where) {
	auto it = std::find_if(where.begin(), where.end(), [&](Styling& s) { return styling.style_for == s.style_for; } );
	if(it != where.end()) {
		
		bool merge = false;
		if(merge) {
			// merge attributes
			for(auto &s2 : styling.attributes) {
				it->attributes.push_back(s2);
				auto i2 = std::find_if(it->attributes.begin(), it->attributes.end(), [&](std::pair<std::string, std::string>& u) {
					return u.first == s2.first;
				});
				
				if(i2 != it->attributes.end()) { 
					i2->second = s2.second;
				} else {
					it->attributes.push_back(s2);
				}
			}

			for(auto &c : styling.child_styles) {
				mergeStyle(c, it->child_styles);
			}
		} else {
			it->attributes.insert(it->attributes.end(), styling.attributes.begin(), styling.attributes.end());
			it->child_styles.insert(it->child_styles.end(), styling.child_styles.begin(), styling.child_styles.end());
		}
	} else {
		// push styling and we are done
		where.push_back(styling);
	}
	
}

void ControlManager::RemoveStylingGroup(std::string name) {
	auto it = std::find_if(group_styles.begin(), group_styles.end(), [&](GroupStyle& s) {
		return s.name == name;
	});
	if(it != group_styles.end()) {
		group_styles.erase(it);
	}
}

#define dbg(x)
void ControlManager::loadXmlFirst(rapidxml::xml_node<>* node) {

	Control* control = nullptr;
	Layout layout;
	for(; node; node = node->next_sibling()) {
		dbg(std::cout << "loadXmlFirst processing: " << node->name() << "\n";)
		if(!strcmp(node->name(), "stylegroup") || !strcmp(node->name(), "groupstyle")) {
			GroupStyle groupstyle;
			groupstyle.disabled = false;
			
			for(auto a = node->first_attribute(); a; a=a->next_attribute()) {
				if(!strcmp(a->name(), "disabled")) {
					groupstyle.disabled = !strcmp(a->value(), "true");
				}
				if(!strcmp(a->name(), "id")) {
					groupstyle.name = a->value();
				}
			}
			int tag = style_tag_counter++;
			groupstyle.tag_id = tag;
			for(auto n=node->first_node(); n; n=n->next_sibling()) {
				parseStyle(n, groupstyle.styles, tag, layout);
				// std::cout << "GS size: " << groupstyle.styles.size() << "\n";
			}
			group_styles.push_back(groupstyle);
		} else if(!strcmp(node->name(), "style")) {
			parseStyle(node, group_styles.front().styles, 1, layout);
		} else if(strcmp(node->name(), "resources") == 0) {
			ResourceManager::ProcessResource(node);
		} else {
			parseAndAddControl(node, group_styles.front().styles, 1, layout);
		}
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

void ControlManager::AddControl( Control* control, bool processlayout ) {
	addControlToCache(control);
}

struct Scope {
	std::function<void()> onDestruct;
	Scope(std::function<void()> onConstruct, std::function<void()> onDestruct) {
		if(onConstruct) {
			onConstruct();
		}
		this->onDestruct = onDestruct;
	}
	~Scope() {
		// if(onDestruct) {
			onDestruct();
		// }
	}
};

Control* ControlManager::parseAndAddControl(rapidxml::xml_node<char>* node, std::vector<Styling>& push_where, int style_group_tag, Layout& layout) {
	if(!node) return 0;
	
	dbg(std::cout << "parse and add control: " << node->name() << " \n";)
	// Scope scope2([&]{creation_vector.push_back({this_widget->GetId(), this_widget->GetType()});},
				// [&]{creation_vector.pop_back();});
				
	// std::cout << "PARSE ADD control: " << this_widget->GetType() << "\n";
	if(!strcmp(node->name(), "style")) {
		// parseStyle(node, group_styles.front().styles, 1, layout);
		parseStyle(node, push_where, style_group_tag, layout);
		return 0;
	}
	
	if(!strcmp(node->name(), "br")) {
		layout.coord.x = 0;
		layout.coord.y++;
		return 0;
	}
	
	std::string type = node->name();
	std::string id;
	auto attr_id = node->first_attribute("id");
	if(attr_id) {
		id = attr_id->value();
	}
	
	Control* control = CreateControl(type,id);
	if(!control) {
		return 0;
	}
	
	// load control styles
	for(rapidxml::xml_attribute<> *attr = node->first_attribute(); attr; attr = attr->next_attribute()) {
		std::string style = std::string(attr->name());
		std::string value = std::string(attr->value());
		dbg(std::cout << "\tattr: " << style << ", " << value << "\n";)
		control->SetStyle(style, value);
	}
	
	Layout a = control->GetLayout();
	a.coord = layout.coord;
	// a += layout;
	control->SetLayout(a);
	
	// Scope scope([&]{creation_vector.push_back({id, type});},
		// [&]{creation_vector.pop_back();});
	creation_vector.push_back({id,type});
	
	// add child controls
	Layout b;
	for(node = node->first_node(); node; node=node->next_sibling()) {
		// control->parseXml(node->first_node());
		control->parseAndAddControl(node, push_where, style_group_tag, b);
	}
	creation_vector.pop_back();
	
	dbg(std::cout << "adding control " << control->GetId() << "\n";)
	AddControl(control);
	dbg(std::cout << "added control " << control->GetId() << "\n";)
	layout.coord.x++;
	return control;
}

void ControlManager::LoadXml(std::string xml_filename) {
	rapidxml::file<> f(xml_filename.c_str());
	parseXmlFile(&f);
	this_widget->ProcessLayout();
}

void ControlManager::LoadXml(std::istream& stream) {
	rapidxml::file<> f(stream);
	parseXmlFile(&f);
	this_widget->ProcessLayout();
}

void ControlManager::printCreationVector() {
	if(creation_vector.size() > 0) {
		std::string form_type;
		for(auto &s : creation_vector) {
			form_type += "/" + s.type + "#"+s.id;
		}
		std::cout << "F: " << form_type << "\n";
	} else {
		std::cout << "creation vector is empty\n";
	}
}


std::vector<ControlCreationPair> ControlManager::creation_vector;
Control* ControlManager::CreateControl(std::string tag, std::string id) {
	// creation_vector.push_back({id,tag});
	Scope scope([&]{creation_vector.push_back({id, tag});},
				[&]{creation_vector.pop_back();});
	
	Control* ctrl = createControlByXmlTag(tag);
	// printCreationVector();
	if(ctrl) {
		if(id.empty()) {
			static int control_cnt = 0;
			id = "control"+std::to_string((control_cnt++));
		}
		ctrl->SetId(id);
		ctrl->applyStyling(creation_vector);
	}
	// creation_vector.pop_back();
	return ctrl;
}
	
	
} // namespace ng
