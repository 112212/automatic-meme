// #include <SDL2/SDL2_gfxPrimitives.h>
#include "../common/SDL/Drawing.hpp"
#include "Button.hpp"
#include <SDL/SDL_opengl.h>

namespace ng {
Button::Button() {
	setType( TYPE_BUTTON );
	initEventVector(1);
	#ifdef USE_SFML
		text.setFont( Fonts::GetFont( "default" ) );
		text.setCharacterSize( 30 );
		rect1.setPosition( m_rect.x, m_rect.y );
		rect1.setSize( sf::Vector2f( m_rect.w, m_rect.h ) );
		rect1.setFillColor( sf::Color( Colors::Dgray ) );
		rect1.setOutlineColor( sf::Color::White );
		rect1.setOutlineThickness( 1 );
	#elif USE_SDL
		tex_text = 0;
		font = Fonts::GetFont( "default", 25 );
		need_update = false;
	#endif
	m_is_mouseDown = false;
}

Button::~Button() {
}

void Button::OnSetStyle(std::string& style, std::string& value) {
	if(style == "value") {
		SetText(value.c_str());
	}
}

#ifdef USE_SFML
	void Button::Render( sf::RenderTarget& ren, sf::RenderStates state, bool isSelected ) {
		
		ren.draw( rect1 );
		ren.draw( text );
	}

	void Button::SetText( const char* _text ) {
		ctext = _text;
		text.setPosition( m_rect.x, m_rect.y );
		this->text.setStri( _text );
	}
	
	void Button::onPositionChange() {
		text.setPosition( m_rect.x, m_rect.y );
		rect1.setPosition( m_rect.x, m_rect.y );
		rect1.setSize( sf::Vector2f( m_rect.w, m_rect.h ) );
	}

	void Button::OnMouseDown( int mX, int mY ) {
		rect1.setOutlineColor( sf::Color::Green );
		text.setColor( sf::Color::Green );
		m_is_mouseDown = true;
	}

	void Button::OnMouseUp( int mX, int mY ) {
		rect1.setOutlineColor( sf::Color::White );
		text.setColor( sf::Color::White );
		m_is_mouseDown = false;
		if(check_collision(mX, mY)) {
			emitEvent( EVENT_BUTTON_CLICK );
		}
	}

#elif USE_SDL
	void Button::Render( SDL_Rect pos, bool isSelected ) {
		const Rect& r = GetRect();
		int x = r.x + pos.x;
		int y = r.y + pos.y;
		
		if(need_update) {
			update_text();
			need_update = false;
		}
		
		Drawing::FillRect(x, y, r.w, r.h, Colors::Dgray);
		
		#ifdef SELECTION_MARK
			Drawing::Rect(x, y, r.w, r.h, isSelected ?  Colors::Yellow : Colors::White);
		#else
			Drawing::Rect(x, y, r.w, r.h, Colors::White);
		#endif
		
		pos.x = m_text_loc.x + pos.x;
		pos.y = m_text_loc.y + pos.y;
		pos.w = m_text_loc.w;
		pos.h = m_text_loc.h;
		
		Drawing::TexRect(pos.x, pos.y, pos.w, pos.h, tex_text);
	}

	void Button::update_text() {
		const Rect& r = GetRect();
		SDL_Color c;
		if(m_is_mouseDown)
			c = {0x0,0xff,0x0,0xff};
		else
			c = {0xff,0xff,0xff,0xff};
		
		SDL_Surface* surf = TTF_RenderText_Blended( font, text.c_str(), c );
		m_text_loc.w = surf->w;
		m_text_loc.h = surf->h;
		tex_text = Drawing::GetTextureFromSurface(surf, tex_text);
		SDL_FreeSurface(surf);
		if(surf) {
			m_text_loc.x = r.x + (r.w - surf->w)/2;
			m_text_loc.y = r.y + int(r.h * 0.15);
		}
		
	}
	
	void Button::SetText( const char* _text ) {
		text = _text;
		need_update = true;
	}
	
	void Button::OnMouseDown( int mX, int mY ) {
		m_is_mouseDown = true;
		need_update = true;
	}

	void Button::OnMouseUp( int mX, int mY ) {
		
		m_is_mouseDown = false;
		need_update = true;
		if(check_collision(mX, mY)) {
			emitEvent( EVENT_BUTTON_CLICK );
		}
	}
	
	void Button::onPositionChange() {
		need_update = true;
	}

#endif


void Button::OnGetFocus() {
}
void Button::OnLostFocus() {
	m_is_mouseDown = false;
}

}
