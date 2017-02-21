#include "TextBox.hpp"
#include <SDL2/SDL_opengl.h>
#include <cctype> // toupper
#include <list>
#include "../../common/Cursor.hpp"

namespace ng {
#ifndef NO_TEXTURE
#define NO_TEXTURE 0xffffffff
#endif

TextBox::TextBox() : m_mousedown(false), m_position{0,0}, 
m_cursor{0,0}, m_anchor{-1,-1}, m_cursor_max_x(0)
 {
	setType( "textbox" );
	m_style.font = Fonts::GetFont( "default", 13 );
	m_cursor_blink_counter = 0;
	m_cursor_blinking_rate = 300;
	m_style.background_color = 0;
	m_max_length = 9999;
	m_multiline = false;
	m_readonly = false;
	m_locked = false;
	m_password = false;
	m_textwrap = false;
	m_wordwrap = false;
	m_color_input = false;
	m_placeholder.text = "";
	m_colors = true;
	
	m_scrollbar = new ScrollBar();
	m_scrollbar->SetVisible(false);
	
	m_scrollbar->SetVertical(true);
	SetText("");
}

TextBox::~TextBox() {
	
}

/*
// scrollbar to implement
void OnMouseMove( int mX, int mY, bool mouseState );
	void OnMouseDown( int mX, int mY );
	void OnMouseUp( int mX, int mY );
	void OnLostFocus();
	void OnMWheel( int updown );
*/

void TextBox::Render( Point pos, bool selected ) {
	
	const Rect& rect = this->GetRect();
	Point r = rect.Offset(pos);
	Control::Render(pos, selected);
	
	int w,h;
	Drawing::GetResolution(w,h);
	
	// TODO: tweak scissor a little
	glScissor(r.x, h-(r.y+rect.h), m_line_max_width, rect.h);
	glEnable(GL_SCISSOR_TEST);
	
	int j;
	
	if(m_lines.size() > 0) {
		// selection
		std::string piece = m_lines[m_cursor.y].text.substr(0, m_position.x, m_password);
		int sz = Fonts::getTextSize( m_style.font, piece );
		if(m_anchor.x != -1) {
			j=0;
			Point p1,p2;
			sortPoints(p1,p2);
			int yy=m_position.y;
			for( auto i = m_lines.begin()+yy; i != m_lines.end(); i++,j++,yy++) {
				if(5+j*m_line_height+i->h > rect.h) break;
				if(yy < p1.y || yy > p2.y) continue;
				
				if(yy == p1.y && p1.y == p2.y) {
					std::string pd = m_lines[yy].text.substr(p1.x, p2.x-p1.x, m_password);
					std::string pd1 = m_lines[yy].text.substr(0, p1.x, m_password);
					int selw = Fonts::getTextSize( m_style.font, pd );
					int selw1 = Fonts::getTextSize( m_style.font, pd1 );
					Drawing::FillRect( r.x-sz+5+selw1, r.y+5+j*m_line_height, selw, i->h, m_selection_color);
				} else if(yy == p1.y) {
					std::string pd = m_lines[yy].text.substr(p1.x, std::string::npos, m_password);
					int selw = Fonts::getTextSize( m_style.font, pd );
					Drawing::FillRect( r.x-sz+5+i->w-selw, r.y+5+j*m_line_height, selw, i->h, m_selection_color);
				} else if(yy == p2.y) {
					std::string pd = m_lines[yy].text.substr(0, p2.x, m_password);
					int selw = Fonts::getTextSize( m_style.font, pd );
					Drawing::FillRect( r.x-sz+5, r.y+5+j*m_line_height, selw, i->h, m_selection_color);
				} else {
					Drawing::FillRect( r.x-sz+5, r.y+5+j*m_line_height, i->w, i->h, m_selection_color);
				}
			}
		}
		
		// placeholder or text
		if(!isActive() && m_lines.size() == 1 && !m_placeholder.text.empty() && m_lines[0].text.size() == 0 ) {
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
	
	// cursor
	if(isActive() && !m_readonly && ++m_cursor_blink_counter > m_cursor_blinking_rate && 
		m_cursor.y >= m_position.y && m_cursor.y-m_position.y < rect.h/m_line_height) {
		
		if(m_cursor_blink_counter > 2*m_cursor_blinking_rate)
			m_cursor_blink_counter = 0;
			
		if(m_cursor.x >= m_position.x && m_cursor.x <= m_lines[m_cursor.y].text.size()) {
			std::string piece = m_lines[m_cursor.y].text.substr(m_position.x, m_cursor.x-m_position.x, m_password);
			Drawing::Rect(Fonts::getTextSize( m_style.font, piece )+r.x+5, 
				(m_cursor.y-m_position.y)*m_line_height+r.y+5, 1, m_line_height, 0xffffffff);
		}
	}
	
	if(m_scrollbar->IsVisible())
		m_scrollbar->Render(r, selected);
}

int TextBox::m_cursor_color = NO_TEXTURE;
int TextBox::m_selection_color = 0xff808080;

void TextBox::SetText( std::string text ) {
	if(!m_style.font) return;
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


void TextBox::OnMouseUp( int x, int y, MouseButton button ) {
	m_mousedown = false;
	m_scrollbar->OnMouseUp(x,y,button);
	m_scrollbar_selected = false;
	// SDL_SetCursor( Cursor::zoomCursor );
}

void TextBox::OnLostFocus() {
	m_mousedown = false;
	
	m_scrollbar->OnLostFocus();
}

void TextBox::OnLostControl() {
	m_locked = false;
}


void TextBox::OnSetStyle(std::string& style, std::string& value) {
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
		_case("password"):
			m_password = value == "true";
		_case("max_length"):
			m_max_length = std::stoi(value);
		_case("textwrap"):
			SetTextWrap(value=="true");
		_case("wordwrap"):
			SetWordWrap(value=="true");
		_case("colors"):
			m_colors = value == "true";
		_case("cursor_blinking_rate"):
			m_cursor_blinking_rate = std::stoi(value);
			
	}
		
}

void TextBox::onFontChange() {
	if(!m_style.font) return;
	m_text_max = Fonts::getMaxTextRep( m_style.font, 'A', m_line_max_width );
	m_line_height = TTF_FontHeight(m_style.font);
	if(m_line_height != 0)
		m_lines_max = GetRect().h / m_line_height;
	else
		m_lines_max = 1;
}

void TextBox::onRectChange() {
	onFontChange();
	const Rect& r = this->GetRect();
	int w = 20;
	
	m_scrollbar->SetRect(r.w - w, 0, w, r.h);
		
	Rect rekt = m_scrollbar->GetRect();
	m_line_max_width = r.w - w;
	for(TextLine& t : m_lines) {
		updateTexture(t);
	}
}

void TextBox::OnGetFocus() {
	m_cursor_blink_counter = m_cursor_blinking_rate;
}


void TextBox::OnMouseDown( int x, int y, MouseButton button ) {
	const Rect& rect = GetRect();
	
	if(m_scrollbar->check_collision(x-rect.x, y-rect.y)) {
		m_scrollbar->OnMouseDown(x - rect.x, y - rect.y, button);
		m_position.y = std::max(0, (int)(m_scrollbar->GetPercentageValue() * (m_lines.size() - m_lines_max) ));
		m_scrollbar_selected = true;
	} else {
		m_cursor_blink_counter = m_cursor_blinking_rate;
		m_locked = true;
		Point pt;
		std::string piece = m_lines[m_cursor.y].text.substr(0, m_position.x, m_password);
		int sz = Fonts::getTextSize( m_style.font, piece );
		pt.y = (y-rect.y-10) / m_line_height + m_position.y;
		
		if(pt.y < 0) pt.y = 0;
		else if(pt.y > m_lines.size()-1) pt.y = m_lines.size()-1;
		
		pt.x = (x-rect.x-10) + sz;
		if(pt.x > 0) pt.x = Fonts::getMaxText( m_style.font, m_lines[pt.y].text.substr(0,std::string::npos, m_password), pt.x );
		
		if(pt.x < 0) pt.x = 0;
		else if(pt.x > m_lines[pt.y].text.size()) pt.x = m_lines[pt.y].text.size();
		
		m_anchor = pt;
		m_cursor = pt;
		m_cursor_max_x = m_cursor.x;
		updatePosition();
	}
}

void TextBox::OnMouseMove( int x, int y, bool mouseState ) {
	const Rect& rect = GetRect();
	if(mouseState) {
		
		if(m_scrollbar_selected) {
			m_scrollbar->OnMouseMove(x - rect.x, y - rect.y, mouseState);
			m_position.y = std::max(0, (int)(m_scrollbar->GetPercentageValue() * (m_lines.size() - m_lines_max) ));
		} else {
			m_cursor_blink_counter = m_cursor_blinking_rate;
			
			Point pt;
			std::string piece = m_lines[m_cursor.y].text.substr(0, m_position.x, m_password);
			int sz = Fonts::getTextSize( m_style.font, piece );
			pt.y = (y-rect.y-10) / m_line_height + m_position.y;
			
			if(pt.y < 0) pt.y = 0;
			else if(pt.y > m_lines.size()-1) pt.y = m_lines.size()-1;
			
			pt.x = (x-rect.x-10) + sz;
			if(pt.x > 0) pt.x = Fonts::getMaxText( m_style.font, m_lines[pt.y].text.substr(0,std::string::npos, m_password), pt.x );
			
			if(pt.x < 0) pt.x = 0;
			else if(pt.x > m_lines[pt.y].text.size()) pt.x = m_lines[pt.y].text.size();
			
			m_cursor = pt;
			m_cursor_max_x = m_cursor.x;
			updatePosition();
			
			if(m_lines.size()-m_lines_max > 0)
				m_scrollbar->SetValue( m_position.y * 100 / (m_lines.size()-m_lines_max) );
		}
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
	
	if(m_textwrap && m_multiline) {
		m_position.x = 0;
	} else {
		
		if(m_position.x > line.text.size()) {
			m_text_max = Fonts::getMaxTextBw(m_style.font, line.text.substr(0,std::string::npos, m_password), m_line_max_width-20);
		} else {
			m_text_max = Fonts::getMaxText(m_style.font, line.text.substr(m_position.x, std::string::npos, m_password), m_line_max_width-20);
		}
		
		if(m_cursor.x > m_position.x + m_text_max ) {
			m_position.x = m_cursor.x - m_text_max + 1;
		}
		else if(m_cursor.x < m_position.x) {
			m_position.x = m_cursor.x;
		}
	}

	if(m_cursor.y < m_position.y) {
		m_position.y = m_cursor.y;
	} else if(m_cursor.y >= m_position.y + m_lines_max) {
		m_position.y = m_cursor.y - m_lines_max + 1;
	}
}

void TextBox::updateTexture(TextLine& line, bool new_tex) {
	if(line.tex == NO_TEXTURE)
		new_tex = true;
	
	SDL_Surface* surf;
	surf = line.text.get_surface(m_style.font, line.last_color, m_password);
	
	line.tex = Drawing::GetTextureFromSurface(surf, new_tex ? 0 : line.tex);
	line.w = surf->w;
	line.h = surf->h;
	SDL_FreeSurface(surf);
}

void TextBox::backspace() {
	if(m_anchor.x != -1 && (m_anchor.x != m_cursor.x || m_anchor.y != m_cursor.y)) {
		deleteSelection();
		return;
	}
	TextLine &line = m_lines[m_cursor.y];
	if(m_cursor.x == 0) {
		// remove line
		if(m_cursor.y > 0) {
			TextLine &line = m_lines[m_cursor.y];
			int p = m_lines[m_cursor.y-1].text.size();
			if(m_lines[m_cursor.y].text.substr(m_cursor.x).size() > 0) {
				m_lines[m_cursor.y-1].text += m_lines[m_cursor.y].text.csubstr(m_cursor.x);
				updateTexture(m_lines[m_cursor.y-1]);
			}
			Drawing::DeleteTexture(m_lines[m_cursor.y].tex);
			m_lines.erase(m_lines.begin()+m_cursor.y);
			m_cursor.y--;
			m_cursor.x = p;
			m_cursor_max_x = m_cursor.x;
			
			if(m_textwrap) {
				m_lines = wrap_lines(m_lines);
				spreadColor(m_lines.begin(),true);
				compact_lines(m_lines, m_lines.begin()+m_cursor.y);
				
				
				m_cursor.y = std::min<int>(std::max<int>(0, m_cursor.y), m_lines.size()-1);
				if(m_lines.size() > 0)
				m_cursor.x = std::min<int>(std::max<int>(0, m_cursor.x), m_lines[m_cursor.y].text.size());
			}
			
			updatePosition();
		}
	} else {
		line.text.erase(m_cursor.x-1, 1);
		updateTexture(line);
		m_anchor.x = -1;
		m_cursor.x--;
		m_cursor_max_x = m_cursor.x;
		
		if(m_textwrap) {
			compact_lines(m_lines, m_lines.begin()+m_cursor.y);
			m_cursor.y = std::min<int>(std::max<int>(0, m_cursor.y), m_lines.size()-1);
			if(m_lines.size() > 0)
			m_cursor.x = std::min<int>(std::max<int>(0, m_cursor.x), m_lines[m_cursor.y].text.size());
		}
		
		updatePosition();
		spreadColor(m_lines.begin() + m_cursor.y);
	}
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
	
	if(val == SDLK_TAB) {
		Control::tabToNextControl();
		return;
	}
	
	if(m_readonly) return;
	
	m_cursor_blink_counter = m_cursor_blinking_rate;
	switch(val) {
		case SDLK_BACKSPACE: {
				if(mod & KMOD_CTRL && m_anchor.x == -1) {
					// set selection to one word before cursor
					auto &line = m_lines[m_cursor.y].text;
					m_anchor = m_cursor;
					while(--m_anchor.x > 0 && !( !isspace(line[m_anchor.x]) && isspace(line[m_anchor.x-1])) );
				}
				backspace();
			}
			break;
		case SDLK_LEFT:
			if((mod & KMOD_SHIFT) == 0) {
				m_anchor.x = -1;
			} else if(m_anchor.x == -1) {
				m_anchor = m_cursor;
			}
			if(m_cursor.x > 0) {
				if(mod & KMOD_CTRL) {
					auto &line = m_lines[m_cursor.y].text;
					while(--m_cursor.x > 0 && !( !isspace(line[m_cursor.x]) && isspace(line[m_cursor.x-1])) );
				} else {
					m_cursor.x --;
				}
				m_cursor_max_x = m_cursor.x;
				updatePosition();
			}
			m_color_input = false;
			break;
		case SDLK_RIGHT:
			if((mod & KMOD_SHIFT) == 0) {
				m_anchor.x = -1;
			} else if(m_anchor.x == -1) {
				m_anchor = m_cursor;
			}
			
			if(m_cursor.x < m_lines[m_cursor.y].text.size()) {
				if(mod & KMOD_CTRL) {
					auto &line = m_lines[m_cursor.y].text;
					while(++m_cursor.x < line.size() && !(m_cursor.x-1 >= 0 && 
						!isspace(line[m_cursor.x-1]) && isspace(line[m_cursor.x])));
				} else {
					m_cursor.x ++;
				}
				
				m_cursor_max_x = m_cursor.x;
				updatePosition();
			}
			m_color_input = false;
			break;
		case SDLK_UP:
			if((mod & KMOD_SHIFT) == 0) {
				m_anchor.x = -1;
			} else if(m_anchor.x == -1) {
				m_anchor = m_cursor;
			}
			if(m_cursor.y > 0) {
				m_cursor.y --;
				m_cursor.x = std::min<int>(m_lines[m_cursor.y].text.size(), m_cursor_max_x);
				updatePosition();
			}
			m_color_input = false;
			break;
		case SDLK_DOWN:
			if((mod & KMOD_SHIFT) == 0) {
				m_anchor.x = -1;
			} else if(m_anchor.x == -1) {
				m_anchor = m_cursor;
			}
			if(m_cursor.y < m_lines.size()-1) {
				m_cursor.y ++;
				m_cursor.x = std::min<int>(m_lines[m_cursor.y].text.size(), m_cursor_max_x);
				updatePosition();
			}
			m_color_input = false;
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
			m_color_input = false;
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
			m_color_input = false;
			break;
		case SDLK_RETURN:
		case SDLK_KP_ENTER: {
				emitEvent( "enter" );
				PutTextAtCursor("\n");
			}
			break;
		
		// keys to ignore (won't be passed to default case)
		case SDLK_LCTRL:
		case SDLK_RCTRL:
		case SDLK_LSHIFT:
		case SDLK_RSHIFT:
			break;
			
		case SDLK_ESCAPE:
			Unselect();
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
			
			val = SDLCodeToChar(val, mod & KMOD_SHIFT);
			
			if(m_color_input) {
				if(val != '^') {
					backspace();
					PutTextAtCursor(std::string("^")+((char)val));
				}
				m_color_input = false;
			} else {
				PutTextAtCursor(std::string(1,(char)val));
				
				if(m_colors && val == '^') {
					m_color_input = true;
				}
			}
			
			emitEvent( "change" );
			break;
		}
	}
	
	if(m_lines.size()-m_lines_max > 0)
		m_scrollbar->SetValue( m_position.y * 100 / (m_lines.size()-m_lines_max) );
}

std::string TextBox::GetText( ) {
	std::string str;
	auto it_last = m_lines.end()-1;
	for(auto it = m_lines.begin(); it != m_lines.end(); it++) {
		if(it != it_last)
			str += it->text.str() + "\n";
		else
			str += it->text.str();
	}
	return str;
}

std::string TextBox::GetRawText( ) {
	std::string str;
	auto it_last = m_lines.end()-1;
	for(auto it = m_lines.begin(); it != m_lines.end(); it++) {
		if(it != it_last)
			str += it->text.GetRawString() + "\n";
		else
			str += it->text.GetRawString();
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
			str += "\n" + it->text.str();
		}
		str += "\n" + m_lines[p2.y].text.substr(0,p2.x);
	}
	return str;
}

std::string TextBox::GetRawSelectedText() {
	Point p1,p2;
	sortPoints(p1,p2);
	
	if(p1.x == -1 or p2.x == -1) return std::string("");
	std::string str = m_lines[p1.y].text.substr(p1.x, p2.y == p1.y ? (p2.x - p1.x) : std::string::npos);
	if(p1.y != p2.y) {
		for(auto it = m_lines.begin()+p1.y+1; it != m_lines.begin()+p2.y; it++) {
			str += "\n" + it->text.GetRawString();
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
		m_lines[p1.y].text = (m_lines[p1.y].text.substr(0,p1.x) + m_lines[p2.y].text.substr(p2.x));
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
	if(!m_colors)
		find_and_replace(text, "^", "^^");
		
	if(m_lines.size() > 0 && m_lines[m_cursor.y].text.size() + text.size() > m_max_length) {
		text = text.substr(0, m_max_length-m_lines[m_cursor.y].text.size());
	}
	
	// form lines to add
	std::vector<TextLine> lines;
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
	
	Colorstring left = "";
	Colorstring right = "";
	
	if(m_lines.size() > 0) {
		
		left = m_lines[m_cursor.y].text.csubstr(0,m_cursor.x);
		right = m_lines[m_cursor.y].text.csubstr(m_cursor.x);
		
		lines.front().text = left + lines.front().text;
		lines.front().wrap = m_lines[m_cursor.y].wrap;
		lines.back().text += right;

		if(m_textwrap && m_multiline) {
			lines = wrap_lines(lines);
			int lc = m_lines[m_cursor.y].last_color;
			m_lines[m_cursor.y].text = lines.front().text;
			m_lines[m_cursor.y].last_color = lc;
		} else {
			m_lines[m_cursor.y].text = lines.front().text;
			
		}
	} else {
		if(m_textwrap && m_multiline) lines = wrap_lines(lines);
		m_lines.push_back(lines.back());
		m_cursor.y = 0;
	}
	
	int last_color = m_lines[m_cursor.y].text.GetLastColor();
	if(last_color == -1)
		last_color = m_lines[m_cursor.y].last_color;
	updateTexture(m_lines[m_cursor.y]);
	Point next_cursor;
	next_cursor.y = m_cursor.y + lines.size() - 1;
	
	// update textures for new lines
	auto it2 = m_lines.begin()+m_cursor.y;
	for(auto it = lines.begin()+1; it != lines.end(); it++) {
		it2 = m_lines.insert(it2+1, *it);
		it2->last_color = last_color;
		int nc = it2->text.GetLastColor();
		if(nc != -1)
			last_color = nc;
		updateTexture(*it2);
	}
	
	if(lines.size() == 1) {
		next_cursor.x = m_lines[m_cursor.y].text.size() - right.size();
	} else {
		next_cursor.x = lines.back().text.size();
		for(int num = 0, l=lines.size()-1; ; l--) {
			int n = std::min<int>(right.size()-num, lines[l].text.size());
			next_cursor.x = lines[l].text.size() - n;
			num += n;
			if(num < right.size())
				next_cursor.y--;
			else
				break;
		}
	}
	
	if(m_textwrap && m_multiline) {
		compact_lines(m_lines, m_lines.begin()+m_cursor.y+1);
	}
	
	spreadColor(m_lines.begin()+m_cursor.y);
	
	m_cursor = next_cursor;
	m_cursor_max_x = m_cursor.x;
	
	updatePosition();
	
	if(m_lines_max+1 < m_lines.size()) {
		m_scrollbar->SetVisible(true);
	} else {
		m_scrollbar->SetVisible(false);
	}
}

void TextBox::OnMWheel( int updown ) {
	m_position.y = std::min<int>(std::max<int>(0, m_position.y-updown), m_lines.size()-GetRect().h/m_line_height);
	m_scrollbar->SetValue( m_position.y * 100 / (m_lines.size()-m_lines_max) );
}

// ----------- Wrapping ------------------------------------------------
std::vector<TextBox::TextLine> TextBox::wrap_lines(const std::vector<TextLine>& lines) {
	std::vector<TextLine> new_lines;
	new_lines.reserve(m_lines.size()*3/2);
	/*
	while have next real line
		bring next real line as next line (save previous line)
		while can break
			break and go to next line
		repeat until not wrap
			try join as much as possible and go to next line
	*/
	int tex = 0;
	int l = 0;
	while(l < lines.size()) {
		TextLine nl = lines[l++];
		Colorstring line = nl.text;
		
		int max_text;
		int last_pos = 0;
		std::string sub;
		do {
			sub = line.substr(last_pos);
			max_text = Fonts::getMaxText(m_style.font, sub, m_line_max_width-15);
			
			if(m_wordwrap && max_text != sub.size()) {
				int s = max_text;
				for(; s >= 0; s--) {
					if(sub[s] == ' ')
						break;
				}
				if(s > 0)
					max_text = s;
			}
			
			nl.text = line.csubstr(last_pos, max_text);
			
			if(last_pos != 0) {
				nl.wrap = true;
				nl.tex = NO_TEXTURE;
			}
			
			new_lines.push_back(nl);
			
			last_pos += max_text;
		} while(max_text < sub.size());
		
	}
	return new_lines;
}

void TextBox::compact_lines(std::vector<TextLine>& v, std::vector<TextLine>::iterator start) {
	if(!m_multiline) return;
	while(start->wrap && v.begin() != start) start--;
	auto it=start;
	auto n = it+1;
	auto last_it = it;
	for(; n != v.end() && n->wrap; it++,n++) {
		int can_offer = Fonts::getMaxText(m_style.font, n->text.str(), m_line_max_width - it->w - 16);
		if(m_wordwrap && can_offer < n->text.size()) {
			while(can_offer > 0 && n->text[can_offer] != ' ')
				can_offer--;
		}
		if(can_offer != 0) {
			it->text += n->text.csubstr(0,can_offer);
			n->text = n->text.csubstr(can_offer);
			n->w = Fonts::getTextSize(m_style.font, n->text.str());
			updateTexture(*it);
			last_it = n;
		}
	}
	updateTexture(*last_it);
	auto end = it+1;
	while(it != v.begin() && it->wrap && it->text.size() == 0) {
		it--;
	}
	if(it != end) {
		v.erase(it+1,end);
	}
}
// ---------------------------------------------------------------------

void TextBox::SetWordWrap(bool word) {
	if(m_wordwrap == word) return;
	
	m_wordwrap = word;
	if(m_textwrap && m_lines.size() > 0) {
		SetTextWrap(false);
		SetTextWrap(true);
	}
}

void TextBox::spreadColor(std::vector<TextLine>::iterator it, bool fully) {
	int last_color = it->text.GetLastColor();
	if(last_color == -1)
		last_color = it->last_color;
	it++;
	for(; it != m_lines.end(); it++) {
		if(!fully && it->last_color == last_color) break;
		it->last_color = last_color;
		int nc = it->text.GetLastColor();
		if(nc != -1)
			last_color = nc;
		updateTexture(*it);
	}
}

void TextBox::SetTextWrap(bool wrap) {
	if(wrap == m_textwrap) return;
	int tex = 0;
	std::vector<TextLine> new_lines;
	
	if(wrap) {
		new_lines.reserve(m_lines.size()*3/2);
		for(auto &line : m_lines) {
			int last_pos = 0;
			int max_text = 0;
			
			do {
				std::string sub = line.text.substr(last_pos);
				max_text = Fonts::getMaxText(m_style.font, sub, m_line_max_width-15);
				if(m_wordwrap && max_text != sub.size()) {
					int s = max_text;
					for(; s >= 0; s--) {
						if(sub[s] == ' ')
							break;
					}
					max_text = s;
				}
				TextLine nl;
				nl.tex = tex < m_lines.size() ? m_lines[tex++].tex : NO_TEXTURE;
				nl.text = line.text.csubstr(last_pos, max_text);
				nl.wrap = last_pos != 0;
				new_lines.push_back(nl);
				last_pos += max_text;
			} while(last_pos < line.text.size());
		}
	} else {
		new_lines.reserve(m_lines.size()*2/3);
		TextLine nl;
		nl.wrap = true; // just mark as not initialized (result lines must have this to be false (real lines))
		for(auto &line : m_lines) {
			if(!line.wrap) {
				if(!nl.wrap) {
					new_lines.push_back(nl);
				}
				nl.tex = tex < m_lines.size() ? m_lines[tex++].tex : NO_TEXTURE;
				nl.text = line.text;
				nl.wrap = false;
			} else {
				nl.text += line.text;
			}
		}
		if(!nl.wrap) {
			new_lines.push_back(nl);
		}
		
		// delete unused texture
		for(;tex < m_lines.size(); tex++) {
			Drawing::DeleteTexture(m_lines[tex].tex);
		}
	}
	
	m_lines = new_lines;
	m_cursor.y = std::min<int>( m_cursor.y, m_lines.size() );
	m_cursor.x = std::min<int>( m_cursor.x, m_lines[m_cursor.y].text.size() );
	m_textwrap = wrap;
	updatePosition();
	spreadColor(m_lines.begin(),true);
}

TextBox* TextBox::Clone() {
	TextBox* t = new TextBox();
	copyStyle(t);
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

