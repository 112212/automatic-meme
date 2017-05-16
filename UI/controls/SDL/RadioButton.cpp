#include "RadioButton.hpp"
#include "../../common/SDL/Drawing.hpp"
namespace ng {
RadioButton::RadioButton() {
	setType( "radiobutton" );
	m_surf_text = 0;
	m_group = 0;
	m_isSelected = false;
	m_font = Fonts::GetFont( "default", 13 );
	tex_text = 0;
}


RadioButton::~RadioButton() {
	
}

void RadioButton::Render( Point pos, bool isSelected ) {
	const Rect& rect = GetRect();
	int x = rect.x + pos.x;
	int y = rect.y + pos.y;
	
	Control::Render(pos,isSelected);
	
	Drawing::Circle( x+RADIO_BUTTON_RADIUS, y+rect.h/2, RADIO_BUTTON_RADIUS, Colors::White );
	if(m_surf_text) {
		Drawing::TexRect( m_text_loc.x+pos.x, m_text_loc.y+pos.y+2, m_surf_text->w, m_surf_text->h, tex_text );
	}
	if(m_isSelected) {
		Drawing::FillCircle(x+RADIO_BUTTON_RADIUS, y+rect.h/2, RADIO_BUTTON_RADIUS-2, Colors::Yellow );
	}
	
	
}


void RadioButton::SetText( std::string text ) {
	m_text = text;
	if(m_surf_text)
		SDL_FreeSurface( m_surf_text );
	
	m_surf_text = TTF_RenderText_Blended( m_font, m_text.c_str(), {255,255,255} );
	tex_text = Drawing::GetTextureFromSurface(m_surf_text, tex_text);
	if(m_surf_text) {
		m_text_loc.x = GetRect().x + RADIO_BUTTON_RADIUS + 15;
		m_text_loc.y = GetRect().y;
	}
}

void RadioButton::OnSetStyle(std::string& style, std::string& value) {
	STYLE_SWITCH {
		_case("value"):
			SetText(value);
		_case("selected"):
			if(value == "true") {
				Select();
			}
	}
}

void RadioButton::OnMouseDown( int mX, int mY, MouseButton which_button ) {
	if(m_surf_text) {
		SDL_FreeSurface(m_surf_text);
		m_surf_text = TTF_RenderText_Blended( m_font, m_text.c_str(), {0,255,0} );
		tex_text = Drawing::GetTextureFromSurface(m_surf_text, tex_text);
	}
	m_is_mouseDown = true;
}

void RadioButton::handleRadioButtonChange() {
	std::vector<Control*> controls;
	if(getWidget()) {
		controls = getWidgetControls();
	} else {
		controls = getEngineControls();
	}
	int len = controls.size();
	int group = m_group;
	for(int i=0; i < len; i++) {
		if( !strcmp(controls[i]->GetType(), "radiobutton") ) {
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
	if(m_surf_text) {
		SDL_FreeSurface(m_surf_text);
		m_surf_text = TTF_RenderText_Blended( m_font, m_text.c_str(), {255,255,255} );
		tex_text = Drawing::GetTextureFromSurface(m_surf_text, tex_text);
	}
	if(check_collision(mX, mY)) {
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
	if(m_text.size())
		SetText( m_text );
}

RadioButton* RadioButton::Clone() {
	RadioButton* r = new RadioButton();
	copyStyle(r);
	return r;
}


}
