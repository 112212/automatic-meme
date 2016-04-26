#include "TextBox.hpp"

TextBox::TextBox() {
	setType( TYPE_TEXTBOX );
	characterSize = 13;
	m_font = Fonts::GetFont( "default" );
	m_text.setFont( m_font );
	m_text.setCharacterSize( characterSize );
	m_text_selection.x = m_text_selection.y = 0;
	m_str = "";
	m_last_sel = 0;
	m_cursor_pt = 0;
	m_cursor_sel = 0;
	m_first_index = 0;
	m_is_mouseDown = false;
	//~ m_cursor.setSize( sf::Vector2f( 1, m_rect.h ) );
	//~ m_rectShape.setSize( sf::Vector2f( m_rect.w, m_rect.h ) );
	m_cursor.setOutlineColor( sf::Color::White );
	m_cursor.setOutlineThickness( 1 );
	m_rectShape.setFillColor( sf::Color::Transparent );
	m_rectShape.setOutlineColor( sf::Color::White );
	m_rectShape.setOutlineThickness( 1 );
	m_selection.setFillColor( sf::Color::Blue );
}

TextBox::~TextBox() {
}

void TextBox::Render( sf::RenderTarget& ren, sf::RenderStates state,  bool isSelected ) {
	ren.draw( m_selection, state );
	if(isSelected)
		ren.draw( m_cursor, state );
	ren.draw( m_text, state );
	ren.draw( m_rectShape, state );
}

void TextBox::SetText( std::string text ) {
	m_str = text;
	m_text_selection.x = m_text_selection.y = 0;
	
	// pocetak teksta ...
	m_text_loc.x = m_rect.x + 5;
	m_text_loc.y = m_rect.y + int(m_rect.h * 0.15);
	
	m_first_index = 0;
	m_cursor_sel = m_str.length();
	
	updateMaxText();
	updateCursor();
	updateSelection();
}

void TextBox::SetText( const char* text ) {
	
	m_str = text;
	m_text_selection.x = m_text_selection.y = 0;
	
	m_first_index = 0;
	m_cursor_sel = m_str.length();
	
	updateMaxText();
	updateCursor();
	updateSelection();
}

void TextBox::SetSelection( int start, int end ) {
	m_text_selection.x = std::max(start, 0);
	m_text_selection.y = std::min(end, (int)m_str.length() );
	
	updateSelection();
}

void TextBox::OnMouseDown( int mX, int mY ) {
	sendGuiCommand( GUI_KEYBOARD_LOCK );
	int sel = getSelectionPoint( mX );
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
	//~ SDL_SetCursor( CCursors::defaultCursor );
	
}

void TextBox::OnLostControl() {
	m_text_selection.x = m_text_selection.y = 0;
	updateSelection();
}

void TextBox::updateSelection() {
	
	if(m_text_selection.x >= (int)m_str.length()) {
		m_text_selection.x = m_text_selection.y = 0;
	}
	
	// ako postoji selekcija
	if(m_text_selection.x != m_text_selection.y) {
		int start = std::max( 0, m_text_selection.x - m_first_index );
		int startPos = getCharDistance( m_first_index, m_first_index+start );
		m_selection.setPosition( m_rect.x + startPos, m_rect.y+m_rect.h*0.25 );
		m_selection.setSize( sf::Vector2f( std::min( m_rect.w - startPos - 1, getCharDistance( start+m_first_index, m_text_selection.y ) ), m_rect.h*0.5 ) );
	} else {
		m_selection.setSize( sf::Vector2f( 0, 0 ) );
	}
}


int TextBox::getSelectionPoint( int mX ) {

	if(mX < m_rect.x)
		return 0;
	else if( mX > m_rect.x+m_rect.w )
		return m_str.length();
	int sum=0;
	for(int i=m_first_index; i < m_str.length(); i++) {
		const sf::Glyph &g = m_font.getGlyph( m_str[i], characterSize, false );
		if( m_rect.x+sum >= mX )
			return i;
		sum += g.advance;
	}
	return m_str.length();
}

void TextBox::OnGetFocus() {
	//~ SDL_SetCursor( CCursors::textCursor );
}

void TextBox::OnMouseMove( int mX, int mY, bool mouseState ) {
	if(m_is_mouseDown) {
		int sel = getSelectionPoint( mX );
		//~ debug(sel);
		m_text_selection.x = m_last_sel;
		m_text_selection.y = sel;
		fixSelection();
		updateSelection();
		m_cursor_sel = sel;
		updateCursor();
	}
}

