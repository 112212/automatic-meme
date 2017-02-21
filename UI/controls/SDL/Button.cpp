// #include <SDL2/SDL2_gfxPrimitives.h>
#include "../../common/SDL/Drawing.hpp"
#include "Button.hpp"
// #include <SDL2/SDL_opengl.h>

namespace ng {
Button::Button() {
	setType( "button" );
	tex_text = 0;
	need_update = false;
	m_is_mouseDown = false;
	m_down_color = 0xffff0000;
	m_up_color = 0xffffffff;
	m_style.background_color = Colors::Dgray;
}

Button::~Button() {
}

void Button::onFontChange() {
	update_text();
}


void Button::OnSetStyle(std::string& style, std::string& value) {
	STYLE_SWITCH {
		_case("value"):
			SetText(value);
		_case("down_color"):
			m_down_color = Colors::ParseColor(value);
		_case("up_color"):
			m_up_color = Colors::ParseColor(value);
	}
}

void Button::Render( Point pos, bool isSelected ) {
	const Rect& r = GetRect();
	int x = r.x + pos.x;
	int y = r.y + pos.y;
	
	Drawing::FillRect(x, y, r.w, r.h, m_style.background_color);
	
	Control::Render(pos,isSelected || isActive());
	
	Drawing::TexRect(m_text_rect.x + pos.x, m_text_rect.y + pos.y, m_text_rect.w, m_text_rect.h, tex_text);
}

void Button::update_text() {
	if(!m_style.font) return;
	int color = m_is_mouseDown ? m_down_color : m_up_color;
	
	SDL_Color c;
	c.r = (color >> 16) & 0xff;
	c.g = (color >> 8) & 0xff;
	c.b = color & 0xff;
	c.a = (color >> 24) & 0xff;
	
	const Rect& r = GetRect();
	SDL_Surface* surf = TTF_RenderText_Blended( m_style.font, text.c_str(), c );
	
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

void Button::OnMouseDown( int mX, int mY, MouseButton which_button ) {
	m_is_mouseDown = true;
	update_text();
}

void Button::OnMouseUp( int mX, int mY, MouseButton which_button ) {
	
	m_is_mouseDown = false;
	update_text();

}

void Button::onRectChange() {
	update_text();
}

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
		emitEvent( "click" );
	}
}

Button* Button::Clone() {
	Button *btn = new Button;
	copyStyle(btn);
	return btn;
}

void Button::OnGetFocus() {
}

void Button::OnLostFocus() {
	m_is_mouseDown = false;
}

}
