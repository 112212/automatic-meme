#include "Button.hpp"

namespace ng {
Button::Button(std::string id) {
	SetId(id);
	setType( "button" );
	tex_text = 0;
	need_update = false;
	m_is_mouseDown = false;
	m_down_color = 0xffff0000;
	m_up_color = 0xffffffff;
	m_style.background_color = Color::Dgray;
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
			m_down_color = Color::ParseColor(value);
		_case("up_color"):
			m_up_color = Color::ParseColor(value);
	}
}

void Button::Render( Point pos, bool isSelected ) {
	const Rect& r = GetRect();
	int x = r.x + pos.x;
	int y = r.y + pos.y;
	
	Drawing().FillRect(x, y, r.w, r.h, m_style.background_color);
	
	Control::Render(pos,isSelected || isActive());
	
	if(tex_text) {
		
		Drawing().TexRect(m_text_rect.x + pos.x, m_text_rect.y + pos.y, m_text_rect.w, m_text_rect.h, tex_text);
	}
}

void Button::update_text() {
	if(!m_style.font) return;
	int color = m_is_mouseDown ? m_down_color : m_up_color;
	
	const Rect& r = GetRect();
	
	Image* img = m_style.font->GetTextImage(text, color);
	if(!img) return;
	
	if(tex_text) {
		delete tex_text;
	}
	tex_text = img;
	Size size = img->GetImageSize();
	m_text_rect.w = size.w;
	m_text_rect.h = size.h;
	m_text_rect.x = r.x + (r.w - size.w)/2;
	m_text_rect.y = r.y + (r.h - size.h)/2;
	
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

void Button::OnKeyDown( Keycode sym, Keymod mod ) {
	if(sym == KEY_TAB) {
		Control::tabToNextControl();
	} else if(sym == KEY_RETURN || sym == KEY_KP_ENTER) {
		m_is_mouseDown = true;
		update_text();
	}
}

void Button::OnKeyUp( Keycode sym, Keymod mod ) {
	if(sym == KEY_RETURN || sym == KEY_KP_ENTER) {
		m_is_mouseDown = false;
		update_text();
		emitEvent( "click" );
	}
}

Button* Button::Clone() {
	Button *btn = new Button;
	btn->m_down_color = m_down_color;
	btn->m_up_color = m_up_color;
	copyStyle(btn);
	return btn;
}

void Button::OnGetFocus() {
}

void Button::OnLostFocus() {
	m_is_mouseDown = false;
}

}
