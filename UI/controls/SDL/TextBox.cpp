#include "TextBox.hpp"
#include <SDL2/SDL_opengl.h>
#include <cctype> // toupper
#include <list>
namespace ng {
TextBox::TextBox() : m_mousedown(false), m_position{0,0}, m_cursor{0,0}, m_anchor{-1,-1}, 
	m_cursor_max_x(0) {
	m_multiline = false;
	m_terminal_mode = false;
	setType( TYPE_TEXTBOX );
	m_font = Fonts::GetFont( "default", 13 );
	m_cursor_blink_counter = 0;
	m_cursor_blinking_rate = 300;
	m_terminal_max_messages = 100;
}

TextBox::~TextBox() {
	
}

void TextBox::Render( Point pos, bool selected ) {
	
	const Rect& rect = this->GetRect();
	Point r = rect.Offset(pos);
	
	Drawing::FillRect(rect.x+pos.x, rect.y+pos.y, rect.w, rect.h, 0x05055050 );
	
	#ifdef SELECTION_MARK
		Drawing::Rect(rect.x+pos.x, rect.y+pos.y, rect.w, rect.h, selected ? Colors::Blue : Colors::GetColor(50,50,100) );
	#else
		Drawing::Rect(rect.x+pos.x, rect.y+pos.y, rect.w, rect.h, Colors::White );
	#endif
	
	
	int w,h;
	Drawing::GetResolution(w,h);
	// TODO: tweak scissor a little
	glScissor(r.x, h-(r.y+rect.h), rect.w, rect.h);
	glEnable(GL_SCISSOR_TEST);
	
	int j;
	
	std::string piece = m_lines[m_cursor.y].text.substr(0, m_position.x);
	int sz = Fonts::getTextSize( m_font, piece );
	if(m_anchor.x != -1) {
		j=0;
		Point p1,p2;
		sortPoints(p1,p2);
		int yy=m_position.y;
		for( auto i = m_lines.begin()+m_position.y; i != m_lines.end(); i++,j++,yy++) {
			if(5+j*m_line_height+i->h > rect.h) break;
			if(yy < p1.y || yy > p2.y) continue;
			
			if(yy == p1.y && p1.y == p2.y) {
				std::string pd = m_lines[yy].text.substr(p1.x, p2.x-p1.x);
				std::string pd1 = m_lines[yy].text.substr(0, p1.x);
				int selw = Fonts::getTextSize( m_font, pd );
				int selw1 = Fonts::getTextSize( m_font, pd1 );
				Drawing::FillRect( rect.x-sz+5+selw1, rect.y+5+j*m_line_height, selw, i->h, 0x808080);
			} else if(yy == p1.y) {
				std::string pd = m_lines[yy].text.substr(p1.x);
				int selw = Fonts::getTextSize( m_font, pd );
				Drawing::FillRect( rect.x-sz+5+i->w-selw, rect.y+5+j*m_line_height, selw, i->h, 0x808080);
			} else if(yy == p2.y) {
				std::string pd = m_lines[yy].text.substr(0,p2.x);
				int selw = Fonts::getTextSize( m_font, pd );
				Drawing::FillRect( rect.x-sz+5, rect.y+5+j*m_line_height, selw, i->h, 0x808080);
			} else {
				Drawing::FillRect( rect.x-sz+5, rect.y+5+j*m_line_height, i->w, i->h, 0x808080);
			}
		}
	}
	
	
	j=0;
	for( auto i = m_lines.begin()+m_position.y; i != m_lines.end(); i++,j++) {
		if(5+j*m_line_height+i->h > rect.h) break;
		Drawing::TexRect( rect.x-sz+5, rect.y+5+j*m_line_height, i->w, i->h, i->tex);
	}
	
	glDisable(GL_SCISSOR_TEST);
	
	if(++m_cursor_blink_counter > m_cursor_blinking_rate && m_cursor.y-m_position.y < m_lines.size()) {
		if(m_cursor_blink_counter > 2*m_cursor_blinking_rate)
			m_cursor_blink_counter = 0;
		std::string piece = m_lines[m_cursor.y].text.substr(m_position.x, m_cursor.x-m_position.x);
		Drawing::Rect(Fonts::getTextSize( m_font, piece )+rect.x+5, 
			(m_cursor.y-m_position.y)*m_line_height+rect.y+5, 1, m_line_height, 0xffffffff);
	}
	
	
}

void TextBox::SetText( std::string text ) {
	if(m_lines.size() > 0)
		SetSelection( Point(m_lines.back().text.size(), m_lines.size()-1), {0,0} );
	m_cursor = Point(0,0);
	PutTextAtCursor(text);
	m_cursor = Point(0,0);
	updatePosition();
}

void TextBox::SetSelection( Point start, Point end ) {
	m_anchor = start;
	m_cursor = end;
}


void TextBox::OnMouseUp( int x, int y ) {
	m_mousedown = false;

}

void TextBox::OnLostFocus() {
	m_mousedown = false;
	// SDL_SetCursor( CCursors::defaultCursor );
}

void TextBox::OnLostControl() {

}


void TextBox::STYLE_FUNC(value) {
	STYLE_SWITCH {
		_case("value"):
			SetText(value);
		_case("multiline"):
			SetMultilineMode(value == "true");
		_case("terminal_mode"):
			SetTerminalMode(value == "true");
			
	}
		
}

void TextBox::onFontChange() {
	m_text_max = Fonts::getMaxTextRep( m_font, 'A', GetRect().w );
	m_line_height = TTF_FontHeight(m_font);
	m_lines_max = GetRect().h / m_line_height;
}
void TextBox::onPositionChange() {
	onFontChange();
}

void TextBox::OnGetFocus() {
	// SDL_SetCursor( CCursors::textCursor );
}

void TextBox::OnMouseDown( int x, int y ) {
	sendGuiCommand( GUI_KEYBOARD_LOCK );
	const Rect& rect = GetRect();
	if(check_collision(x,y)) {
		Point pt;
		std::string piece = m_lines[m_cursor.y].text.substr(0, m_position.x);
		int sz = Fonts::getTextSize( m_font, piece );
		pt.y = (y-rect.y-10) / m_line_height + m_position.y;
		
		if(pt.y < 0) pt.y = 0;
		else if(pt.y > m_lines.size()-1) pt.y = m_lines.size()-1;
		
		pt.x = (x-rect.x-10) + sz;
		if(pt.x > 0) pt.x = Fonts::getMaxText( m_font, m_lines[pt.y].text, pt.x );
		
		if(pt.x < 0) pt.x = 0;
		else if(pt.x > m_lines[pt.y].text.size()) pt.x = m_lines[pt.y].text.size();
		
		m_anchor = pt;
		m_cursor = pt;
		m_cursor_max_x = m_cursor.x;
		updatePosition();
	}
}

void TextBox::OnMouseMove( int x, int y, bool mouseState ) {
	if(mouseState) {
		const Rect& rect = GetRect();
		Point pt;
		std::string piece = m_lines[m_cursor.y].text.substr(0, m_position.x);
		int sz = Fonts::getTextSize( m_font, piece );
		pt.y = (y-rect.y-10) / m_line_height + m_position.y;
		
		if(pt.y < 0) pt.y = 0;
		else if(pt.y > m_lines.size()-1) pt.y = m_lines.size()-1;
		
		pt.x = (x-rect.x-10) + sz;
		if(pt.x > 0) pt.x = Fonts::getMaxText( m_font, m_lines[pt.y].text, pt.x );
		
		if(pt.x < 0) pt.x = 0;
		else if(pt.x > m_lines[pt.y].text.size()) pt.x = m_lines[pt.y].text.size();
		
		m_cursor = pt;
		m_cursor_max_x = m_cursor.x;
		updatePosition();
	}
}

void TextBox::sortPoints(Point &p1, Point &p2) {
	if(m_anchor.y < m_cursor.y || (m_anchor.y == m_cursor.y && m_anchor.x < m_cursor.x)) {
		p1 = m_anchor;
		p2 = m_cursor;
	} else {
		p1 = m_cursor;
		p2 = m_anchor;
	}
}

void TextBox::updatePosition() {
	TextLine &line = m_lines[m_cursor.y];
	const Rect& rect = GetRect();
	if(m_position.x > line.text.size()) {
		m_text_max = Fonts::getMaxTextBw(m_font, line.text, rect.w-20);
	} else {
		m_text_max = Fonts::getMaxText(m_font, line.text.substr(m_position.x), rect.w-20);
	}
	
	if(m_cursor.x > m_position.x + m_text_max ) {
		m_position.x = m_cursor.x - m_text_max + 1;
	}
	else if(m_cursor.x < m_position.x) {
		m_position.x = m_cursor.x;
	}
	
	if(m_cursor.y < m_position.y) {
		m_position.y = m_cursor.y;
	} else if(m_cursor.y >= m_position.y + m_lines_max) {
		m_position.y = m_cursor.y - m_lines_max + 1;
	}
}

void TextBox::updateTexture(TextLine& line, bool new_tex) {
	if(line.tex == 0xffffffff)
		new_tex = true;
	SDL_Surface* surf = TTF_RenderText_Blended( m_font, line.text.size() > 0 ? line.text.c_str() : " ", {255,255,255} );
	// SDL_Surface* surf = TTF_RenderUTF8_Solid( m_font, line.text.size() > 0 ? line.text.c_str() : " ", {255,255,255} );
	// SDL_Surface* tempSurface = SDL_CreateRGBSurface(0, surf->w, surf->h, 32, 0x00ff0000, 0x0000ff00, 0x000000ff, 0xff000000);
    // SDL_BlitSurface(surf, 0, tempSurface, 0);
	line.tex = Drawing::GetTextureFromSurface(surf, new_tex ? 0 : line.tex);
	line.w = surf->w;
	line.h = surf->h;
	SDL_FreeSurface(surf);
	// SDL_FreeSurface(tempSurface);
}



void TextBox::OnKeyDown( SDL_Keycode &sym, SDL_Keymod mod ) {
	int val = sym;
	
	// if(val >= 256 && val <= 265) val -= 208;
	// cout << val << endl;
	m_cursor_blink_counter = m_cursor_blinking_rate;
	switch(val) {
		case SDLK_BACKSPACE: {
				if(m_anchor.x != -1) {
					deleteSelection();
					break;
				}
				TextLine &line = m_lines[m_cursor.y];
				if(m_cursor.x == 0) {
					// remove line
					if(m_cursor.y > 0) {
						TextLine &line = m_lines[m_cursor.y];
						int p = m_lines[m_cursor.y-1].text.size();
						if(m_lines[m_cursor.y].text.substr(m_cursor.x).size() > 0) {
							m_lines[m_cursor.y-1].text += m_lines[m_cursor.y].text.substr(m_cursor.x);
							updateTexture(m_lines[m_cursor.y-1]);
						}
						Drawing::DeleteTexture(m_lines[m_cursor.y].tex);
						m_lines.erase(m_lines.begin()+m_cursor.y);
						m_cursor.y--;
						m_cursor.x = p;
						m_cursor_max_x = m_cursor.x;
						updatePosition();
					}
				} else {
					line.text.erase(m_cursor.x-1, 1);
					updateTexture(line);
					m_cursor.x--;
					m_cursor_max_x = m_cursor.x;
					updatePosition();
				}
				
			}
			break;
		case SDLK_LEFT:
			if((mod & KMOD_SHIFT) == 0) {
				m_anchor.x = -1;
			} else if(m_anchor.x == -1) {
				m_anchor = m_cursor;
			}
			if(m_cursor.x > 0) {
				m_cursor.x --;
				m_cursor_max_x = m_cursor.x;
				updatePosition();
			}
			break;
		case SDLK_RIGHT:
			if((mod & KMOD_SHIFT) == 0) {
				m_anchor.x = -1;
			} else if(m_anchor.x == -1) {
				m_anchor = m_cursor;
			}
			
			if(m_cursor.x < m_lines[m_cursor.y].text.size()) {
				m_cursor.x ++;
				m_cursor_max_x = m_cursor.x;
				updatePosition();
			}
			break;
		case SDLK_UP:
			if((mod & KMOD_SHIFT) == 0) {
				m_anchor.x = -1;
			} else if(m_anchor.x == -1) {
				m_anchor = m_cursor;
			}
			if(m_cursor.y > 0) {
				m_cursor.y--;
				m_cursor.x = std::min<int>(m_lines[m_cursor.y].text.size(), m_cursor_max_x);
				updatePosition();
			}
			break;
		case SDLK_DOWN:
			if((mod & KMOD_SHIFT) == 0) {
				m_anchor.x = -1;
			} else if(m_anchor.x == -1) {
				m_anchor = m_cursor;
			}
			if(m_cursor.y < m_lines.size()-1) {
				m_cursor.y++;
				m_cursor.x = std::min<int>(m_lines[m_cursor.y].text.size(), m_cursor_max_x);
				updatePosition();
			}
			break;
		case SDLK_END:
			if((mod & KMOD_SHIFT) == 0) {
				m_anchor.x = -1;
			} else if(m_anchor.x == -1) {
				m_anchor = m_cursor;
			}
			
			if(mod & KMOD_CTRL) {
				m_cursor.y = m_lines.size()-1;
			}
			m_cursor.x = m_lines[m_cursor.y].text.size();
			m_cursor_max_x = m_cursor.x;
			updatePosition();
			break;
		case SDLK_HOME:
			if((mod & KMOD_LSHIFT) == 0) {
				m_anchor.x = -1;
			} else if(m_anchor.x == -1) {
				m_anchor = m_cursor;
			}
			
			if(mod & KMOD_CTRL) {
				m_cursor.y = 0;
			}
			m_cursor.x = 0;
			m_cursor_max_x = m_cursor.x;
			updatePosition();
			break;
		case SDLK_RETURN:
		case SDLK_KP_ENTER: {
				PutTextAtCursor("\n");
				emitEvent( event::enter );
			}
			break;
		
		// keys to ignore (won't be passed to default case)
		case SDLK_LCTRL:
		case SDLK_RCTRL:
		case SDLK_ESCAPE:
		case SDLK_LSHIFT:
		case SDLK_RSHIFT:
			break;
			
		default: {
			bool handled = false;
			if(mod & KMOD_CTRL) {
				switch(val) {
					case 'c': // copy
						handled = true;
						SDL_SetClipboardText( GetSelectedText().c_str() );
						break;
					case 'v': // paste
						handled = true;
						PutTextAtCursor( SDL_GetClipboardText() );
						break;
					case 'a': // select all
						m_anchor = Point(0,0);
						m_cursor = Point(m_lines.back().text.size(), m_lines.size()-1);
						updatePosition();
						break;
				}
				break;
			}
			
			if(handled) return;
			
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
			
			PutTextAtCursor(std::string(1,(char)val));
			
			emitEvent( event::change );
		}
			break;
	}
}

std::string TextBox::GetText( ) {
	std::string str;
	for(auto& l : m_lines) {
		str += l.text + "\n";
	}
	return str;
}




std::string TextBox::GetSelectedText() {
	Point p1,p2;
	sortPoints(p1,p2);
	
	std::string str = m_lines[p1.y].text.substr(p1.x, p2.y == p1.y ? (p2.x - p1.x) : std::string::npos);
	if(p1.y != p2.y) {
		for(auto it = m_lines.begin()+p1.y+1; it != m_lines.begin()+p2.y; it++) {
			str += "\n" + it->text;
		}
		str += "\n" + m_lines[p2.y].text.substr(0,p2.x);
	}
	return str;
}

void TextBox::deleteSelection() {
	if(m_anchor.x == -1 or m_anchor.y == -1) return;
	
	Point p1,p2;
	sortPoints(p1,p2);
	
	if(p1.y == p2.y) {
		m_lines[p1.y].text.erase(p1.x, p2.x-p1.x);
		updateTexture(m_lines[p1.y]);
	} else {
		m_lines[p1.y].text = m_lines[p1.y].text.substr(0,p1.x) + 
			m_lines[p2.y].text.substr(p2.x);
		updateTexture(m_lines[p1.y]);
		auto it1 = m_lines.begin()+p1.y+1;
		auto it2 = m_lines.begin()+p2.y+1;
		for(auto it = it1; it != it2; it++) {
			Drawing::DeleteTexture(it->tex);
		}
		m_lines.erase(it1,it2);
	}
	
	m_cursor = p1;
	m_anchor.x = -1;
}

static void find_and_replace(std::string& source, std::string const& find, std::string const& replace) {
    for(std::string::size_type i = 0; (i = source.find(find, i)) != std::string::npos; i += replace.length()) {
        source.replace(i, find.length(), replace);
    }
}

void TextBox::PutTextAtCursor(std::string text) {
	
	if(m_anchor.x != -1) {
		deleteSelection();
	}
	
	if(!m_multiline)
		find_and_replace(text, "\n", "");
	
	std::list<TextLine> lines;
	size_t pos = 0;
	size_t last = 0;
	for(int j = 0; pos != std::string::npos; j++) {
		pos = text.find_first_of("\n", last);
		TextLine line;
		if(pos == std::string::npos) {
			line.text = text.substr(last);
		} else {
			line.text = text.substr(last, pos-last);
		}
		last = pos+1;
		m_line_max = std::max<int>(m_line_max, line.text.size());
		lines.push_back(line);
	}
	
	
	
	Point next_cursor;
	next_cursor.y = m_cursor.y + lines.size() - 1;
	
	std::string left = "";
	std::string right = "";
	
	if(m_lines.size() > 0) {
		left = m_lines[m_cursor.y].text.substr(0,m_cursor.x);
		right = m_lines[m_cursor.y].text.substr(m_cursor.x);
		lines.back().text += right;
		m_lines[m_cursor.y].text = left + lines.front().text;
		updateTexture(m_lines[m_cursor.y]);
		
	} else {
		m_lines.push_back(lines.front());
		updateTexture(m_lines[0],true);
	}
	
	if(lines.size() == 1) {
		next_cursor.x = m_lines[m_cursor.y].text.size() - right.size();
	} else {
		next_cursor.x = lines.back().text.size() - right.size();
	}

	auto it2 = m_lines.begin()+m_cursor.y;
	for(auto it = ++lines.begin(); it != lines.end(); it++) {
		it2 = m_lines.insert(it2+1, *it);
		updateTexture(*it2, true);
	}
	
	m_cursor = next_cursor;
	m_ring_head = m_cursor.y;
	m_cursor_max_x = m_cursor.x;
	updatePosition();
	
}

void TextBox::SetTerminalMode( bool tf ) {
	if(!m_terminal_mode && tf) {
	}
	m_terminal_mode = tf;
}

void TextBox::SetMultilineMode( bool tf ) {
	m_multiline = tf;
	if(tf && m_lines.size() > 1) {
		m_lines.erase(m_lines.begin()+1, m_lines.end());
	}
}

void TextBox::SetCursorBlinkingRate( int rate ) {
	m_cursor_blinking_rate = rate;
}

void TextBox::SetTerminalHistoryBuffer(int n_messages) {
	m_terminal_max_messages = n_messages;
}

void TextBox::TerminalAddMessage( std::string msg ) {
}

}

