#include "Terminal.hpp"
namespace ng {

Terminal::Terminal() {
	m_style.font = Fonts::GetFont( "default", 13 );
	m_log = (TextBox*)CreateControl("terminal/textbox");
	m_terminal = (TextBox*)CreateControl("terminal/textbox");
	m_log->SetMultilineMode(true);
	m_log->SetReadOnly(true);
	initEventVector(event::max_events);
	m_history_counter = 0;
	AddControl(m_log);
	AddControl(m_terminal);
	setInterceptMask(imask::mouse_up | imask::key_down);
	m_terminal->SubscribeEvent( TextBox::event::enter, [this](Control *c) { return tbox_enter(c); });
	m_log_immediate = true;
}

Terminal::~Terminal() {}

void Terminal::tbox_enter(Control* c) {
	TextBox* t = static_cast<TextBox*>(c);
	if(t->GetText().size() <= 0) return;
	m_command = t->GetText();
	m_history.push_back(m_command);
	m_history_counter = m_history.size();
	t->SetText("");
	m_log_msg = "> " + m_command;
	m_log_immediate = false;
	emitEvent(event::command);
	m_log_immediate = true;
	WriteLog(m_log_msg);
}

const std::string& Terminal::GetText() {
	return m_command;
}

void Terminal::WriteLog(const std::string& s) {
	if(m_log_immediate) {
		m_log->PutCursorAt(Point(9999,9999));
		m_log->PutTextAtCursor(s+"\n");
	} else {
		m_log_msg = s;
	}
}

void Terminal::AppendLog(const std::string& s) {
	m_log_msg += s;
}

void Terminal::OnMouseDown( int x, int y ) {

}

void Terminal::OnMouseUp( int x, int y ) {
	if(m_log->GetSelectedText().size() == 0)
		m_terminal->Activate();
}

void Terminal::Render( Point position, bool isSelected ) {
	Control::Render(position, isSelected);
	RenderWidget(position,isSelected);
}

Terminal* Terminal::Clone() {
	Terminal *t = new Terminal;
	copyStyle(t);
	return t;
}

void Terminal::STYLE_FUNC(value) {
	STYLE_SWITCH {
		_case("wordwrap"): {
			bool val = value == "true";
			m_log->SetTextWrap(val);
			m_log->SetWordWrap(val);
		}
		_case("textwrap"): {
			bool val = value == "true";
			m_log->SetTextWrap(val);
		}
		_case("max_length"): {
			m_terminal->SetStyle(style, value);
		}
	}
}

void Terminal::OnKeyDown( SDL_Keycode &sym, SDL_Keymod mod ) {
	if(sym == SDLK_UP) {
		if(m_history_counter == 0) 
			return;
		if(m_history_counter == m_history.size()) {
			m_command = m_terminal->GetText();
		}
		
		m_terminal->SetText(m_history[--m_history_counter]);
		m_terminal->PutCursorAt(Point(0,9999));
		
	} else if(sym == SDLK_DOWN) {
		if(m_history_counter >= m_history.size())
			return;
		
		if(m_history_counter == m_history.size()-1) {
			m_terminal->SetText(m_command);
			m_history_counter = m_history.size();
			m_terminal->PutCursorAt(Point(0,9999));
		} else {
			m_terminal->SetText(m_history[++m_history_counter]);
			m_terminal->PutCursorAt(Point(0,9999));
		}
	}
}

void Terminal::onFontChange() {}

void Terminal::onPositionChange() {
	int h = TTF_FontHeight(m_style.font) + 5;
	m_log->SetRect(0,0,GetRect().w, GetRect().h-h);
	m_terminal->SetRect(0,GetRect().h-h,GetRect().w,h);
	
	Anchor a = m_log->GetAnchor();
	a.sx = 0;
	a.sy = GetRect().h - h;
	a.y = 0;
	a.x = 0;
	a.sW = 1;
	a.sH = 0;
	m_log->SetAnchor(a);
	a.y = a.sy;
	a.sy = h;
	m_terminal->SetAnchor(a);
}

}
