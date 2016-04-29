#include "CheckBox.hpp"
#include "../../common/SDL/Drawing.hpp"
namespace ng {
CheckBox::CheckBox() {
	setType( TYPE_CHECKBOX );
	m_surf_text = 0;
	m_text = 0;
	m_isChecked = false;
	m_font = Fonts::GetFont( "default", 13 );
}


CheckBox::~CheckBox() {
	if(m_text)
		delete[] m_text;
}

void CheckBox::Render(  SDL_Rect pos, bool isSelected ) {
	int x = m_rect.x + pos.x;
	int y = m_rect.y + pos.y;
	
	#ifdef SELECTION_MARK
		if(isSelected)
			Drawing::Rect(x, y, m_rect.w, m_rect.h, isSelected ? Colors::Yellow : Colors::Yellow );
	#endif
	
	
	Drawing::Rect(x+CHECKBOX_SHIFT, y, CHECKBOX_SIZE, CHECKBOX_SIZE, Colors::White );
	
	if(m_surf_text) {
		// TODO: fix this
		// CSurface::OnDraw( ren, m_surf_text, m_text_loc.x+pos.x, m_text_loc.y+pos.y );
	}
	
	if(m_isChecked) {
		int x1 = x+CHECKBOX_OKVIR+CHECKBOX_SHIFT;
		int y1 = y+CHECKBOX_OKVIR;
		int x2 = x+CHECKBOX_SIZE-CHECKBOX_OKVIR+CHECKBOX_SHIFT;
		int y2 = y+CHECKBOX_SIZE-CHECKBOX_OKVIR;
		
		
		Drawing::Line(x1, y1, x2, y2, Colors::Yellow );
		Drawing::Line(x1, y2, x2, y1, Colors::Yellow );
		
	}
	
	
	
}


void CheckBox::SetText( const char* text ) {
	m_text = new char[ strlen( text ) ];
	strcpy( m_text, text );
	updateText();
}

void CheckBox::updateText() {
	if(m_surf_text)
		SDL_FreeSurface( m_surf_text );
	
	m_surf_text = TTF_RenderText_Solid( m_font, m_text, {255,255,255} );
	if(m_surf_text) {
		m_text_loc.x = m_rect.x + CHECKBOX_SIZE + 15;
		m_text_loc.y = m_rect.y;
	}
}

void CheckBox::OnMouseDown( int mX, int mY ) {
	// generisati event za gui mozda ...
	// ili posetiti callback funkciju :)
	if(m_surf_text) {
		SDL_FreeSurface(m_surf_text);
		m_surf_text = TTF_RenderText_Solid( m_font, m_text, {0,255,0} );
	}
}

void CheckBox::OnMouseUp( int mX, int mY ) {
	if(m_surf_text) {
		SDL_FreeSurface(m_surf_text);
		m_surf_text = TTF_RenderText_Solid( m_font, m_text, {255,255,255} );
	}
	if(check_collision(mX, mY)) {
		m_isChecked = !m_isChecked;
		emitEvent( EVENT_CHECKBOX_CHANGE );
	}
}

void CheckBox::OnLostFocus() {
}


void CheckBox::onPositionChange() {
	if(m_text)
		updateText(); // update poziciju teksta :)
}

void CheckBox::OnSetStyle(std::string& style, std::string& value) {
	if(style == "text")
		SetText(value.c_str());
}

}
