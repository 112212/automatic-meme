#include "CheckBox.hpp"
namespace ng {
CheckBox::CheckBox() : Control() {
	setType( "checkbox" );
	m_text = "";
	tex_text = 0;
	m_isChecked = false;
	m_font = Fonts::GetFont( "default", 13 );
	m_checksize = 13;
	tmode = false;
}


CheckBox::~CheckBox() {
}

void CheckBox::Render( Point pos, bool isSelected ) {
	const Rect& rect = GetRect();
	int x = rect.x + pos.x;
	int y = rect.y + pos.y + (rect.h-m_checksize)/2;
	
	Drawing().Rect(x+CHECKBOX_SHIFT, y, m_checksize, m_checksize, Color::White );
	
	if(tex_text) {
		Size s = tex_text->GetImageSize();
		Drawing().TexRect(m_text_loc.x+pos.x, m_text_loc.y+pos.y + (rect.h-s.h)/2-2, s.w, s.h, tex_text);
	}
	
	if(m_isChecked) {
		int x1 = x+CHECKBOX_FRAME+CHECKBOX_SHIFT;
		int y1 = y+CHECKBOX_FRAME;
		int x2 = x+m_checksize-CHECKBOX_FRAME+CHECKBOX_SHIFT;
		int y2 = y+m_checksize-CHECKBOX_FRAME;
		
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
	if(m_font) {
		tex_text = m_font->GetTextImage( m_text, 0xffffff );
	}
}

void CheckBox::OnMouseDown( int mX, int mY, MouseButton which_button ) {
	tex_text = m_font->GetTextImage( m_text, 0x00ff00 );
	if(tmode && !m_isChecked) {
		m_isChecked = true;
		emitEvent("change",  {(int)IsChecked()});
	}
}

void CheckBox::OnMouseUp( int mX, int mY, MouseButton which_button ) {
	tex_text = m_font->GetTextImage( m_text, 0xffffff );
	if(tmode) {
		if(m_isChecked) {
			m_isChecked = false;
		}
		emitEvent("change",  {(int)IsChecked()});
	} else {
		if(CheckCollision(Point(mX,mY)+GetRect())) {
			m_isChecked = !m_isChecked;
			emitEvent( "change",  {(int)IsChecked()} );
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
		emitEvent( "change", {(int)IsChecked()} );
	}
}

void CheckBox::Uncheck() {
	if(m_isChecked) {
		emitEvent( "change", {(int)IsChecked()} );
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
			SetValue( toBool(value));
		_case("tmode"):
			tmode = ( toBool(value) );
		_case("checksize"):
			m_checksize = std::stoi(value);
	}
}

}
