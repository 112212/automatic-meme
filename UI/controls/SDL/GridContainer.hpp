#ifndef _H_GRID_CONTAINER
#define _H_GRID_CONTAINER

#include "../../Widget.hpp"
namespace ng {

class GridContainer : public Widget {
	private:
		int m_grid_w, m_grid_h;
		bool m_is_mouseDown;
		int m_num_controls;
		// GuiEngine mngGui;
		void onPositionChange();
		int getPointedControl( int x, int y );
		
	public:
		GridContainer();
		// void OnMouseDown( int mX, int mY );
		// void OnMouseUp( int mX, int mY );
		// void OnMWheel( int &updown );
		// void OnLostControl();
		// void OnMouseMove( int mX, int mY, bool mouseState );
		// void OnKeyDown( sf::Event::KeyEvent &sym );
		void Render( Point position, bool isSelected );
		
		void AddItem( Control* control );
		void SetGrid( int x, int y );
};
}
#endif
