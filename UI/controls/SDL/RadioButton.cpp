#include "RadioButton.hpp"
#include "../../common/SDL/CSurface.h"
namespace ng {
RadioButton::RadioButton() {
	setType( TYPE_RADIOBUTTON );
	m_surf_text = 0;
	m_group = 0;
	m_isSelected = false;
	m_font = Fonts::GetFont( "default", 13 );
}


RadioButton::~RadioButton() {
	
}

void RadioButton::Render( SDL_Renderer* ren, SDL_Rect pos, bool isSelected ) {
	int x = m_rect.x + pos.x;
	int y = m_rect.y + pos.y;
	#ifdef SELECTION_MARK
	if(isSelected)
		Draw_Rect(ren, x, y, m_rect.w, m_rect.h, isSelected ? Colors::Yellow : Colors::Yellow );
	#endif
	Draw_Circle(ren, x+RADIO_BUTTON_RADIUS, y+m_rect.h/2, RADIO_BUTTON_RADIUS, Colors::White );
	if(m_surf_text)
		CSurface::OnDraw( ren, m_surf_text, m_text_loc.x+pos.x, m_text_loc.y+pos.y );
	if(m_isSelected)
		Draw_FillCircle(ren, x+RADIO_BUTTON_RADIUS, y+m_rect.h/2, RADIO_BUTTON_RADIUS-2, Colors::Yellow );
}


void RadioButton::SetText( std::string text ) {
	m_text = text;
	if(m_surf_text)
		SDL_FreeSurface( m_surf_text );
	
	m_surf_text = TTF_RenderText_Solid( m_font, m_text.c_str(), {255,255,255} );
	if(m_surf_text) {
		m_text_loc.x = m_rect.x + RADIO_BUTTON_RADIUS + 15;
		m_text_loc.y = m_rect.y + RADIO_BUTTON_RADIUS;
	}
}

void RadioButton::OnSetStyle(std::string& style, std::string& value) {
	if(style == "text") {
		SetText(value);
	}
}

void RadioButton::OnMouseDown( int mX, int mY ) {
	// generisati event za gui mozda ...
	// ili posetiti callback funkciju :)
	if(m_surf_text) {
		SDL_FreeSurface(m_surf_text);
		m_surf_text = TTF_RenderText_Solid( m_font, m_text.c_str(), {0,255,0} );
	}
	m_is_mouseDown = true;
}

void RadioButton::handleRadioButtonChange() {
	std::vector<Control*> controls;
	if(getWidget()) {
		controls = getWidgetControls();
	} else {
		controls = getEineControls();
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

void RadioButton::OnMouseUp( int mX, int mY ) {
	m_is_mouseDown = false;
	if(m_surf_text) {
		SDL_FreeSurface(m_surf_text);
		m_surf_text = TTF_RenderText_Solid( m_font, m_text.c_str(), {255,255,255} );
	}
	if(check_collision(mX, mY)) {
		if( !m_isSelected ) {
			emitEvent( EVENT_RADIOBUTTON_CHANGE );
			handleRadioButtonChange();
			m_isSelected = true;
		}
	}
}

void RadioButton::OnLostFocus() {
	m_is_mouseDown = false;
}


void RadioButton::onPositionChange() {
	if(m_text.size())
		SetText( m_text ); // update poziciju teksta :)
}

}
