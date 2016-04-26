#include <SDL2/SDL2_gfxPrimitives.h>
#include "Button.hpp"

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
	if(style == "text") {
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
	void Button::Render( SDL_Renderer* ren, SDL_Rect pos, bool isSelected ) {
		int x = m_rect.x + pos.x;
		int y = m_rect.y + pos.y;
		
		if(need_update) {
			update_text(ren);
		}
		
		boxColor(ren, x, y, x+m_rect.w, y+m_rect.h, 0xff505050);
		#ifdef SELECTION_MARK
			rectangleColor(ren, x, y, x+m_rect.w, y+m_rect.h, isSelected ?  Colors::Yellow : Colors::White );
		#else
			rectangleColor(ren, x, y, x+m_rect.w, y+m_rect.h, Colors::White );
		#endif
		
		// SDL_SetRenderDrawColor(ren,255,0,0,255);
		pos.x = m_text_loc.x + pos.x;
		pos.y = m_text_loc.y + pos.y;
		pos.w = m_text_loc.w;
		pos.h = m_text_loc.h;
		SDL_RenderCopy( ren, tex_text, 0, &pos);
	}

	void Button::update_text(SDL_Renderer* ren) {
		if(!font) cout << "Button: font error\n";
		SDL_Color c;
		if(m_is_mouseDown)
			c = {0x0,0xff,0x0,0xff};
		else
			c = {0xff,0xff,0xff,0xff};
		SDL_Surface* surf = TTF_RenderText_Blended( font, text.c_str(), c );
		SDL_Texture* tex = SDL_CreateTextureFromSurface( ren, surf );
		SDL_QueryTexture(tex, 0,0, &m_text_loc.w, &m_text_loc.h);
		if(surf) {
			m_text_loc.x = m_rect.x + (m_rect.w - surf->w)/2;
			m_text_loc.y = m_rect.y + int(m_rect.h * 0.15);
		}

		tex_text = tex;
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
		if(check_collision(mX, mY)) {
			emitEvent( EVENT_BUTTON_CLICK );
		}
	}
	
	void Button::onPositionChange() {
	}

#endif







void Button::OnGetFocus() {
}
void Button::OnLostFocus() {
	m_is_mouseDown = false;
}

}
