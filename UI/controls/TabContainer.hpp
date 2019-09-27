#ifndef NG_TAB_CONTAINER
#define NG_TAB_CONTAINER

#include "../Control.hpp"
#include "Container.hpp"
namespace ng {

class TabContainer : public Control {
	private:
		struct TabContext {
			std::string tabname;
			Control* tab;
			Control* widget;
			int tabwidth;
		};
		
		std::vector<TabContext> m_tabs;
		int m_cur_tab;
		int m_tab_height;
		
		void onRectChange();
		void OnMouseDown( int mX, int mY );
		// void OnMouseUp( int mX, int mY );
		// void OnMWheel( int updown );
		// void OnMouseMove( int mX, int mY, bool mouseState );
		virtual void parseXml(rapidxml::xml_node<char>* node);
	public:
		TabContainer();
		void Render( Point pos, bool isSelected );
		Control* Clone();
		Control* NewTab(std::string tabname);
		Control* GetTabWidget(std::string tabname);
		Control* GetTabWidget(int idx);
};
}
#endif
