// #include <SDL2/SDL2_gfxPrimitives.h>
#include "../common/SDL/Drawing.hpp"
#include "Button.hpp"
#include <SDL2/SDL_opengl.h>

namespace ng {
Button::Button() {
	setType( TYPE_BUTTON );
	initEventVector(max_events);
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
		m_font = Fonts::GetFont( "default", 25 );
		need_update = false;
	#endif
	m_is_mouseDown = false;
	m_down_color = 0xffff0000;
	m_up_color = 0xffffffff;
	m_backcolor = Colors::Dgray;
}

Button::~Button() {
}

void Button::onFontChange() {
	update_text();
}


void Button::STYLE_FUNC(value) {
	STYLE_SWITCH {
		_case("value"):
			SetText(value);
		_case("down_color"):
			m_down_color = Colors::ParseColor(value);
		_case("up_color"):
			m_up_color = Colors::ParseColor(value);
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
		this->text.setString( _text );
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
	void Button::Render( Point pos, bool isSelected ) {
		const Rect& r = GetRect();
		int x = r.x + pos.x;
		int y = r.y + pos.y;
		
		Drawing::FillRect(x, y, r.w, r.h, m_backcolor);
		
		Control::Render(pos,isSelected || isActive());
		
		Drawing::TexRect(m_text_rect.x + pos.x, m_text_rect.y + pos.y, m_text_rect.w, m_text_rect.h, tex_text);
	}

	void Button::update_text() {
		if(!m_font) return;
		int color = m_is_mouseDown ? m_down_color : m_up_color;
		
		SDL_Color c;
		c.r = (color >> 16) & 0xff;
		c.g = (color >> 8) & 0xff;
		c.b = color & 0xff;
		c.a = (color >> 24) & 0xff;
		
		const Rect& r = GetRect();
		SDL_Surface* surf = TTF_RenderText_Blended( m_font, text.c_str(), c );
		
		if(!surf) return;
		tex_text = Drawing::GetTextureFromSurface(surf, tex_text);
		m_text_rect.w = surf->w;
		m_text_rect.h = surf->h;
		m_text_rect.x = r.x + (r.w - surf->w)/2;
		m_text_rect.y = r.y + (r.h - surf->h)/2;
		SDL_FreeSurface(surf);
	}
	
	void Button::SetText( std::string _text ) {
		text = _text;
		update_text();
	}
	
	void Button::OnMouseDown( int mX, int mY ) {
		m_is_mouseDown = true;
		update_text();
	}

	void Button::OnMouseUp( int mX, int mY ) {
		
		m_is_mouseDown = false;
		update_text();
		if(check_collision(mX, mY)) {
			emitEvent( event::click );
		}
	}
	
	void Button::onPositionChange() {
		update_text();
	}

#endif

void Button::OnKeyDown( SDL_Keycode &sym, SDL_Keymod mod ) {
	if(sym == SDLK_TAB) {
		Control::tabToNextControl();
	} else if(sym == SDLK_RETURN || sym == SDLK_KP_ENTER) {
		m_is_mouseDown = true;
		update_text();
	}
}

void Button::OnKeyUp( SDL_Keycode &sym, SDL_Keymod mod ) {
	if(sym == SDLK_RETURN || sym == SDLK_KP_ENTER) {
		m_is_mouseDown = false;
		update_text();
		emitEvent( event::click );
	}
}

Button* Button::Clone() {
	Button *btn = new Button;
	*btn = *this;
	return btn;
}

void Button::OnGetFocus() {
}

void Button::OnLostFocus() {
	m_is_mouseDown = false;
}

}
