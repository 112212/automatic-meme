#include "CheckBox.hpp"
#include "../../common/SDL/Drawing.hpp"
namespace ng {
CheckBox::CheckBox() {
	setType( TYPE_CHECKBOX );
	m_surf_text = 0;
	m_text = 0;
	tex_text = 0;
	m_isChecked = false;
	m_font = Fonts::GetFont( "default", 13 );
}


CheckBox::~CheckBox() {
	if(m_text) delete[] m_text;
}

void CheckBox::Render( Point pos, bool isSelected ) {
	const Rect& rect = GetRect();
	int x = rect.x + pos.x;
	int y = rect.y + pos.y + 5;
	
	Drawing::Rect(x+CHECKBOX_SHIFT, y, CHECKBOX_SIZE, CHECKBOX_SIZE, Colors::White );
	
	if(m_surf_text) {
		Drawing::TexRect(m_text_loc.x+pos.x, m_text_loc.y+pos.y + 4, m_surf_text->w, m_surf_text->h, tex_text);
	}
	
	if(m_isChecked) {
		int x1 = x+CHECKBOX_OKVIR+CHECKBOX_SHIFT;
		int y1 = y+CHECKBOX_OKVIR;
		int x2 = x+CHECKBOX_SIZE-CHECKBOX_OKVIR+CHECKBOX_SHIFT;
		int y2 = y+CHECKBOX_SIZE-CHECKBOX_OKVIR;
		
		Drawing::Line(x1, y1, x2, y2, Colors::Yellow );
		Drawing::Line(x1, y2, x2, y1, Colors::Yellow );
		
	}
	
	Control::Render(pos,isSelected);
	
}


void CheckBox::SetText( const char* text ) {
	m_text = new char[ strlen( text ) ];
	strcpy( m_text, text );
	updateText();
}

void CheckBox::updateText() {
	if(m_surf_text)
		SDL_FreeSurface( m_surf_text );
	
	m_surf_text = TTF_RenderText_Blended( m_font, m_text, {255,255,255} );
	if(m_surf_text) {
		m_text_loc.x = GetRect().x + CHECKBOX_SIZE + 15;
		m_text_loc.y = GetRect().y;
	}
	tex_text = Drawing::GetTextureFromSurface(m_surf_text, tex_text);
}

void CheckBox::OnMouseDown( int mX, int mY ) {
	if(m_surf_text) {
		SDL_FreeSurface(m_surf_text);
		m_surf_text = TTF_RenderText_Blended( m_font, m_text, {0,255,0} );
		tex_text = Drawing::GetTextureFromSurface(m_surf_text, tex_text);
	}
}

void CheckBox::OnMouseUp( int mX, int mY ) {
	if(m_surf_text) {
		SDL_FreeSurface(m_surf_text);
		m_surf_text = TTF_RenderText_Blended( m_font, m_text, {255,255,255} );
		tex_text = Drawing::GetTextureFromSurface(m_surf_text, tex_text);
	}
	if(check_collision(mX, mY)) {
		m_isChecked = !m_isChecked;
		emitEvent( change );
	}
}

void CheckBox::OnLostFocus() {
	
}


CheckBox* CheckBox::Clone() {
	CheckBox* c = new CheckBox;
	*c = *this;
	return c;
}

void CheckBox::onPositionChange() {
	if(m_text) updateText(); // update poziciju teksta :)
}

void CheckBox::Check() {
	m_isChecked = true;
	emitEvent( change );
}

void CheckBox::Uncheck() {
	m_isChecked = false;
}

void CheckBox::SetValue( bool check ) {
	 { m_isChecked = check; }
}

bool CheckBox::IsSelected() {
	return m_isChecked;
}

void CheckBox::STYLE_FUNC(value) {
	STYLE_SWITCH {
		_case("value"):
			SetText(value.c_str());
		_case("checked"):
			SetValue( value == "true" );
	}
}

}
