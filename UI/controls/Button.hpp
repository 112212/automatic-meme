#ifndef NG_BUTTON_HPP
#define NG_BUTTON_HPP

#include "../Control.hpp"
namespace ng {
enum {
	EVENT_BUTTON_CLICK
};

class Button : public Control {
	private:
		#ifdef USE_SFML
			sf::Text text;
			sf::RectangleShape rect1;
		#elif USE_SDL
			SDL_Texture* tex_text;
			TTF_Font* font;
			SDL_Rect m_text_loc;
			bool need_update;
			std::string text;
		#endif
		Point text_loc;
		bool m_is_mouseDown;
		
		const char* ctext;
		void onPositionChange();
		#ifdef USE_SDL
			void update_text();
		#endif
		void OnSetStyle(std::string& style, std::string& value);
		void OnMouseDown( int mX, int mY );
		void OnMouseUp( int mX, int mY );
		void OnLostFocus();
		void OnGetFocus();
		#ifdef USE_SFML
			void Render( sf::RenderTarget& ren, sf::RenderStates state, bool isSelected );
		#elif USE_SDL
			void Render( SDL_Rect pos, bool isSelected );
		#endif
	public:
		Button();
		~Button();
		
		void SetText( const char* text );
};
}
#endif
