#ifndef NG_CONTROL_MANAGER
#define NG_CONTROL_MANAGER


#include <vector>
#include <string>
#include <istream>
#include <functional>
#include "../common.hpp"
#include <map>


namespace rapidxml {
	template<typename ch> class xml_node;
	template<typename ch> class file;
}

namespace ng {
	
	
class Control;
struct cache_entry {
	const char* type;
	int z_index;
	bool visible;
	bool interactible;
	bool isWidget;
	Rect rect;
	Control* control;
};


class Gui;
class Control;
struct Layout;

struct Styling {
	std::string style_for;
	std::vector<std::pair<std::string, std::string>> attributes;
	std::vector<Styling> child_styles;
};

struct GroupStyle {
	std::string name;
	bool disabled;
	int tag_id;
	std::vector<Styling> styles;
};

struct ControlCreationPair {
	std::string id;
	std::string type;
};



class ControlManager {
	private:
		friend class Gui;
		ControlManager();
		
		Point coords;

		// cache
		int next_z_index;
		cache_entry getControlCache(Control* control);
		int binary_search(int z_index);
		void rescale_z_indices(int spacing);
		//
		
		static Control* tryExtendedTags(std::string tag);
		static std::map<std::string, std::function<Control*()>> m_extended_tags;
		static Control* createControlByXmlTag(std::string tag);
		
		void parseXmlFile(rapidxml::file<char>* f);
		void loadXmlFirst(rapidxml::xml_node<char>* node);
		
		void parseStyle(rapidxml::xml_node<char>* node, std::vector<Styling>& push_where, int style_group_tag, Layout& layout);
		void mergeStyle(Styling& s, std::vector<Styling>& where);
	protected:
		static void printCreationVector();
		Control* this_widget;
		std::vector<Control*> controls;
		static std::vector<GroupStyle> group_styles;
		static std::vector<ControlCreationPair> creation_vector;
		static void RemoveStylingGroup(std::string name);
		
		// cache
		std::vector<cache_entry> cache;
		void printCacheZIndex();
		void updateCache(Control* control, CacheUpdateFlag flag);
		void setZIndex(Control* control, int new_z_index);
		void addControlToCache(Control* control);
		void removeControlFromCache(Control* control);
		//
		
		void sendToFront(Control* control);
		void sendToBack(Control* control);
	public:
		Control* parseAndAddControl(rapidxml::xml_node<char>* node, Layout& layout);
		
		inline const std::vector<Control*>& GetControls() { return controls; }
		void RemoveControls();
		void BreakRow();
		// void RegisterControl(std::string tag, std::function<Control*()> control_constructor);
		
		static void RegisterControl(std::string tag, std::function<Control*()> control_constructor);
		static Control* CreateControl(std::string tag, std::string id="");
		void LoadXml(std::string xml_filename);
		void LoadXml(std::istream& stream);
		virtual void AddControl( Control* control, bool processlayout=true );
		
		
		ControlManager(Control* c);
		// ControlManager();
		~ControlManager();
};
}

#endif
