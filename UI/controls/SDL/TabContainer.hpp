#ifndef _H_TAB_CONTAINER
#define _H_TAB_CONTAINER

#include "../../Control.hpp"
#include "../../common/SDL/Drawing.hpp"
#include "Container.hpp"
namespace ng {

class TabContainer : public Widget {
	private:
		int characterSize;
		TTF_Font* m_font;
		std::vector<std::string> tabnames;
		std::vector<Container*> tabcontainers;
		Container* cur_container;
		int selectedTab;
		void onPositionChange();
		int getTextWidth( const std::string &txt );
		inline int getTabHeight();
		
		void OnMouseDown( int mX, int mY );
		// void OnMouseUp( int mX, int mY );
		// void OnMWheel( int updown );
		void OnLostControl();
		// void OnMouseMove( int mX, int mY, bool mouseState );
	public:
		TabContainer();
		
		void SetFont( std::string font_name );
		
		// add item to current tab container
		void AddItem( Control* control );
		
		// pushes tab and all previously added items to it
		void AddTab( std::string tab_name );
};
}
#endif
