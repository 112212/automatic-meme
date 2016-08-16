#include "TextBox.hpp"
#include <SDL2/SDL_opengl.h>
#include <cctype> // toupper
#include <list>
namespace ng {
TextBox::TextBox() : m_mousedown(false), m_position{0,0}, m_cursor{0,0}, m_anchor{-1,-1} /*textboxes anchor*/, 
		m_cursor_max_x(0) {
	setType( TYPE_TEXTBOX );
	initEventVector(event::max_events);
	m_multiline = false;
	m_terminal_mode = false;
	m_font = Fonts::GetFont( "default", 13 );
	m_cursor_blink_counter = 0;
	m_cursor_blinking_rate = 300;
	m_terminal_max_messages = 100;
	m_backcolor = 0;
	m_readonly = false;
	m_placeholder.text = "";
	SetText("");
}

TextBox::~TextBox() {
	
}

void TextBox::Render( Point pos, bool selected ) {
	
	const Rect& rect = this->GetRect();
	Point r = rect.Offset(pos);
	
	Drawing::FillRect(r.x, r.y, rect.w, rect.h, m_backcolor );
	
	int w,h;
	Drawing::GetResolution(w,h);
	// TODO: tweak scissor a little
	glScissor(r.x, h-(r.y+rect.h), rect.w, rect.h);
	glEnable(GL_SCISSOR_TEST);
	
	int j;
	
	if(m_lines.size() > 0) {
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
					Drawing::FillRect( r.x-sz+5+selw1, r.y+5+j*m_line_height, selw, i->h, m_selection_color);
				} else if(yy == p1.y) {
					std::string pd = m_lines[yy].text.substr(p1.x);
					int selw = Fonts::getTextSize( m_font, pd );
					Drawing::FillRect( r.x-sz+5+i->w-selw, r.y+5+j*m_line_height, selw, i->h, m_selection_color);
				} else if(yy == p2.y) {
					std::string pd = m_lines[yy].text.substr(0,p2.x);
					int selw = Fonts::getTextSize( m_font, pd );
					Drawing::FillRect( r.x-sz+5, r.y+5+j*m_line_height, selw, i->h, m_selection_color);
				} else {
					Drawing::FillRect( r.x-sz+5, r.y+5+j*m_line_height, i->w, i->h, m_selection_color);
				}
			}
		}
		
		if(!m_locked && m_lines.size() == 1 && m_lines[0].text.size() == 0) {
			Drawing::TexRect( r.x+5, r.y+5, m_placeholder.w, m_placeholder.h, m_placeholder.tex);
		} else {
			j=0;
			for( auto i = m_lines.begin()+m_position.y; i != m_lines.end(); i++,j++) {
				if(5+j*m_line_height+i->h > rect.h) break;
				Drawing::TexRect( r.x-sz+5, r.y+5+j*m_line_height, i->w, i->h, i->tex);
			}
		}
	}
	
	glDisable(GL_SCISSOR_TEST);
	
	if(m_locked && !m_readonly && ++m_cursor_blink_counter > m_cursor_blinking_rate && m_cursor.y-m_position.y < m_lines.size()) {
		if(m_cursor_blink_counter > 2*m_cursor_blinking_rate)
			m_cursor_blink_counter = 0;
			
		if(m_cursor.x >= m_position.x) {
			std::string piece = m_lines[m_cursor.y].text.substr(m_position.x, m_cursor.x-m_position.x);
			Drawing::Rect(Fonts::getTextSize( m_font, piece )+r.x+5, 
				(m_cursor.y-m_position.y)*m_line_height+r.y+5, 1, m_line_height, 0xffffffff);
		}
	}
	
	Control::Render(pos, selected);
}

int TextBox::m_cursor_color = 0xffffffff;
int TextBox::m_selection_color = 0xff808080;

void TextBox::SetText( std::string text ) {
	if(!m_font) return;
	if(m_lines.size() > 0)
		SetSelection( Point(m_lines.back().text.size(), m_lines.size()-1), {0,0} );
	m_cursor = Point(0,0);
	PutTextAtCursor(text);
	m_cursor = Point(0,0);
	updatePosition();
}

