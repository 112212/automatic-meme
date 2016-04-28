#include "TextBox.hpp"
namespace ng {
TextBox::TextBox() {
	setType( TYPE_TEXTBOX );
	m_font = Fonts::GetFont( "default", 13 );
	m_text_selection.x = m_text_selection.y = 0;
	m_surf_first = 0;
	m_surf_middle = 0;
	m_surf_last = 0;
	m_text = "";
	m_last_sel = 0;
	m_cursor_pt = 0;
	m_cursor_sel = 0;
	m_first_index = 0;
	m_is_mouseDown = false;
}

TextBox::~TextBox() {
	m_text = "";
}

void TextBox::Render( SDL_Renderer* ren, SDL_Rect pos, bool isSelected ) {
	
	
	#ifdef SELECTION_MARK
		Drawing::Rect(m_rect.x+pos.x, m_rect.y+pos.y, m_rect.w, m_rect.h, isSelected ? Colors::Yellow : Colors::White );
	#else
		Drawing::Rect(m_rect.x+pos.x, m_rect.y+pos.y, m_rect.w, m_rect.h, Colors::White );
	#endif
	
	
	// rectangleColor(ren, m_rect.x, m_rect.y, m_rect.x+m_rect.w, m_rect.y+m_rect.h, Colors::Gray );
	// #ifdef SELECTION_MARK
		// rectangleColor(ren, m_rect.x, m_rect.y, m_rect.x+m_rect.w, m_rect.y+m_rect.h, isSelected ?  Colors::Yellow : Colors::White );
	// #else
		// rectangleColor(ren, m_rect.x, m_rect.y, m_rect.x+m_rect.w, m_rect.y+m_rect.h, Colors::White );
	// #endif
	
	// prvo mora da postoji prvi tekst
	if(m_surf_first) {
		// TODO: fix this
		// CSurface::OnDraw( ren, m_surf_first, m_text_loc.x+pos.x, m_text_loc.y+pos.y );
		// ako postoji prvi tekst, da li postoji drugi tekst (selekcija)
		if(m_surf_middle) {
			// TODO: fix this
			// CSurface::OnDraw( ren, m_surf_middle, m_text_loc.x + m_surf_first->w+pos.x, m_text_loc.y+pos.y );
			// ako postoji drugi tekst, da li postoji treci tekst (znaci da postoji selekcija negde izmedju)
			if(m_surf_last) {
				// TODO: fix this
				// CSurface::OnDraw( ren, m_surf_last, m_text_loc.x + m_surf_first->w + m_surf_middle->w + pos.x, m_text_loc.y + pos.y );
			}
		}
	}
	
	//Draw_HLine(surf, m_text_loc.x, m_rect.y+m_rect.h+10, getCharPos( m_maxtext ), CColors::c_yellow );
	
	if(isSelected)
		//Draw_VLine(surf, m_cursor_pt, m_rect.y+5, m_rect.y+m_rect.h-5, CColors::c_white ); 
		Drawing::VLine(m_cursor_pt, m_rect.y+5, m_rect.y+m_rect.h-5, Colors::White); 
		// vlineColor(ren, m_cursor_pt+pos.x, m_rect.y+5+pos.y, m_rect.y+m_rect.h-5+pos.y, Colors::White );
}

void TextBox::SetText( std::string text ) {
	m_text = text;
	m_text_selection.x = m_text_selection.y = 0;
	
	// pocetak teksta ...
	m_text_loc.x = m_rect.x + 5;
	m_text_loc.y = m_rect.y + int(m_rect.h * 0.15);
	
	m_first_index = 0;
	m_cursor_sel = m_text.length();
	
	updateMaxText();
	updateCursor();
	updateSelection();
}

void TextBox::SetText( const char* text ) {
	
	m_text = text;
	m_text_selection.x = m_text_selection.y = 0;
	
	// pocetak teksta ...
	m_text_loc.x = m_rect.x + 5;
	m_text_loc.y = m_rect.y + int(m_rect.h * 0.15);
	
	m_first_index = 0;
	m_cursor_sel = m_text.length();
	
	updateMaxText();
	updateCursor();
	updateSelection();
}

void TextBox::SetSelection( int start, int end ) {
	m_text_selection.x = std::max(start, 0);
	m_text_selection.y = std::min(end, (int)m_text.length() );
	
	updateSelection();
}

void TextBox::OnMouseDown( int mX, int mY ) {
	sendGuiCommand( GUI_KEYBOARD_LOCK );
	
	// generisati event za gui mozda ...
	// ili posetiti callback funkciju :)
	int sel = getSelectionPoint( mX );
	//cout << "first selection: " << sel << endl;
	m_last_sel = sel;
	m_is_mouseDown = true;
	m_cursor_sel = sel;
	updateCursor();
}

void TextBox::OnMouseUp( int mX, int mY ) {
	m_is_mouseDown = false;
	int sel = getSelectionPoint( mX );
	if(sel == m_last_sel) {
		m_text_selection.x = m_text_selection.y = 0;
		updateSelection();
	}
}

void TextBox::OnLostFocus() {
	m_is_mouseDown = false;
	// SDL_SetCursor( CCursors::defaultCursor );
}

void TextBox::OnLostControl() {
	m_text_selection.x = m_text_selection.y = 0;
	updateSelection();
}

void TextBox::updateSelection() {
	
	TTF_Font* my_font = m_font;
	
	if(m_surf_first)
		SDL_FreeSurface( m_surf_first );
		
	// izbaci ostale surfove
	if(m_surf_middle) {
		SDL_FreeSurface( m_surf_middle );
		m_surf_middle = 0;
	}
	if(m_surf_last) {
		SDL_FreeSurface( m_surf_last );
		m_surf_last = 0;
	}
	
	if(m_text_selection.x >= (int)m_text.length()) {
		m_text_selection.x = m_text_selection.y = 0;
	}
	
	int maxtext = m_maxtext;
	int len2;
	
	// ako postoji selekcija
	if(m_text_selection.x != m_text_selection.y) {
		// ako selekcija pocinje od pocetka, onda je m_surf_first shaded
		if(m_text_selection.x <= m_first_index) {
			len2 = std::min( m_text_selection.y-m_text_selection.x+1, maxtext );
			maxtext -= len2;
			
			
			m_surf_first = TTF_RenderText_Shaded( my_font, m_text.substr( m_first_index, len2 ).c_str(), {255,255,255}, {100,100,100} );
			// odavde je zavrsena selekcija, handled ...
			
			
			// znaci da postoji samo m_surf_middle(ako nije sve selektovano :) )
			if(maxtext > 0) {
				if(m_text_selection.y+1 < m_text.length())
					m_surf_middle = TTF_RenderText_Solid( my_font, m_text.substr( m_text_selection.y+1, maxtext ).c_str(), {255,255,255} );
			}
		} else { // ako selekcija ne pocinje od pocetka, onda je m_surf_first obican tekst (unselected)
			//  if(m_text_selection.x < m_first_index + m_maxtext)
			len2 = std::min( maxtext, m_text_selection.x-m_first_index );
			maxtext -= len2;
			m_surf_first = TTF_RenderText_Blended( my_font, m_text.substr(m_first_index, len2).c_str(), {255,255,255} );
			
			if(maxtext > 0) {
				len2 =  std::min(m_text_selection.y-m_text_selection.x+1, maxtext);
				maxtext -= len2;
				
				// dok middle postaje selekcija
				m_surf_middle = TTF_RenderText_Shaded( my_font, m_text.substr( m_text_selection.x, len2 ).c_str(), {255,255,255}, {100,100,100});
			}
			
			// ako nije do kraja selektovano, znaci da postoji m_surf_last (unselected)
			if(m_text_selection.y+1 < m_text.length()) {
				if(maxtext > 0)
					m_surf_last = TTF_RenderText_Solid( my_font, m_text.substr( m_text_selection.y+1, maxtext ).c_str(), {255,255,255} );
			}
		}
			
	} else {
		// ne postoji selekcija
		if(m_text.size() > 0)
		m_surf_first = TTF_RenderText_Solid( my_font, m_text.substr(m_first_index, maxtext).c_str(), {255,255,255} );
	}
	
}

void TextBox::OnSetStyle(std::string& style, std::string& value) {
	if(style == "text")
		SetText(value.c_str());
}

int TextBox::getSelectionPoint( int &mX ) {
	TTF_Font* fnt = m_font;
	
	int dummy, advance;
	if(mX < m_text_loc.x)
		return 0;
	else if( mX > m_rect.x+m_rect.w )
		return m_text.length();
	int sum=0;
	for(int i=m_first_index; i < m_text.length(); i++) {
		TTF_GlyphMetrics( fnt, m_text[i],&dummy,&dummy,&dummy,&dummy,&advance);
		if( m_text_loc.x+sum >= mX )
			return i;
		sum += advance;
	}
	return m_text.length();
}

void TextBox::OnGetFocus() {
	// SDL_SetCursor( CCursors::textCursor );
	
}

void TextBox::OnMouseMove( int mX, int mY, bool mouseState ) {
	if(m_is_mouseDown) {
		int sel = getSelectionPoint( mX );
		m_text_selection.x = m_last_sel;
		m_text_selection.y = sel;
		fixSelection();
		updateSelection();
		m_cursor_sel = sel;
		updateCursor();
	}
}

void TextBox::fixSelection() {
	if( m_text_selection.y < m_text_selection.x ) {
		int ex;
		// zamena
		ex = m_text_selection.x;
		m_text_selection.x = m_text_selection.y;
		m_text_selection.y = ex;
	}
}

void TextBox::onPositionChange() {
	if(!m_text.empty())
		SetText( m_text.c_str() ); // update poziciju teksta :)
}


void TextBox::OnKeyDown( SDL_Keycode &sym, SDL_Keymod &mod ) {
	int val = sym;
	
	if(val >= 256 && val <= 265) val -= 208;
	if(val == SDLK_KP_PERIOD) val = SDLK_PERIOD;
	//cout << val << endl;
	switch(val) {
		case SDLK_BACKSPACE:
				if(m_text.length() > 0) {
					// ako postoji selekcija, brisi sve sto je selektovano
					if(m_text_selection.x != m_text_selection.y) {
						if(m_text_selection.x >= 0) {
							m_text.erase(m_text_selection.x, m_text_selection.y-m_text_selection.x+1);
							m_cursor_sel = m_text_selection.x;
							// remove selection
							m_text_selection.x = m_text_selection.y = 0;
						}
					} else if(m_cursor_sel > 0) { // ako ne postoji selekcija, brisi tamo gde je kursor
						// pomeriti cursor
						m_text = m_text.erase(m_cursor_sel-1, 1);
						m_cursor_sel--;
						int mtext = getMaxTextBw( m_cursor_sel );
						if(mtext != m_maxtext) {
							m_maxtext = mtext;
							m_first_index = m_cursor_sel - mtext;
						}
					}
					updateCursor();
					emitEvent( EVENT_TEXTBOX_CHANGE );
				}
				break;
			case SDLK_LEFT:
				if(m_cursor_sel >= 0) {
					m_cursor_sel--;
					updateCursor();
				}
				break;
			case SDLK_RIGHT:
				if(m_cursor_sel < m_text.length()) {
					m_cursor_sel++;
					updateCursor();
				}
				break;
			case SDLK_END:
				m_cursor_sel = m_text.length();
				updateCursor();
				break;
			case SDLK_HOME:
				setFirstIndex(0);
				m_cursor_sel = 0;
				updateCursor();
				break;
			case SDLK_RETURN:
			case SDLK_KP_ENTER:
				emitEvent( EVENT_TEXTBOX_ENTER );
				break;
			default:
			
				if(m_text_selection.x != m_text_selection.y) {
					m_text.erase(m_text_selection.x, m_text_selection.y-m_text_selection.x+1);
					m_cursor_sel = m_text_selection.x;
					m_text_selection.x = m_text_selection.y = 0;
				}
				
				m_text.insert(m_cursor_sel, 1, val);
				
				// pomeriti cursor
				m_cursor_sel++;
				updateCursor();
				
				emitEvent( EVENT_TEXTBOX_CHANGE );
				break;
	}
	updateSelection();
}

int TextBox::getCharPos( int num ) {
	int sum=m_text_loc.x;
	TTF_Font* fnt = m_font;
	
	int dummy, advance;
	int len = m_first_index+num;
	for(int i=m_first_index; i < len; i++) {
		TTF_GlyphMetrics( fnt, m_text[i], &dummy, &dummy, &dummy, &dummy, &advance);
		sum += advance;
	}
	return sum;
}

void TextBox::updateCursor() {

	if(m_cursor_sel < m_first_index) {
		// u principu treba da vraca m_first_index
		if(m_cursor_sel < 0)m_cursor_sel = 0;
		m_first_index=m_cursor_sel;
		m_maxtext = getMaxText();
	} else if(m_cursor_sel-m_first_index > m_maxtext) {
		m_maxtext = getMaxTextBw( m_cursor_sel );
		m_first_index = m_cursor_sel - m_maxtext;
		// treba da inkrementira
	} else
		m_maxtext = getMaxText();
	m_cursor_pt = getCharPos( m_cursor_sel-m_first_index );
}

int TextBox::getMaxText( ) {
	TTF_Font* fnt = m_font;
	
	int dummy, advance;
	int len = m_text.length() - m_first_index;
	int sum=0;
	for(int i=0; i < len; i++) {
		TTF_GlyphMetrics( fnt, m_text[m_first_index+i], &dummy, &dummy, &dummy, &dummy, &advance);
		if( sum > m_rect.w-25 ) {
			return i+1;
		}
		sum += advance;
	}
	return len;
}

int TextBox::getMaxTextBw( int indx ) {
	TTF_Font* fnt = m_font;
	
	int dummy, advance;
	int len = indx;
	int sum=0;
	for(int i=len-1; i >= 0; i--) {
		TTF_GlyphMetrics( fnt, m_text[i], &dummy, &dummy, &dummy, &dummy, &advance);
		if( sum > m_rect.w-25 ) {
			return len-i;
		}
		sum += advance;
	}
	return len;
}

void TextBox::updateMaxText() {
	m_maxtext = getMaxText( );
}

void TextBox::setFirstIndex( int index ) {
	m_first_index = index;
	m_maxtext = getMaxText( );
}

const char* TextBox::GetText( ) {
	return m_text.c_str();
}

const char* TextBox::GetSelectedText() {
	return m_text.substr( m_text_selection.x, m_text_selection.y-m_text_selection.x+1 ).c_str();
}
}
