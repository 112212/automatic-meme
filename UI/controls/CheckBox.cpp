#include "CheckBox.hpp"
namespace ng {
CheckBox::CheckBox() : Control() {
	setType( "checkbox" );
	m_text = "";
	tex_text = 0;
	m_isChecked = false;
	m_font = Fonts::GetFont( "default", 13 );
	tmode = false;
}


CheckBox::~CheckBox() {
}

void CheckBox::Render( Point pos, bool isSelected ) {
	const Rect& rect = GetRect();
	int x = rect.x + pos.x;
	int y = rect.y + pos.y + 5;
	
	Drawing().Rect(x+CHECKBOX_SHIFT, y, CHECKBOX_SIZE, CHECKBOX_SIZE, Color::White );
	
	if(tex_text) {
		Size s = tex_text->GetImageSize();
		Drawing().TexRect(m_text_loc.x+pos.x, m_text_loc.y+pos.y + 4, s.w, s.h, tex_text);
	}
	
	if(m_isChecked) {
		int x1 = x+CHECKBOX_FRAME+CHECKBOX_SHIFT;
		int y1 = y+CHECKBOX_FRAME;
		int x2 = x+CHECKBOX_SIZE-CHECKBOX_FRAME+CHECKBOX_SHIFT;
		int y2 = y+CHECKBOX_SIZE-CHECKBOX_FRAME;
		
		Drawing().Line(x1, y1, x2, y2, Color::Yellow );
		Drawing().Line(x1, y2, x2, y1, Color::Yellow );
	}
	
	Control::Render(pos,isSelected);
}


void CheckBox::SetText( std::string text ) {
	m_text = text;
	updateText();
}

void CheckBox::updateText() {
	m_text_loc.x = GetRect().x + CHECKBOX_SIZE + 15;
	m_text_loc.y = GetRect().y;
	tex_text = m_font->GetTextImage( m_text, 0xffffff );
}

void CheckBox::OnMouseDown( int mX, int mY, MouseButton which_button ) {
	tex_text = m_font->GetTextImage( m_text, 0x00ff00 );
	if(tmode && !m_isChecked) {
		m_isChecked = true;
		emitEvent("change");
	}
}

void CheckBox::OnMouseUp( int mX, int mY, MouseButton which_button ) {
	tex_text = m_font->GetTextImage( m_text, 0xffffff );
	if(tmode) {
		if(m_isChecked) {
			m_isChecked = false;
		}
		emitEvent("change");
	} else {
		if(CheckCollision(mX, mY)) {
			m_isChecked = !m_isChecked;
			emitEvent( "change" );
		}
	}
}

void CheckBox::OnLostFocus() {
	
}


CheckBox* CheckBox::Clone() {
	CheckBox* c = new CheckBox;
	copyStyle(c);
	return c;
}

void CheckBox::onRectChange() {
	updateText();
}

void CheckBox::Check() {
	if(!m_isChecked) {
		m_isChecked = true;
		emitEvent( "change" );
	}
}

void CheckBox::Uncheck() {
	if(m_isChecked) {
		emitEvent( "change" );
		m_isChecked = false;
	}
}

void CheckBox::SetValue( bool check ) {
	m_isChecked = check;
}

bool CheckBox::IsSelected() {
	return m_isChecked;
}

bool CheckBox::IsChecked() {
	return m_isChecked;
}

void CheckBox::OnSetStyle(std::string& style, std::string& value) {
	STYLE_SWITCH {
		_case("value"):
			SetText(value.c_str());
		_case("checked"):
			SetValue( value == "true" );
		_case("tmode"):
			tmode = ( value == "true" );
	}
}

}
