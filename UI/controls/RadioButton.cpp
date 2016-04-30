#include "RadioButton.hpp"

namespace ng {

RadioButton::RadioButton() {
	setType( TYPE_RADIOBUTTON );
	initEventVector(1);
	ctext = 0;
	m_group = 0;
	m_isSelected = false;
	
	text.setFont( Fonts::GetFont( "default" ) );
	text.setCharacterSize( 30 );
	radio.setFillColor( sf::Color::Transparent );
	radio.setOutlineColor( sf::Color::White );
	active.setFillColor( sf::Color::Green );
	radio.setOutlineThickness( 1 );
}

RadioButton::~RadioButton() {
	
}

void RadioButton::Render( sf::RenderTarget& ren, sf::RenderStates state, bool isSelected ) {
	
	ren.draw( radio );
	if( m_isSelected ) {
		ren.draw( active );
	}
	ren.draw( text );
}


void RadioButton::SetText( const char* _text ) {
	ctext = _text;
	this->text.setStri( _text );
	/*
	if(m_font_index >= 0) {
		if(m_surf_text)
			SDL_FreeSurface( m_surf_text );
		
		m_surf_text = TTF_RenderText_Solid( CFonts::g_fonts[ m_font_index ], m_text, CColors::s_white );
		if(m_surf_text) {
			m_text_loc.x = m_rect.x + RADIO_BUTTON_RADIUS + 15;
			m_text_loc.y = m_rect.y + RADIO_BUTTON_RADIUS;
		}
	}
	*/
}


void RadioButton::OnMouseDown( int mX, int mY ) {
	m_is_mouseDown = true;
}

void RadioButton::OnMouseUp( int mX, int mY ) {
	m_is_mouseDown = false;

	if(check_collision(mX, mY)) {
		if( !m_isSelected ) {
			m_isSelected = true;
			emitEvent( EVENT_RADIOBUTTON_CHANGE );
			handleRadioButtonChange();
		}
	}
}

void RadioButton::OnLostFocus() {
	m_is_mouseDown = false;
}


void RadioButton::onPositionChange() {
	radio.setPosition( m_rect.x, m_rect.y+m_rect.h*0.25 );
	//radio.setRadius( m_rect.h/4 ); 
	radio.setRadius( RADIO_BUTTON_RADIUS ); 
	active.setPosition( m_rect.x + m_rect.w*.001, m_rect.y+m_rect.h*.001 + m_rect.h*0.25 );
	//active.setRadius( m_rect.h/4 * 0.9 );
	active.setRadius( RADIO_BUTTON_RADIUS );
	text.setPosition( m_rect.x + RADIO_BUTTON_RADIUS*4 + 2, m_rect.y - m_rect.h/4 + m_rect.h*0.25 );
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
		if( controls[i]->GetType() == TYPE_RADIOBUTTON ) {
			RadioButton* rb = ((RadioButton*)controls[i]);
			if( rb != this ) {
				if( rb->GetGroup() == group ) {
					rb->Unselect();
				}
			}
		}
	}
}

void RadioButton::Select() { 
	m_isSelected = true;
	handleRadioButtonChange();
	emitEvent( EVENT_RADIOBUTTON_CHANGE ); 
}

void RadioButton::OnSetStyle(std::string& style, std::string& value) {
	if(style == "text") {
		SetText(value.c_str());
	}
}

}
