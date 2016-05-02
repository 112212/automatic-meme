#ifndef _H_GRID_CONTAINER
#define _H_GRID_CONTAINER

#include "../Widget.hpp"
#include "../Gui.hpp"
namespace ng {
// events
enum {
	
};


class GridContainer : public Widget {
	private:
		#ifdef USE_SFML
		sf::RectangleShape m_rectShape;
		#endif
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
		#ifdef USE_SFML
		void Render( sf::RenderTarget& ren, sf::RenderStates state, bool isSelected );
		#elif USE_SDL
		void Render( SDL_Renderer* ren, SDL_Rect position, bool isSelected );
		#endif
		
		
		void AddItem( Control* control );
		void SetGrid( int x, int y );
};
}
#endif
