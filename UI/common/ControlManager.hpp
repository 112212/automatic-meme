#ifndef _INTERNAL_GUI_CONTROL_MANAGER_
#define _INTERNAL_GUI_CONTROL_MANAGER_


#include "../Control.hpp"
#include <vector>


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


class ControlManager {
	private:
		int next_z_index;
		int binary_search(int z_index);
		void rescale_z_indices(int spaci);
	protected:
		std::vector<Control*> controls;
		std::vector<cache_entry> cache;
		
		void updateCache(Control* control, CacheUpdateFlag flag);
		void setZIndex(Control* control, int new_z_index);
		void addControlToCache(Control* control);
		
		void removeControlFromCache(Control* control);
	public:
		inline const std::vector<Control*> GetControls() { return controls; }
		
		ControlManager();
		~ControlManager();
};
}
#endif
