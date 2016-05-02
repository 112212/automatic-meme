#ifndef _H__CONTAINER
#define _H__CONTAINER

#include "../Control.hpp"
#include "../Widget.hpp"
#include "ScrollBar.hpp"


namespace ng {
class Container : public Widget {
	private:
		#ifdef USE_SFML
			sf::RectangleShape m_rectShape;
		#endif
		int m_grid_w, m_grid_h;
		bool m_is_mouseDown;
		int m_num_controls;
		int m_tx, m_ty;
		int max_v, max_h;
		bool overflow_v, overflow_h;
		ScrollBar* m_scroll_v;
		ScrollBar* m_scroll_h;
		void onPositionChange();
		void onOverflow();
		bool isOnVerticalScrollbar( int x, int y );
		bool isOnHorizontalScrollbar( int x, int y );
		inline void transformCoords( int &x, int &y );
		static int depth;
		Widget* innerWidget;
		
	public:
		Container();
		~Container();
		
		#ifdef USE_SFML
			void Render( sf::RenderTarget& ren, sf::RenderStates state, bool isSelected );
		#elif USE_SDL
			void Render( SDL_Rect position, bool isSelected );
		#endif
		void OnMWheel( int updown );
		void OnMouseDown( int mX, int mY );
		void OnMouseUp( int x, int y );
		void OnMouseMove( int x, int y, bool lmb );
		
		void AddItem( Control* control );
		void AddControl( Control* control );
		
};

}
#endif
