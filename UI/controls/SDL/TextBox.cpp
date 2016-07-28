#include "TextBox.hpp"
#include <cctype> // toupper
namespace ng {
TextBox::TextBox() : m_mousedown(false) {
	setType( TYPE_TEXTBOX );
	m_font = Fonts::GetFont( "default", 13 );
	
}

TextBox::~TextBox() {
	
}

void TextBox::Render( SDL_Rect pos, bool selected ) {
	
	
	#ifdef SELECTION_MARK
		Drawing::Rect(m_rect.x+pos.x, m_rect.y+pos.y, m_rect.w, m_rect.h, selected ? Colors::Yellow : Colors::White );
	#else
		Drawing::Rect(m_rect.x+pos.x, m_rect.y+pos.y, m_rect.w, m_rect.h, Colors::White );
	#endif
	
	
	
}

void TextBox::SetText( std::string text ) {
	size_t pos = 0;
	size_t last = 0;
	m_lines.clear();
	while((pos = text.find_first_of("\n", last)) != std::string::npos) {
		TextLine line;
		// line.tex = 
		line.text = text.substr(last, pos-last);
		m_lines.push_back(line);
		last = pos;
	}
}

void TextBox::SetSelection( Point start, Point end ) {
	
}

void TextBox::OnMouseDown( int x, int y ) {
	sendGuiCommand( GUI_KEYBOARD_LOCK );
}

void TextBox::OnMouseUp( int x, int y ) {
	m_mousedown = false;

}

void TextBox::OnLostFocus() {
	m_mousedown = false;
	// SDL_SetCursor( CCursors::defaultCursor );
}

void TextBox::OnLostControl() {
	m_text_selection.x = m_text_selection.y = 0;
	updateSelection();
}

void TextBox::updateSelection() {
	
	
	
}

void TextBox::STYLE_FUNC(value) {
	STYLE_SWITCH {
		_case("value"):
			SetText(value);
	}
		
}

void TextBox::onPositionChange() {
	
}

void TextBox::OnGetFocus() {
	// SDL_SetCursor( CCursors::textCursor );
}

void TextBox::OnMouseMove( int mX, int mY, bool mouseState ) {
	
}

void TextBox::fixSelection() {
	
}


void TextBox::OnKeyDown( SDL_Keycode &sym, SDL_Keymod mod ) {
	int val = sym;
	
	// if(val >= 256 && val <= 265) val -= 208;
	
	// cout << val << endl;
	switch(val) {
		case SDLK_BACKSPACE: {
				emitEvent( event::change );
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
			// setFirstIndex(0);
			m_cursor_sel = 0;
			updateCursor();
			break;
		case SDLK_RETURN:
		case SDLK_KP_ENTER:
			emitEvent( event::enter );
			break;
			
		case SDLK_LSHIFT:
		case SDLK_RSHIFT:
		case SDLK_LCTRL:
		case SDLK_RCTRL:
		case SDLK_LALT:
		case SDLK_RALT:
		
			break;
			
		default:
		
			if(mod & KMOD_CTRL) {
				switch(val) {
					case 'c': // copy
						cout << "Ctrl-c\n";
						break;
					case 'v': // paste
						cout << "Ctrl-v\n";
						break;
					case 'a': // select all
						cout << "Ctrl-a\n";
						break;
				}
				break;
			}
			
			if(val == SDLK_KP_0) {
				val = '0';
			} else if(val >= SDLK_KP_1 && val <= SDLK_KP_9) {
				val = val - SDLK_KP_1 + 0x31;
			} else if(val == SDLK_KP_DIVIDE) {
				val = '/';
			}
		
			if(val == SDLK_KP_PERIOD) 
				val = SDLK_PERIOD;
				
			if(mod & KMOD_SHIFT) {
				val = toupper(val);
			}
			
			
			
			emitEvent( event::change );
			break;
	}
}


void TextBox::updateCursor() {

}


std::string TextBox::GetText( ) {
	return std::string("");
}

std::string TextBox::GetSelectedText() {
}

}