void TextBox::PutCursorAt( Point cursor ) {
	m_anchor = Point(-1,-1);
	m_cursor = cursor;
	if(m_cursor.y < 0 || m_cursor.x < 0)
		m_cursor = Point(0,0);
	if(m_cursor.y >= m_lines.size() || m_anchor.x >= m_lines[m_cursor.y].text.size()) {
		int last_y = m_lines.size()-1;
		int last_x = m_lines[last_y].text.size();
		m_cursor = Point(last_x, last_y);
	}
	updatePosition();
}

void TextBox::SetSelection( Point start, Point end ) {
	m_anchor = start;
	m_cursor = end;
	if(m_anchor.y >= m_lines.size() || m_anchor.x > m_lines[m_anchor.y].text.size())
		m_anchor = Point(-1,-1);
	if(m_cursor.y >= m_lines.size() || m_cursor.x > m_lines[m_cursor.y].text.size())
		m_cursor = Point(0,0);
	updatePosition();
}


void TextBox::OnMouseUp( int x, int y ) {
	m_mousedown = false;
}

void TextBox::OnLostFocus() {
	m_mousedown = false;
	// SDL_SetCursor( CCursors::defaultCursor );
}

void TextBox::OnLostControl() {
	m_locked = false;
}


void TextBox::STYLE_FUNC(value) {
	STYLE_SWITCH {
		_case("value"):
			SetText(value);
		_case("multiline"):
			SetMultilineMode(value == "true");
		_case("selection_color"):
			m_selection_color = Colors::ParseColor(value);
		_case("readonly"):
			SetReadOnly(value == "true");
		_case("placeholder"):
			m_placeholder.text = value;
			updateTexture(m_placeholder);
	}
		
}

void TextBox::onFontChange() {
	if(!m_font) return;
	m_text_max = Fonts::getMaxTextRep( m_font, 'A', GetRect().w );
	m_line_height = TTF_FontHeight(m_font);
	if(m_line_height != 0)
		m_lines_max = GetRect().h / m_line_height;
	else
		m_lines_max = 1;
}

void TextBox::onPositionChange() {
	onFontChange();
	for(TextLine& t : m_lines) {
		updateTexture(t);
	}
}

void TextBox::OnGetFocus() {
	// SDL_SetCursor( CCursors::textCursor );
}

void TextBox::OnMouseDown( int x, int y ) {
	sendGuiCommand( GUI_KEYBOARD_LOCK );
	const Rect& rect = GetRect();
	if(check_collision(x,y)) {
		m_locked = true;
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
	SDL_Surface* surf = TTF_RenderUTF8_Blended( m_font, line.text.size() > 0 ? line.text.c_str() : " ", {255,255,255} );
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
	if(mod & KMOD_CTRL) {
		bool handled = true;
		switch(val) {
			case 'c': // copy
					SDL_SetClipboardText( GetSelectedText().c_str() );
				break;
			case 'v': // paste
				PutTextAtCursor( SDL_GetClipboardText() );
				break;
			case 'a': // select all
				m_anchor = Point(0,0);
				m_cursor = Point(m_lines.back().text.size(), m_lines.size()-1);
				updatePosition();
				break;
			default:
				handled = false;
		}
		if(handled)return;
	}
	
	if(m_readonly) return;
			
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
				emitEvent( event::enter );
				PutTextAtCursor("\n");
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
	auto it_last = m_lines.end()-1;
	for(auto it = m_lines.begin(); it != m_lines.end(); it++) {
		if(it != it_last)
			str += it->text + "\n";
		else
			str += it->text;
	}
	return str;
}


std::string TextBox::GetSelectedText() {
	Point p1,p2;
	sortPoints(p1,p2);
	
	if(p1.x == -1 or p2.x == -1) return std::string("");
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
	updatePosition();
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

TextBox* TextBox::Clone() {
	TextBox* t = new TextBox;
	*t = *this;
	t->m_lines.clear();
	t->SetText("");
	return t;
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

void TextBox::SetReadOnly( bool readonly ) {
	m_readonly = readonly;
}


}

