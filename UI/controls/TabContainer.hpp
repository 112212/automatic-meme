#ifndef _H_TAB_CONTAINER
#define _H_TAB_CONTAINER

#include "../Control.hpp"
#include "Container.hpp"
namespace ng {

class TabContainer : public Widget {
	private:
		struct TabContext {
			std::string tabname;
			Control* tab;
			Widget* widget;
			int tabwidth;
		};
		
		std::vector<TabContext> m_tabs;
		int m_cur_tab;
		int m_tab_height;
		
		void onRectChange();
		void OnMouseDown( int mX, int mY );
		// void OnMouseUp( int mX, int mY );
		// void OnMWheel( int updown );
		void OnLostControl();
		// void OnMouseMove( int mX, int mY, bool mouseState );
		virtual void parseXml(rapidxml::xml_node<char>* node);
	public:
		TabContainer();
		void Render( Point pos, bool isSelected );
		Control* Clone();
		Widget* NewTab(std::string tabname);
		Widget* GetTabWidget(std::string tabname);
		Widget* GetTabWidget(int idx);
};
}
#endif
