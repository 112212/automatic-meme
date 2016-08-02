#include "RadioButton.hpp"
#include "../../common/SDL/Drawing.hpp"
namespace ng {
RadioButton::RadioButton() {
	setType( TYPE_RADIOBUTTON );
	m_surf_text = 0;
	m_group = 0;
	m_isSelected = false;
	m_font = Fonts::GetFont( "default", 13 );
	tex_text = 0;
}


RadioButton::~RadioButton() {
	
}

void RadioButton::Render( SDL_Rect pos, bool isSelected ) {
	const Rect& rect = GetRect();
	int x = rect.x + pos.x;
	int y = rect.y + pos.y;
	#ifdef SELECTION_MARK
	if(isSelected)
		Drawing::Rect(x, y, rect.w, rect.h, isSelected ? Colors::Yellow : Colors::Yellow );
	#endif
	Drawing::Circle( x+RADIO_BUTTON_RADIUS, y+rect.h/2, RADIO_BUTTON_RADIUS, Colors::White );
	if(m_surf_text) {
		// TODO: fix this
		// CSurface::OnDraw( ren, m_surf_text, m_text_loc.x+pos.x, m_text_loc.y+pos.y );
		Drawing::TexRect( m_text_loc.x+pos.x, m_text_loc.y+pos.y-5, m_surf_text->w, m_surf_text->h, tex_text );
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
		m_text_loc.y = GetRect().y + RADIO_BUTTON_RADIUS;
	}
}

void RadioButton::STYLE_FUNC(value) {
	STYLE_SWITCH {
		_case("value"):
			SetText(value);
	}
}

void RadioButton::OnMouseDown( int mX, int mY ) {
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
		m_surf_text = TTF_RenderText_Blended( m_font, m_text.c_str(), {255,255,255} );
		tex_text = Drawing::GetTextureFromSurface(m_surf_text, tex_text);
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
		SetText( m_text );
}

}
