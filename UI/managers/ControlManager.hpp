#ifndef _INTERNAL_GUI_CONTROL_MANAGER_
#define _INTERNAL_GUI_CONTROL_MANAGER_


#include <vector>
#include <string>
#include <istream>
#include <functional>
#include "../common.hpp"


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
	// bool custom_check;
	Rect rect;
	Control* control;
};


class Gui;
class Widget;
struct Layout;

class ControlManager {
	private:
		int next_z_index;
		int binary_search(int z_index);
		void rescale_z_indices(int spacing);
		Point coords;
		Control* get(const std::string& id);
		cache_entry getControlCache(Control* control);
		
		void parseXmlFile(rapidxml::file<char>* f);
		void loadXmlFirst(rapidxml::xml_node<char>* node);
	protected:
		Widget* this_widget;
		Gui* this_engine;
		std::vector<Control*> controls;
		std::vector<cache_entry> cache;
		
		void printCacheZIndex();
		
		void updateCache(Control* control, CacheUpdateFlag flag);
		void setZIndex(Control* control, int new_z_index);
		void addControlToCache(Control* control);
		void removeControlFromCache(Control* control);
		void sendToFront(Control* control);
		void sendToBack(Control* control);
	public:
		void parseAndAddControl(rapidxml::xml_node<char>* node, Layout& layout);
		
		inline const std::vector<Control*>& GetControls() { return controls; }
		void RemoveControls();
		void ProcessLayout(bool asRoot=false);
		void BreakRow();
		
		static void RegisterControl(std::string tag, std::function<Control*()> control_constructor);
		static Control* CreateControl(std::string tag, std::string id="");
		void LoadXml(std::string xml_filename);
		void LoadXml(std::istream& stream);
		virtual void AddControl( Control* control );
		
		
		ControlManager(Widget* c);
		ControlManager(Gui* c);
		// ControlManager();
		~ControlManager();
		
		template<typename C>
		C* Get(const std::string id) {
			return static_cast<C*>(get(id));
		}
};
}

#endif