/*
	for m_text_selection it must apply !
		.x < .y
*/
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
	m_cursor.setSize( sf::Vector2f( 0.5, m_rect.h*0.5 ) );
	//~ m_cursor.setPosition( m_rect.x, m_rect.y + m_rect.h );
	m_rectShape.setPosition( m_rect.x, m_rect.y );
	m_rectShape.setSize( sf::Vector2f( m_rect.w, m_rect.h ) );
	m_text.setPosition( m_rect.x + 2, m_rect.y + m_rect.h*0.3 );
	//~ m_text.setSize( sf::Vector2f( m_rect.w, m_rect.h ) );
	updateMaxText();
	updateCursor();
	updateSelection();
}

usi Key = sf::Keyboard::Key;
void TextBox::OnKeyDown( sf::Event::KeyEvent &sym ) {
	Key val = sym.code;
	
	switch(val) {
		case Key::BackSpace:
				if(m_str.length() > 0) {
					// ako postoji selekcija, brisi sve sto je selektovano
					if(m_text_selection.x != m_text_selection.y) {
						if(m_text_selection.x >= 0) {
							m_str.erase(m_text_selection.x, m_text_selection.y-m_text_selection.x);
							m_cursor_sel = m_text_selection.x;
							// remove selection
							m_text_selection.x = m_text_selection.y = 0;
						}
					} else if(m_cursor_sel > 0) { // ako ne postoji selekcija, brisi tamo gde je kursor
						// pomeriti cursor
						m_str = m_str.erase(m_cursor_sel-1, 1);
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
			case Key::Left:
				if(m_cursor_sel >= 0) {
					m_cursor_sel--;
					updateCursor();
				}
				break;
			case Key::Right:
				if(m_cursor_sel < m_str.length()) {
					m_cursor_sel++;
					updateCursor();
				}
				break;
			case Key::End:
				m_cursor_sel = m_str.length();
				updateCursor();
				break;
			case Key::Home:
				setFirstIndex(0);
				m_cursor_sel = 0;
				updateCursor();
				break;
			case Key::Return:
				emitEvent( EVENT_TEXTBOX_ENTER );
				break;
			default:
			
				char cval;
				if( val == Key::Period )
					cval = '.';
				else if( (cval = SFMLCodeToChar( val, sym.shift )) == '.' ) {
					break;
				}
				
				if(m_text_selection.x != m_text_selection.y) {
					m_str.erase(m_text_selection.x, m_text_selection.y-m_text_selection.x);
					m_cursor_sel = m_text_selection.x;
					m_text_selection.x = m_text_selection.y = 0;
				}
				
				m_str.insert(m_cursor_sel, 1, cval);
				
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
	int len = m_first_index+num;
	for(int i=m_first_index; i < len; i++) {
		const sf::Glyph &g = m_font.getGlyph( m_str[i], characterSize, false );
		sum += g.advance;
	}
	return sum;
}

int TextBox::getCharDistance( int startChar, int endChar ) {
	int sum=0;
	for(int i=startChar; i < endChar; i++) {
		const sf::Glyph &g = m_font.getGlyph( m_str[i], characterSize, false );
		sum += g.advance;
	}
	return sum;
}

void TextBox::updateCursor() {

	if(m_cursor_sel < m_first_index) {
		// u principu treba da vraca m_first_index
		if(m_cursor_sel < 0)
			m_cursor_sel = 0;
		m_first_index = m_cursor_sel;
		m_maxtext = getMaxText();
	} else if(m_cursor_sel-m_first_index > m_maxtext) {
		m_maxtext = getMaxTextBw( m_cursor_sel );
		m_first_index = m_cursor_sel - m_maxtext;
		// treba da inkrementira
	} else {
		m_maxtext = getMaxText();
	}
	m_text.setStri( m_str.substr( m_first_index, m_maxtext ) );
	//~ m_cursor_pt = getCharPos( m_cursor_sel-m_first_index );
	m_cursor_pt = getCharDistance( m_first_index, m_cursor_sel );
	
	m_cursor.setPosition( m_rect.x + m_cursor_pt + 3, m_rect.y + m_rect.h*0.25 );
}

const int textMargin = 17;
int TextBox::getMaxText( ) {
	int len = m_str.length() - m_first_index;
	int sum=0;
	for(int i=0; i < len; i++) {
		const sf::Glyph &g = m_font.getGlyph( m_str[m_first_index + i], characterSize, false );
		if( sum > m_rect.w-textMargin ) {
			return i+1;
		}
		sum += g.advance;
	}
	return len;
}

int TextBox::getMaxTextBw( int indx ) {
	int len = indx;
	int sum=0;
	for(int i=len-1; i >= 0; i--) {
		const sf::Glyph &g = m_font.getGlyph( m_str[i], characterSize, false );
		if( sum > m_rect.w-textMargin ) {
			return len-i;
		}
		sum += g.advance;
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
	return m_str.c_str();
}

const char* TextBox::GetSelectedText() {
	return m_str.substr( m_text_selection.x, m_text_selection.y-m_text_selection.x+1 ).c_str();
}
