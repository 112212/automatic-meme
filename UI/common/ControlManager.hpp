#ifndef _INTERNAL_GUI_CONTROL_MANAGER_
#define _INTERNAL_GUI_CONTROL_MANAGER_


#include "../Control.hpp"
#include <vector>
#include <string>
#include <istream>
namespace ng {

struct cache_entry {
	controlType type;
	int z_index;
	bool visible;
	bool interactible;
	bool isWidget;
	bool custom_check;
	Rect rect;
	Control* control;
};


class GuiEngine;
class Widget;
class Control;

class ControlManager {
	private:
		int next_z_index;
		int binary_search(int z_index);
		void rescale_z_indices(int spacing);
	protected:
		Widget* this_widget;
		GuiEngine* this_engine;
		std::vector<Control*> controls;
		std::vector<cache_entry> cache;
		
		void updateCache(Control* control, Control::CacheUpdateFlag flag);
		void setZIndex(Control* control, int new_z_index);
		void addControlToCache(Control* control);
		void removeControlFromCache(Control* control);
	public:
		inline const std::vector<Control*>& GetControls() { return controls; }
		void ApplyAnchoring();
		static void RegisterControl(std::string tag, std::function<Control*()> control_constructor);
		static Control* CreateControl(std::string tag);
		void LoadXml(std::string xml_filename);
		void LoadXml(std::istream& stream);
		
		ControlManager(Widget* c);
		ControlManager(GuiEngine* c);
		~ControlManager();
};
}
#endif
