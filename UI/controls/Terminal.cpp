#include "Terminal.hpp"
#include "../Gui.hpp"
#include "../managers/Effects.hpp"
namespace ng {

Terminal::Terminal() {
	setType( "terminal" );
	m_log = (TextBox*)CreateControl("terminal/textbox");
	m_terminal = (TextBox*)CreateControl("terminal/textbox");
	m_log->SetMultilineMode(true);
	m_log->SetReadOnly(true);
	m_log->SetStyle("colors", "false");
	m_history_counter = 0;
	AddControl(m_log);
	AddControl(m_terminal);
	m_log_fade_effect = new Effects::AutoFade();
	m_log->AddEffect( m_log_fade_effect );
	
	setInterceptMask(imask::mouse_up | imask::mouse_down | imask::key_down | imask::key_text);
	m_terminal->OnEvent( "enter", [this](Args& args) { return tbox_enter(args.control); });
	m_log_immediate = true;
	
	// m_log->SetAlpha(0.0f);
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
	m_log_fade_effect->Appear();
	
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
}


void Terminal::AppendLog(const std::string& s) {
	m_log->PutCursorAt(Point(9999,9999));
	m_log->PutTextAtCursor(s+"\n");
}

void Terminal::OnMouseDown( int x, int y, MouseButton button ) {
	m_log_fade_effect->Appear();
}

void Terminal::OnMouseUp( int x, int y, MouseButton button ) {
	if(m_log->GetSelectedText().size() == 0) {
		m_terminal->Activate();
	}
}

void Terminal::Focus() {
	m_terminal->Focus();
	m_terminal->Activate();
	m_log_fade_effect->Appear();
}

void Terminal::Render( Point position, bool isSelected ) {
	// cout << m_state << endl;
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
			break;
			default:
				m_terminal->SetStyle(style, value);
		}
	}
}

void Terminal::OnKeyDown( Keycode &sym, Keymod mod ) {
	m_log_fade_effect->Appear(); 
	
	if(sym == KEY_UP) {
		if(m_history_counter == 0) {
			return;
		}
		if(m_history_counter == m_history.size()) {
			m_command = m_terminal->GetText();
		}
		
		m_terminal->SetText(m_history[--m_history_counter]);
		m_terminal->PutCursorAt(Point(0,9999));
		
	} else if(sym == KEY_DOWN) {
		if(m_history_counter >= m_history.size()) {
			return;
		}
		
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

void Terminal::OnText( std::string s ) {
	m_log_fade_effect->Appear();
}

void Terminal::onFontChange() {
	onRectChange();
}

void Terminal::onRectChange() {
	int h = m_style.font->GetHeight() + 10;
	Layout a = m_log->GetLayout();

	a.SetPosition(0,0);
	a.SetSize(0,1,-h,1);
	m_log->SetLayout(a);
	
	a = Layout::parseRect("0,B");
	a.SetSize(0,1,h,0);
	
	m_terminal->SetLayout(a);
	ProcessLayout();
}

}
