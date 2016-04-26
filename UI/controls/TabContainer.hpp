#ifndef _H_TAB_CONTAINER
#define _H_TAB_CONTAINER

#include "../Control.hpp"
#include "Container.hpp"
#include "../common/SFML/Drawi.hpp"
namespace ng {
// events
enum {
};

class TabContainer : public Widget {
	private:
		int characterSize;
		#ifdef USE_SFML
			sf::Font font;
			sf::RectangleShape esc;
			sf::Texture tabtex;
			sf::Texture esctex;
			std::vector<sf::RectangleShape> tabrects;
			std::vector<sf::Text> tabtexts;
		#elif USE_SDL
			TTF_Font* m_font;
		#endif
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
		
		#ifdef USE_SFML
			void Render( sf::RenderTarget& ren, sf::RenderStates state, bool isSelected );
		#elif USE_SDL
			
		#endif
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
