#include "RadioButton.hpp"
namespace ng {
RadioButton::RadioButton() {
	setType( "radiobutton" );
	m_surf_text = 0;
	m_group = 0;
	m_isSelected = false;
	tex_text = 0;
	m_checksize = 14;
}


RadioButton::~RadioButton() {
	if(tex_text) {
		delete tex_text;
	}
	if(m_surf_text) {
		delete m_surf_text;
	}
}

void RadioButton::Render( Point pos, bool isSelected ) {
	const Rect& rect = GetRect();
	int x = rect.x + pos.x;
	int y = rect.y + pos.y;
	
	Control::Render(pos,isSelected);
	
	Drawing().Circle( x+m_checksize, y+rect.h/2, m_checksize, Color::White );
	if(tex_text) {
		Size s = tex_text->GetImageSize();
		// Drawing().TexRect( m_text_loc.x+pos.x, m_text_loc.y+pos.y+2, s.w, s.h, tex_text );
		Drawing().TexRect( m_text_loc.x+pos.x, pos.y+rect.y+(rect.h-s.h)*0.5, s.w, s.h, tex_text );
	}
	if(m_isSelected) {
		Drawing().FillCircle( x+m_checksize, y+rect.h/2, m_checksize, Color::Yellow );
	}
	
	
}


void RadioButton::SetText( std::string text ) {
	m_text = text;
	if(!m_style.font) {
		return;
	}
	tex_text = m_style.font->GetTextImage( m_text, 0xffffff );
	m_text_loc.x = GetRect().x + m_checksize + 15;
	m_text_loc.y = GetRect().y;
}

void RadioButton::OnSetStyle(std::string& style, std::string& value) {
	STYLE_SWITCH {
		_case("value"):
			SetText(value);
		_case("selected"):
			if(toBool(value)) {
				Select();
			}
		_case("group"):
			m_group = std::stoi(value);
		_case("checksize"):
			m_checksize = std::stoi(value);
	}
}

void RadioButton::OnMouseDown( int mX, int mY, MouseButton which_button ) {
	if(!m_style.font) {
		return;
	}
	tex_text = m_style.font->GetTextImage( m_text, 0x00ff00 );
	m_is_mouseDown = true;
}

void RadioButton::handleRadioButtonChange() {
	std::vector<Control*> controls;
	controls = getParentControls();
	int len = controls.size();
	int group = m_group;
	for(int i=0; i < len; i++) {
		// if( controls[i]->GetType() == "radiobutton" ) {
		if( dynamic_cast<RadioButton*>(controls[i]) ) {
			RadioButton* rb = ((RadioButton*)controls[i]);
			if( rb != this ) {
				if( rb->GetGroup() == group ) {
					rb->Unselect();
				}
			}
		}
	}
}

void RadioButton::OnMouseUp( int mX, int mY, MouseButton which_button ) {
	m_is_mouseDown = false;
	if(!m_style.font) {
		return;
	}
	const Rect& r = GetRect();
	tex_text = m_style.font->GetTextImage( m_text, 0xffffff );
	if(CheckCollision(Point(mX,mY)+r)) {
		if( !m_isSelected ) {
			emitEvent( "change" );
			handleRadioButtonChange();
			m_isSelected = true;
		}
	}
}

void RadioButton::Select() { 
	m_isSelected = true; 
	handleRadioButtonChange(); 
	emitEvent( "change" ); 
}

void RadioButton::OnLostFocus() {
	m_is_mouseDown = false;
}


void RadioButton::onRectChange() {
	if(m_text.size()) {
		SetText( m_text );
	}
}

RadioButton* RadioButton::Clone() {
	RadioButton* r = new RadioButton();
	copyStyle(r);
	return r;
}


}
