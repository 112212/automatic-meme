#include <cctype> // toupper
#include <list>

#include "../Gui.hpp"
#include "TextBox.hpp"
#include "../Cursor.hpp"

namespace ng {

#ifndef NO_TEXTURE
	#define NO_TEXTURE 0xffffffff
#endif

TextBox::TextBox(std::string id) 
	: m_mousedown(false)
	, m_anchor{-1,-1}
	, m_cursor_max_x(0) 
	, m_cursor_blink_counter(0) 
	, m_cursor_blinking_rate(0.3)
	, m_max_length(9999)
	, m_multiline(false)
	, m_readonly (false)
	, m_locked  (false)
	, m_password (false)
	, m_textwrap (false)
	, m_wordwrap (false)
	, m_color_input(false)
	, m_colors(true)
	, m_update_viewport(false)
	, m_should_refresh_line_max(false)
	, m_hscroll(false)
	, m_text_max(0)
	, m_selection_color(0xff808080)
	, m_line_max(1)
	, m_cursor(0,0)
	, m_position(0,0)
	, m_scrollbar_thickness(10)
	, m_tabnextcontrol(1)
	{
	SetId(id);
	setType( "textbox" );
	m_style.background_color = 0;
	m_placeholder.text = "";
	m_text_area_chars.h = 1;
	m_text_area.w = 10;
	setInteractible(true);
	
	m_scrollbar_v = createControl<ScrollBar>("scrollbar", "vscrollbar");
	m_scrollbar_v->SetVisible(false);
	m_scrollbar_v->SetVertical(true);
	m_scrollbar_v->SetLayout(getString("R,T,%d,H", m_scrollbar_thickness));
	m_scrollbar_v->OnEvent("change", [&](Args &args) {
		m_position.y = std::max(0, (int)(m_scrollbar_v->GetPercentageValue() * (m_lines.size() - m_text_area_chars.h) ));
		// updatePosition();
	});
	
	AddControl(m_scrollbar_v);
	
	m_scrollbar_h = createControl<ScrollBar>("scrollbar", "hscrollbar");
	m_scrollbar_h->SetVisible(false);
	m_scrollbar_h->SetLayout(getString("L,B,W,%d", m_scrollbar_thickness));
	m_scrollbar_h->OnEvent("change", [&](Args &args) {
		int cw = m_style.font->GetMaxTextRep( 'a', m_text_area.w );
		// std::cout << "mw: " << m_text_area.w << " : " << cw << " : " << m_line_max << " : " << m_text_area.w<< "\n";
		m_position.x = std::max(0, (int)(m_scrollbar_h->GetPercentageValue() * (m_line_max - cw)) );
		// m_update_viewport = true;
		auto s = std::string(m_position.x, 'a');
		m_viewport_offset.x = m_style.font->GetTextSize(s);
	});
	AddControl(m_scrollbar_h);
	
	// setInterceptMask(0xffffffffu);
	SetText("");
}

TextBox::~TextBox() {
	
}

void TextBox::Render( Point pos, bool selected ) {
	
	const Rect& rect = this->GetRect();
	const Point r = rect.Offset(pos);
	Control::RenderBase(pos, selected);
	
	int w,h;
	Drawing().GetResolution(w,h);
	
	Drawing().PushClipRegion( r.x+2, r.y, m_text_area.w-5, m_text_area.h );
	
	int j;
	
	int sz=0;
	
	if(!m_lines.empty()) {
		// selection
		std::string piece = m_lines[m_cursor.y].text.utf8_substr(0, m_position.x, m_password);
		if(m_update_viewport) {
			sz = m_style.font->GetTextSize(piece);
			m_viewport_offset.x = sz;
			m_update_viewport = false;
		} else {
			sz = m_viewport_offset.x;
		}
		
		// draw selection if there is one
		if(m_anchor.x != -1) {
			j=0;
			Point p1,p2;
			sortPoints(p1,p2);
			int yy=m_position.y;
			for( auto i = m_lines.begin()+yy; i != m_lines.end(); i++,j++,yy++) {
				if(5+j*m_line_height+i->h > rect.h) break;
				if(yy < p1.y || yy > p2.y) continue;
				
				if(yy == p1.y && p1.y == p2.y) {
					std::string pd = m_lines[yy].text.utf8_substr(p1.x, p2.x-p1.x, m_password);
					std::string pd1 = m_lines[yy].text.utf8_substr(0, p1.x, m_password);
					
					int selw = m_style.font->GetTextSize( pd );
					int selw1 = m_style.font->GetTextSize( pd1 );
					
					Drawing().FillRect( r.x-sz+5+selw1, r.y+5+j*m_line_height, selw, i->h, m_selection_color);
				} else if(yy == p1.y) {
					std::string pd = m_lines[yy].text.utf8_substr(p1.x, std::string::npos, m_password);
					int selw = m_style.font->GetTextSize( pd );
					Drawing().FillRect( r.x-sz+5+i->w-selw, r.y+5+j*m_line_height, selw, i->h, m_selection_color);
				} else if(yy == p2.y) {
					std::string pd = m_lines[yy].text.utf8_substr(0, p2.x, m_password);
					int selw = m_style.font->GetTextSize( pd );
					Drawing().FillRect( r.x-sz+5, r.y+5+j*m_line_height, selw, i->h, m_selection_color);
				} else {
					Drawing().FillRect( r.x-sz+5, r.y+5+j*m_line_height, i->w, i->h, m_selection_color);
				}
			}
		}
		
		// placeholder or text
		if(!isActive() && m_lines.size() == 1 && !m_placeholder.text.empty() && m_lines[0].text.empty() ) {
			Drawing().TexRect( r.x+5, r.y+5, m_placeholder.w, m_placeholder.h, m_placeholder.tex);
		} else {
			j=0;
			if(m_position.y < m_lines.size()) {
				for( auto i = m_lines.begin()+m_position.y; i != m_lines.end(); i++,j++) {
					// std::cout << "rend line: " << i->text << " , " << i->w << " " << i->h << "\n";
					if(5+j*m_line_height+i->h > rect.h) break;
					if(i->h <= 1 || !i->tex) continue;
					// std::cout << GetId() << " did rend line: " << i->text << " : " << i->w << " " << i->h << "\n";
					Drawing().TexRect(r.x-sz+5, r.y+5+j*m_line_height, i->w, i->h, i->tex);
				}
			}
		}
	}

	
	// draw cursor
	if(isActive() && !m_readonly && (m_cursor_blink_counter += getDeltaTime()) > m_cursor_blinking_rate && 
		m_cursor.y >= m_position.y && m_cursor.y-m_position.y < rect.h/m_line_height) {
		
		if(m_cursor_blink_counter > 2*m_cursor_blinking_rate) {
			m_cursor_blink_counter = 0;
		}
			
		if(m_cursor.x >= m_position.x && m_cursor.x <= m_lines[m_cursor.y].text.utf8_size()) {
			// std::string piece = m_lines[m_cursor.y].text.utf8_substr(m_position.x, m_cursor.x-m_position.x, m_password);
			std::string piece = m_lines[m_cursor.y].text.utf8_substr(0, m_cursor.x, m_password);
			Drawing().Rect(m_style.font->GetTextSize( piece )+r.x-sz+5, 
				(m_cursor.y-m_position.y)*m_line_height+r.y+5, 1, m_line_height, 0xffffffff);
		}
	}
	
	Drawing().PopClipRegion();
	
	RenderWidget(pos,selected);
	
}


void TextBox::SetText( std::string text ) {
	if(!m_style.font) return;
	
	if(m_lines.size() > 0) {
		SetSelection( Point(m_lines.back().text.utf8_size(), m_lines.size()-1), {0,0} );
	}
	m_line_max = 1;
	m_cursor = Point(0,0);
	PutTextAtCursor(text);
	m_cursor = Point(0,0);
	updatePosition();
}

void TextBox::PutCursorAt( Point cursor ) {
	m_anchor = Point(-1,-1);
	m_cursor = cursor;
	if(m_cursor.y < 0 || m_cursor.x < 0) {
		m_cursor = Point(0,0);
	}
	if(m_cursor.y >= m_lines.size() || m_anchor.x >= m_lines[m_cursor.y].text.utf8_size()) {
		int last_y = m_lines.size()-1;
		int last_x = m_lines[last_y].text.size();
		m_cursor = Point(last_x, last_y);
	}
	updatePosition();
}

void TextBox::SetSelection( Point start, Point end ) {
	m_anchor = start;
	m_cursor = end;
	if(m_anchor.y >= m_lines.size() || m_anchor.x > m_lines[m_anchor.y].text.utf8_size()) {
		m_anchor = Point(-1,-1);
	}
	if(m_cursor.y >= m_lines.size() || m_cursor.x > m_lines[m_cursor.y].text.utf8_size()) {
		m_cursor = Point(0,0);
	}
	updatePosition();
}


void TextBox::OnMouseUp( int x, int y, MouseButton button ) {
	m_mousedown = false;
}

void TextBox::OnLostFocus() {
	m_mousedown = false;
	getEngine()->GetCursor().SetCursorType(CursorType::pointer);
}

void TextBox::OnLostControl() {
	// m_locked = false;
	// std::cout << "on lost control: " << GetText() << "\n";
}

void TextBox::OnSetStyle(std::string& style, std::string& value) {
	STYLE_SWITCH {
		_case("value"):
			SetText(value);
		_case("multiline"):
			SetMultilineMode(toBool(value));
		_case("selection_color"):
			m_selection_color = Color(value).GetUint32();
		_case("readonly"):
			SetReadOnly(toBool(value));
		_case("placeholder"):
			m_placeholder.text = value;
			updateTexture(m_placeholder);
		_case("password"):
			m_password = toBool(value);
		_case("max_length"):
			m_max_length = std::stoi(value);
		_case("textwrap"):
			SetTextWrap(toBool(value));
		_case("wordwrap"):
			SetWordWrap(toBool(value));
		_case("colors"):
			m_colors = toBool(value);
		_case("cursor_blinking_rate"):
			m_cursor_blinking_rate = std::stoi(value);
		_case("hscroll"):
			m_hscroll = toBool(value);
		_case("cursor"): {
			std::vector<std::string> s;
			split_string(value, s, ',');
			if(s.size() == 2) {
				PutCursorAt(Point(std::stoi(s[0]), std::stoi(s[1])));
			}
		}
		_case("tabnext"): {
			m_tabnextcontrol = toBool(value);
		}
	}
}

void TextBox::onFontChange() {
	if(!m_style.font) return;
	m_text_area_chars.w = m_style.font->GetMaxTextRep( 'a', m_text_area.w );
	GlyphMetrics g = m_style.font->GetGlyphMetrics('A');
	m_line_height = g.height;
	if(m_line_height != 0) {
		m_text_area_chars.h = GetRect().h / m_line_height;
	} else {
		m_text_area_chars.h = 1;
	}
	updatePosition();
}

void TextBox::onRectChange() {
	onFontChange();
	const Rect& r = GetRect();
	
	m_text_area = Size( m_scrollbar_v->IsVisible() ? r.w-m_scrollbar_thickness : r.w,
						m_scrollbar_h->IsVisible() ? r.h-m_scrollbar_thickness : r.h );
	
	if(getEngine()) {
		for(TextLine& t : m_lines) {
			updateTexture(t);
		}
	}
}

void TextBox::OnGetFocus() {
	m_cursor_blink_counter = m_cursor_blinking_rate;
}

void TextBox::OnActivate() {
	PutCursorAt(Point(999,0));
	sendGuiCommand(GUI_KEYBOARD_LOCK);
}


void TextBox::OnMouseDown( int x, int y, MouseButton button ) {
	const Rect& rect = GetRect();
	if(m_readonly) return;
	
	m_cursor_blink_counter = m_cursor_blinking_rate;
	m_locked = true;
	Point pt;
	pt.y = (y-10) / m_line_height + m_position.y;
	
	if(pt.y < 0) {
		pt.y = 0;
	} else if(pt.y > m_lines.size()-1) {
		pt.y = m_lines.size()-1;
	}
	
	pt.x = (x-10) + m_viewport_offset.x;
	if(pt.x > 0) {
		pt.x = m_style.font->GetMaxText( m_lines[pt.y].text.utf8_substr(0, ColorString::npos, m_password), pt.x );
	}
	
	
	if(pt.x < 0) {
		pt.x = 0;
	} else if(pt.x > m_lines[pt.y].text.utf8_size()) {
		pt.x = m_lines[pt.y].text.utf8_size();
	}
	
	m_anchor = pt;
	m_cursor = pt;
	m_cursor_max_x = m_cursor.x;
	updatePosition();
	Activate();
}

void TextBox::OnMouseMove( int x, int y, bool mouseState ) {
	const Rect& rect = GetRect();
	setCursor(CursorType::textinput);
	if(mouseState) {
		
		if(m_readonly) return;
		m_cursor_blink_counter = m_cursor_blinking_rate;
		
		Point pt;
		int sz = m_viewport_offset.x;
		pt.y = (y-10) / m_line_height + m_position.y;
		
		if(pt.y < 0) {
			pt.y = 0;
		} else if(pt.y > m_lines.size()-1) {
			pt.y = m_lines.size()-1;
		}
		
		pt.x = (x-10) + sz;
		if(pt.x > 0) {
			pt.x = m_style.font->GetMaxText( m_lines[pt.y].text.utf8_substr(0,std::string::npos, m_password), pt.x );
		}
		
		if(pt.x < 0) {
			pt.x = 0;
		} else if(pt.x > m_lines[pt.y].text.utf8_size()) {
			pt.x = m_lines[pt.y].text.utf8_size();
		}
		
		m_cursor = pt;
		m_cursor_max_x = m_cursor.x;
		updatePosition();
		
		if(m_lines.size()-m_text_area_chars.h > 0) {
			m_scrollbar_v->SetValue( m_position.y * 100 / (m_lines.size()-m_text_area_chars.h) );
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
		
		if(m_position.x > line.text.utf8_size()) {
			m_text_area_chars.w = m_style.font->GetMaxTextBw( line.text.utf8_substr(0,std::string::npos, m_password), std::max(m_text_area.w-10, 1));
		} else {
			m_text_area_chars.w = m_style.font->GetMaxText( line.text.utf8_substr(m_position.x, std::string::npos, m_password), std::max(m_text_area.w-10, 1));
		}
		
		if(m_cursor.x > m_position.x + m_text_area_chars.w ) {
			m_position.x = m_cursor.x - m_text_area_chars.w + 2;
			m_update_viewport = true;
		} else if(m_cursor.x < m_position.x) {
			m_position.x = m_cursor.x;
			m_update_viewport = true;
		}
	}

	if(m_cursor.y < m_position.y) {
		m_position.y = m_cursor.y;
		m_update_viewport = true;
	} else if(m_cursor.y >= m_position.y + m_text_area_chars.h) {
		m_position.y = m_cursor.y - m_text_area_chars.h + 1;
		m_update_viewport = true;
	}
	
	if(m_should_refresh_line_max) {
		m_line_max = 0;
		for(auto &l : m_lines) {
			m_line_max = std::max(m_line_max, l.char_count);
		}
		m_should_refresh_line_max = false;
	}
	
	// vertical scroll
	if(m_lines.size() > m_text_area_chars.h+1) {
		
		if(!m_lines.empty()) {
			m_scrollbar_v->SetSliderSize(m_text_area_chars.h * rect.h / m_lines.size());
		}
		if(!m_scrollbar_v->IsVisible()) {
			m_text_area.w = rect.w - m_scrollbar_thickness;
			m_scrollbar_v->SetVisible(true);
			m_scrollbar_h->GetLayout().SetSize(-m_scrollbar_thickness, 1, m_scrollbar_thickness, 0);
			ProcessLayout();
		}
	} else {
		if(m_scrollbar_v->IsVisible()) {
			m_text_area.w = rect.w;
			m_scrollbar_h->GetLayout().SetSize(0, 1, m_scrollbar_thickness, 0);
			m_scrollbar_v->SetVisible(false);
			ProcessLayout();
		}
	}
	
	if(!m_textwrap && m_hscroll) {
		int w = GetRect().w;
		int cw = m_style.font->GetMaxTextRep( 'a', GetRect().w );
		// std::cout << "["<<GetId() << "] m_text_max: " << m_text_max << ", m_line_max: " << m_line_max << " , " << w << "\n";
		// if(m_line_max > cw) {
		if(m_text_max >= w) {
			if(!m_scrollbar_h->IsVisible()) {
				m_scrollbar_h->SetVisible(true);
				m_text_area.h = rect.h - m_scrollbar_thickness;
				m_scrollbar_v->GetLayout().SetSize(m_scrollbar_thickness, 0, -m_scrollbar_thickness, 1);
				ProcessLayout();
			}
			// std::cout << "updating slider\n";
			m_scrollbar_h->SetSliderSize(rect.w * cw / m_line_max );
			m_scrollbar_h->SetValue( 100 * m_position.x / std::max(1,m_line_max-cw));
			if(m_line_height != 0) {
				m_text_area_chars.h = (GetRect().h-10) / m_line_height;
			}
		} else {
			if(m_scrollbar_h->IsVisible()) {
				m_scrollbar_h->SetVisible(false);
				m_scrollbar_v->GetLayout().SetSize(m_scrollbar_thickness, 0, -m_scrollbar_thickness, 1);
				m_text_area.h = rect.h;
				ProcessLayout();
			}
		}
	} else {
		if(m_scrollbar_h->IsVisible()) {
			m_scrollbar_h->SetVisible(false);
			m_scrollbar_v->GetLayout().SetSize(m_scrollbar_thickness, 0, -m_scrollbar_thickness, 1);
			m_text_area.h = rect.h;
			ProcessLayout();
		}
	}
}

void TextBox::updateTexture(TextLine& line, bool new_tex) {
	if(line.text.empty()) {
		if(line.tex) {
			delete line.tex;
			line.tex=0;
		}
		return;
	}
	if(line.tex == 0) {
		new_tex = true;
	}
	
	if(line.tex) {
		delete line.tex;
		line.tex=0;
	}
	
	line.tex = line.text.get_image(m_style.font, line.last_color, m_password);
	if(!line.tex) return;
	Size s = line.tex->GetImageSize();
	line.w = s.w;
	line.h = s.h;
	m_text_max = std::max(s.w, m_text_max);
	// std::cout << "line: " << line.text << " w: " << s.w << "\n";
	line.char_count = line.text.utf8_size();
	m_line_max = std::max<int>(m_line_max, line.char_count);
}

void TextBox::backspace() {
	if(m_anchor.x != -1 && (m_anchor.x != m_cursor.x || m_anchor.y != m_cursor.y)) {
		deleteSelection();
		return;
	}
	
	TextLine &line = m_lines[m_cursor.y];
	m_should_refresh_line_max = m_should_refresh_line_max || line.char_count == m_line_max;
	
	if(m_cursor.x == 0) {
		// remove line
		if(m_cursor.y > 0) {
			TextLine &line = m_lines[m_cursor.y];
			int p = m_lines[m_cursor.y-1].text.utf8_size();
			if(!m_lines[m_cursor.y].text.utf8_substr(m_cursor.x).empty()) {
				m_lines[m_cursor.y-1].text += m_lines[m_cursor.y].text.utf8_csubstr(m_cursor.x);
				updateTexture(m_lines[m_cursor.y-1]);
			}
			delete m_lines[m_cursor.y].tex;
			m_lines[m_cursor.y].tex = 0;
			m_lines.erase(m_lines.begin()+m_cursor.y);
			m_cursor.y--;
			m_cursor.x = p;
			m_cursor_max_x = m_cursor.x;
			
			if(m_textwrap) {
				m_lines = wrap_lines(m_lines);
				spreadColor(m_lines.begin(),true);
				compact_lines(m_lines, m_lines.begin()+m_cursor.y);
				
				m_cursor.y = clip(m_cursor.y, 0, (int)(m_lines.size()-1));
				if(m_lines.size() > 0) {
					m_cursor.x = clip(m_cursor.x, 0, (int)m_lines[m_cursor.y].text.utf8_size());
				}
			}
			updatePosition();
		}
	} else {
		m_color_input = false;
		line.text.utf8_erase(m_cursor.x-1, 1);
		updateTexture(line);
		m_anchor.x = -1;
		if(m_position.x > 0) {
			m_position.x--;
		}
		m_cursor.x--;
		m_cursor_max_x = m_cursor.x;
		
		if(m_textwrap) {
			compact_lines(m_lines, m_lines.begin()+m_cursor.y);
			m_cursor.y = std::min<int>(std::max<int>(0, m_cursor.y), m_lines.size()-1);
			if(m_lines.size() > 0) {
				m_cursor.x = std::min<int>(std::max<int>(0, m_cursor.x), m_lines[m_cursor.y].text.utf8_size());
			}
		}
		
		updatePosition();
		spreadColor(m_lines.begin() + m_cursor.y);
	}
	
	emitEvent("change", {GetText()});
}


void TextBox::OnText( std::string s ) {
		if(m_readonly) return;
		if(m_color_input) {
			if(s != "^") {
				backspace();
				PutTextAtCursor("^"+s);
			}
			m_color_input = false;
		} else {
			PutTextAtCursor(s);
			if(m_colors && s == "^") {
				m_color_input = true;
			}
		}
		
		// std::cout << "OnText: " << s << "\n";
		emitEvent("change", {GetText()});
}

void TextBox::OnKeyDown( Keycode sym, Keymod mod ) {
	int val = sym;
	// std::cout << "tb on kdown: " << (char)sym << "\n";
	if(mod & KEYMOD_CTRL) {
		bool handled = true;
		switch(val) {
			
			case 'c': // copy
				GetSystem().SetClipboard( GetSelectedText() );
				break;
				
			case 'v': // paste
				PutTextAtCursor( GetSystem().GetClipboard() );
				break;
				
			case 'a': // select all
				m_anchor = Point(0,0);
				m_cursor = Point(m_lines.back().text.utf8_size(), m_lines.size()-1);
				updatePosition();
				break;
				
			default:
				handled = false;
		}
		
		if(handled) {
			return;
		}
	}
	
	if(val == KEY_TAB && m_tabnextcontrol) {
		Control::tabToNextControl();
		return;
	}
	
	if(m_readonly) return;
	
	m_cursor_blink_counter = m_cursor_blinking_rate;
	switch(val) {
		case KEY_BACKSPACE: {
				if(mod & KEYMOD_CTRL && m_anchor.x == -1) {
					// set selection to one word before cursor
					auto &line = m_lines[m_cursor.y].text;
					m_anchor = m_cursor;
					while(--m_anchor.x > 0 && !( !isspace(line[m_anchor.x]) && isspace(line[m_anchor.x-1])) );
				}
				backspace();
			}
			break;
		case KEY_LEFT:
			if((mod & KEYMOD_SHIFT) == 0) {
				m_anchor.x = -1;
			} else if(m_anchor.x == -1) {
				m_anchor = m_cursor;
			}
			if(m_cursor.x > 0) {
				if(mod & KEYMOD_CTRL) {
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
		case KEY_RIGHT:
			if((mod & KEYMOD_SHIFT) == 0) {
				m_anchor.x = -1;
			} else if(m_anchor.x == -1) {
				m_anchor = m_cursor;
			}
			
			if(m_cursor.x < m_lines[m_cursor.y].text.utf8_size()) {
				if(mod & KEYMOD_CTRL) {
					auto &line = m_lines[m_cursor.y].text;
					while(++m_cursor.x < line.utf8_size() && !(m_cursor.x-1 >= 0 && 
						!isspace(line[m_cursor.x-1]) && isspace(line[m_cursor.x])));
				} else {
					m_cursor.x ++;
				}
				
				m_cursor_max_x = m_cursor.x;
				updatePosition();
			}
			m_color_input = false;
			break;
		case KEY_UP:
			if((mod & KEYMOD_SHIFT) == 0) {
				m_anchor.x = -1;
			} else if(m_anchor.x == -1) {
				m_anchor = m_cursor;
			}
			if(m_cursor.y > 0) {
				m_cursor.y --;
				m_cursor.x = std::min<int>(m_lines[m_cursor.y].text.utf8_size(), m_cursor_max_x);
				updatePosition();
			}
			m_color_input = false;
			break;
		case KEY_DOWN:
			if((mod & KEYMOD_SHIFT) == 0) {
				m_anchor.x = -1;
			} else if(m_anchor.x == -1) {
				m_anchor = m_cursor;
			}
			if(m_cursor.y < m_lines.size()-1) {
				m_cursor.y ++;
				m_cursor.x = std::min<int>(m_lines[m_cursor.y].text.utf8_size(), m_cursor_max_x);
				updatePosition();
			}
			m_color_input = false;
			break;
		case KEY_END:
			if((mod & KEYMOD_SHIFT) == 0) {
				m_anchor.x = -1;
			} else if(m_anchor.x == -1) {
				m_anchor = m_cursor;
			}
			
			if(mod & KEYMOD_CTRL) {
				m_cursor.y = m_lines.size()-1;
			}
			m_cursor.x = m_lines[m_cursor.y].text.utf8_size();
			m_cursor_max_x = m_cursor.x;
			updatePosition();
			m_color_input = false;
			break;
		case KEY_HOME:
			if((mod & KEYMOD_LSHIFT) == 0) {
				m_anchor.x = -1;
			} else if(m_anchor.x == -1) {
				m_anchor = m_cursor;
			}
			
			if(mod & KEYMOD_CTRL) {
				m_cursor.y = 0;
			}
			m_cursor.x = 0;
			m_cursor_max_x = m_cursor.x;
			updatePosition();
			m_color_input = false;
			break;
		case KEY_RETURN:
		case KEY_KP_ENTER: {
				emitEvent( "enter" );
				PutTextAtCursor("\n");
			}
			break;
		
		// keys to ignore (won't be passed to default case)
		case KEY_LCTRL:
		case KEY_RCTRL:
		case KEY_LSHIFT:
		case KEY_RSHIFT:
			break;
			
		case KEY_ESCAPE:
			Unselect();
			break;
			
		default: {
			break;
		}
	}
	
	if(m_lines.size()-m_text_area_chars.h > 0) {
		m_scrollbar_v->SetValue( m_position.y * 100 / (m_lines.size()-m_text_area_chars.h) );
	}
}

std::string TextBox::GetText( ) {
	std::string str;
	auto it_last = m_lines.end()-1;
	for(auto it = m_lines.begin(); it != m_lines.end(); it++) {
		if(it != it_last) {
			str += it->text.str() + "\n";
		} else {
			str += it->text.str();
		}
	}
	return str;
}

std::string TextBox::GetRawText( ) {
	std::string str;
	auto it_last = m_lines.end()-1;
	for(auto it = m_lines.begin(); it != m_lines.end(); it++) {
		if(it != it_last) {
			str += it->text.GetRawString() + "\n";
		} else {
			str += it->text.GetRawString();
		}
	}
	return str;
}


std::string TextBox::GetSelectedText() {
	Point p1,p2;
	sortPoints(p1,p2);
	
	if(p1.x == -1 or p2.x == -1) return std::string("");
	std::string str = m_lines[p1.y].text.utf8_substr(p1.x, p2.y == p1.y ? (p2.x - p1.x) : std::string::npos);
	if(p1.y != p2.y) {
		for(auto it = m_lines.begin()+p1.y+1; it != m_lines.begin()+p2.y; it++) {
			str += "\n" + it->text.str();
		}
		str += "\n" + m_lines[p2.y].text.utf8_substr(0,p2.x);
	}
	return str;
}

std::string TextBox::GetRawSelectedText() {
	Point p1,p2;
	sortPoints(p1,p2);
	
	if(p1.x == -1 or p2.x == -1) return std::string("");
	std::string str = m_lines[p1.y].text.utf8_substr(p1.x, p2.y == p1.y ? (p2.x - p1.x) : std::string::npos);
	if(p1.y != p2.y) {
		for(auto it = m_lines.begin()+p1.y+1; it != m_lines.begin()+p2.y; it++) {
			str += "\n" + it->text.GetRawString();
		}
		str += "\n" + m_lines[p2.y].text.utf8_substr(0,p2.x);
	}
	return str;
}

void TextBox::deleteSelection() {
	if(m_anchor.x == -1 or m_anchor.y == -1) {
		return;
	}
	
	Point p1,p2;
	sortPoints(p1,p2);
	
	if(p1.y == p2.y) {
		m_lines[p1.y].text.utf8_erase(p1.x, p2.x-p1.x);
		updateTexture(m_lines[p1.y]);
	} else {
		m_lines[p1.y].text = (m_lines[p1.y].text.utf8_substr(0,p1.x) + m_lines[p2.y].text.utf8_substr(p2.x));
		updateTexture(m_lines[p1.y]);
		auto it1 = m_lines.begin()+p1.y+1;
		auto it2 = m_lines.begin()+p2.y+1;
		for(auto it = it1; it != it2; it++) {
			if(it->tex) {
				it->tex->FreeCache();
				delete it->tex;
			}
		}
		m_lines.erase(it1,it2);
	}
	
	m_cursor = p1;
	m_anchor.x = -1;
	updatePosition();
}

void TextBox::PutTextAtCursor(std::string text) {
	if(m_anchor.x != -1) {
		deleteSelection();
	}
	
	if(!m_multiline) {
		find_and_replace(text, "\n", "");
	}
	
	if(!m_colors) {
		find_and_replace(text, "^", "^^");
	}
	
	ColorString cs(text);
	
	if(m_lines.size() > 0 && m_lines[m_cursor.y].text.utf8_size() + cs.utf8_size() > m_max_length) {
		// TODO: utf8 fix
		cs = cs.utf8_substr(0, m_max_length-m_lines[m_cursor.y].text.utf8_size());
	}
	
	// form lines to add
	std::vector<TextLine> lines;
	std::pair<ColorString::size_type,ColorString::size_type> pos;
	std::pair<ColorString::size_type,ColorString::size_type> last_pos{0,0};
	ColorString::size_type len = cs.size();
	do {
		pos = cs.utf8_find('\n', last_pos.first);
		TextLine line;
		
		if(pos.first == ColorString::npos) {
			line.text = cs.utf8_csubstr(last_pos.second);
		} else {
			line.text = cs.utf8_csubstr(last_pos.second, pos.second-last_pos.second);
			last_pos = pos;
			last_pos.first++;
			last_pos.second++;
		}
		
		lines.push_back(line);
	} while( pos.first < len );
	
	if(pos.first != ColorString::npos) {
		TextLine line;
		line.text = ColorString("");
		lines.push_back(line);
	}
	
	ColorString left = "";
	ColorString right = "";
	
	if(m_lines.size() > 0) {
		
		left = m_lines[m_cursor.y].text.utf8_csubstr(0,m_cursor.x);
		right = m_lines[m_cursor.y].text.utf8_csubstr(m_cursor.x);
		
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
		if(m_textwrap && m_multiline) {
			lines = wrap_lines(lines);
		}
		m_lines.push_back(lines.back());
		m_cursor.y = 0;
	}
	
	int last_color = m_lines[m_cursor.y].text.GetLastColor();
	if(last_color == -1) {
		last_color = m_lines[m_cursor.y].last_color;
	}
	updateTexture(m_lines[m_cursor.y]);
	Point next_cursor;
	next_cursor.y = m_cursor.y + lines.size() - 1;
	
	// update textures for new lines
	auto it2 = m_lines.begin()+m_cursor.y;
	for(auto it = lines.begin()+1; it != lines.end(); it++) {
		it2 = m_lines.insert(it2+1, *it);
		it2->last_color = last_color;
		int nc = it2->text.GetLastColor();
		if(nc != -1) {
			last_color = nc;
		}
		updateTexture(*it2);
	}
	
	if(lines.size() == 1) {
		next_cursor.x = m_lines[m_cursor.y].text.utf8_size() - right.utf8_size();
	} else {
		next_cursor.x = lines.back().text.utf8_size();
		for(int num = 0, l=lines.size()-1; ; l--) {
			int n = std::min<int>(right.utf8_size()-num, lines[l].text.utf8_size());
			next_cursor.x = lines[l].text.utf8_size() - n;
			num += n;
			if(num < right.utf8_size()) {
				next_cursor.y--;
			} else {
				break;
			}
		}
	}
	
	if(m_textwrap && m_multiline) {
		compact_lines(m_lines, m_lines.begin()+m_cursor.y+1);
	}
	
	spreadColor(m_lines.begin()+m_cursor.y);
	
	m_cursor = next_cursor;
	m_cursor_max_x = m_cursor.x;
	
	updatePosition();
}

void TextBox::OnMWheel( int updown ) {
	m_position.y = clip<int>(m_position.y-updown, 0, (m_lines.size()-GetRect().h/m_line_height));
	if(m_lines.size() != m_text_area_chars.h) {
		int val = m_position.y * 100 / (m_lines.size()-m_text_area_chars.h);
		m_scrollbar_v->SetValue( m_position.y * 100 / (m_lines.size()-m_text_area_chars.h) );
	} else {
		m_scrollbar_v->SetValue(100);
	}
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
		ColorString line = nl.text;
		
		int max_text;
		int last_pos = 0;
		ColorString sub;
		do {
			sub = line.utf8_csubstr(last_pos);
			max_text = m_style.font->GetMaxText( sub.str(), std::max(m_text_area.w-15, 1));
			
			if(m_wordwrap && max_text != sub.utf8_size()) {
				auto s2 = sub.utf8_rfind(' ', sub.utf8_byte_pos(max_text) );
				if(s2.second > 0) {
					max_text = s2.second;
				}
			}
			
			nl.text = line.utf8_csubstr(last_pos, max_text);
			
			if(last_pos != 0) {
				nl.wrap = true;
				// TODO: delete nl.tex?
				nl.tex = 0;
			}
			
			new_lines.push_back(nl);
			
			last_pos += max_text;
		} while(max_text < sub.utf8_size());
	}
	return new_lines;
}

void TextBox::compact_lines(std::vector<TextLine>& v, std::vector<TextLine>::iterator start) {
	if(!m_multiline || start >= m_lines.end()) return;
	while(start->wrap && v.begin() != start) {
		start--;
	}
	auto it=start;
	auto n = it+1;
	auto last_it = it;
	
	for(; n != v.end() && n->wrap; it++,n++) {
	
		int can_offer = m_style.font->GetMaxText( n->text.str(), m_text_area.w - it->w - 16);
		
		if(m_wordwrap && can_offer < n->text.utf8_size()) {
			can_offer = n->text.utf8_rfind( ' ', n->text.utf8_byte_pos(can_offer) ).second;
		}
		if(can_offer != 0) {
			it->text += n->text.utf8_csubstr(0,can_offer);
			n->text = n->text.utf8_csubstr(can_offer);
			n->w = m_style.font->GetTextSize( n->text.str() );
			updateTexture(*it);
			last_it = n;
		}
	}
	updateTexture(*last_it);
	auto end = it+1;
	while(it != v.begin() && it->wrap && it->text.utf8_size() == 0) {
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
		if(nc != -1) {
			last_color = nc;
		}
		updateTexture(*it);
	}
}

void TextBox::SetTextWrap(bool wrap) {
	if(!m_multiline || wrap == m_textwrap) return;
	int tex = 0;
	std::vector<TextLine> new_lines;
	
	if(wrap) {
		new_lines.reserve(m_lines.size()*3/2);
		for(auto &line : m_lines) {
			int last_pos = 0;
			int max_text = 0;
			
			do {
				std::string sub = line.text.utf8_substr(last_pos);
				max_text = m_style.font->GetMaxText( sub, std::max(m_text_area.w-15, 1));
				if(m_wordwrap && max_text != sub.size()) {
					int s = max_text;
					for(; s >= 0; s--) {
						if(sub[s] == ' ')
							break;
					}
					max_text = s;
				}
				TextLine nl;
				nl.tex = tex < m_lines.size() ? m_lines[tex++].tex : 0;
				nl.text = line.text.utf8_csubstr(last_pos, max_text);
				nl.wrap = last_pos != 0;
				new_lines.push_back(nl);
				last_pos += max_text;
			} while(last_pos < line.text.utf8_size());
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
				nl.tex = tex < m_lines.size() ? m_lines[tex++].tex : 0;
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
		for(; tex < m_lines.size(); tex++) {
			m_lines[tex].tex->FreeCache();
		}
	}
	
	m_lines = new_lines;
	m_cursor.y = std::min<int>( m_cursor.y, m_lines.size() );
	m_cursor.x = std::min<int>( m_cursor.x, m_lines[m_cursor.y].text.utf8_size() );
	m_textwrap = wrap;
	updatePosition();
	spreadColor(m_lines.begin(),true);
	
	if(m_textwrap) {
		m_scrollbar_h->SetVisible(false);
	} else {
		m_line_max = 0;
		for(auto &line : m_lines) {
			m_line_max = std::max<int>(m_line_max, line.text.utf8_size());
		}
	}
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

