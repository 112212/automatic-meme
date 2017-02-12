#include "Terminal.hpp"
#include "../../Gui.hpp"
namespace ng {

Terminal::Terminal() {
	setType( "terminal" );
	m_style.font = Fonts::GetFont( "default", 13 );
	m_log = (TextBox*)CreateControl("terminal/textbox");
	m_terminal = (TextBox*)CreateControl("terminal/textbox");
	m_log->SetMultilineMode(true);
	m_log->SetReadOnly(true);
	m_log->SetStyle("colors", "false");
	m_history_counter = 0;
	AddControl(m_log);
	AddControl(m_terminal);
	setInterceptMask(imask::mouse_up | imask::key_down);
	m_terminal->OnEvent( "enter", [this](Control *c, const Argv& a) { return tbox_enter(c); });
	m_log_immediate = true;
	m_state = invisible;
	m_tick = 0;
	m_log->SetAlpha(0.0f);
}

Terminal::~Terminal() {}

void Terminal::tbox_enter(Control* c) {
	TextBox* t = static_cast<TextBox*>(c);
	// if(t->GetText().size() <= 0) return;
	m_command = t->GetRawText();
	m_history.push_back(m_command);
	m_history_counter = m_history.size();
	// m_log_msg = "> " + m_command;
	t->SetText("");
	// m_log_immediate = false;
	emitEvent("enter", {m_command});
	// m_log_immediate = true;
	// WriteLog(m_log_msg);
}

const std::string& Terminal::GetLastCommand() {
	return m_command;
}

const std::string Terminal::GetText() {
	return m_terminal->GetText();
}

void Terminal::SetText(const std::string& str) {
	m_terminal->SetText(str);
	m_terminal->PutCursorAt(Point(9999,0));
}

void Terminal::ClearLog() {
	m_log->SetText("");
}

void Terminal::WriteLog(const std::string& s) {
	if(m_log_immediate) {
		m_log->PutCursorAt(Point(9999,9999));
		m_log->PutTextAtCursor(s+"\n");
	} else {
		m_log_msg = s;
	}
	appear();
}

void Terminal::appear() {
	if(m_state == invisible) {
		m_state = appearing;
		m_tick = 0;
	} else if(m_state == fading) {
		m_state = appearing;
	} else if(m_state == visible) {
		m_tick = 0;
	}
}

void Terminal::AppendLog(const std::string& s) {
	m_log->PutCursorAt(Point(9999,9999));
	m_log->PutTextAtCursor(s+"\n");
	appear();
}

void Terminal::OnMouseDown( int x, int y, MouseButton button ) {

}

void Terminal::OnMouseUp( int x, int y, MouseButton button ) {
	if(m_log->GetSelectedText().size() == 0)
		m_terminal->Activate();
	appear();
}

void Terminal::Focus() {
	m_terminal->Focus();
	m_terminal->Activate();
	appear();
}

void Terminal::Render( Point position, bool isSelected ) {
	// cout << m_state << endl;
	if(m_state == visible) {
		if(IsSelected()) {
			m_tick = 0;
		} else if(m_fadeout_tick > 0) {
			if(++m_tick > m_fadeout_tick) {
				m_tick = 0;
				m_state = fading;
			}
		}
	} else if(m_state == fading) {
		m_log->SetAlpha(1.0f - (float)m_tick / m_fadeout_speed);
		if(++m_tick > m_fadeout_speed) {
			m_state = invisible;
			m_tick = 0;
		}
	} else if(m_state == appearing) {
		m_log->SetAlpha((float)m_tick / m_fadeout_speed);
		if(++m_tick > m_fadeout_speed) {
			m_state = visible;
			m_tick = 0;
		}
	} else if(m_state == invisible && IsSelected()) {
		appear();
	}
	
	
	Control::Render(position, isSelected);
	RenderWidget(position,isSelected);
}

Terminal* Terminal::Clone() {
	Terminal *t = new Terminal;
	copyStyle(t);
	return t;
}

void Terminal::OnSetStyle(std::string& style, std::string& value) {
	if(style.substr(0,3) == "log") {
		// log_* applies styles to log window
		m_log->SetStyle(style.substr(4), value);
	} else {
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
			_case("fadeout_speed"):
				m_fadeout_speed = std::stoi(value);
			_case("fadeout_delay"):
				m_fadeout_tick = std::stoi(value);
			break;
			default:
				m_terminal->SetStyle(style, value);
		}
	}
}

void Terminal::OnKeyDown( SDL_Keycode &sym, SDL_Keymod mod ) {
	appear();
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

void Terminal::onFontChange() {
	onPositionChange();
	if(getEngine())
		getEngine()->ApplyAnchoring();
}

void Terminal::onPositionChange() {
	int h = TTF_FontHeight(m_style.font) + 7;
	m_log->SetRect(0,0,GetRect().w, GetRect().h-h);
	m_terminal->SetRect(0,GetRect().h-h,GetRect().w,h);
	
	Anchor a = m_log->GetAnchor();
	a.w_min[0] = 0;
	a.h_min[0] = GetRect().h - h;
	a.y = 0;
	a.x = 0;
	a.w_min[1] = 1;
	a.h_min[1] = 0;
	m_log->SetAnchor(a);
	a.y = a.h_min[0];
	a.h_min[0] = h;
	m_terminal->SetAnchor(a);
}

}
