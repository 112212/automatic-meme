#include "Terminal.hpp"
namespace ng {

Terminal::Terminal() {
	m_font = Fonts::GetFont( "default", 13 );
	m_log = (TextBox*)CreateControl("textbox");
	m_terminal = (TextBox*)CreateControl("textbox");
	m_log->SetMultilineMode(true);
	m_log->SetReadOnly(true);
	initEventVector(event::max_events);
	AddControl(m_log);
	AddControl(m_terminal);
	setInterceptMask(imask::mouse_up | imask::mouse_down);
	m_terminal->SubscribeEvent( TextBox::event::enter, [this](Control *c) { return tbox_enter(c); });
}

Terminal::~Terminal() {}

void Terminal::tbox_enter(Control* c) {
	TextBox* t = static_cast<TextBox*>(c);
	if(t->GetText().size() <= 1) return;
	m_command = t->GetText();
	t->SetText("");
	emitEvent(event::command);
	WriteLog("> "+m_command);
}

const std::string& Terminal::GetText() {
	return m_command;
}

void Terminal::WriteLog(const std::string& s) {
	m_log->PutCursorAt(Point(9999,9999));
	m_log->PutTextAtCursor(s+"\n");
}

void Terminal::OnMouseDown( int x, int y ) {
	m_last_pos = {x,y};
}
void Terminal::OnMouseUp( int x, int y ) {
	if(x == m_last_pos.x && y == m_last_pos.y) {
		m_terminal->Focus();
	}
}

void Terminal::Render( Point position, bool isSelected ) {
	Control::Render(position, isSelected);
	RenderWidget(position,isSelected);
}

Terminal* Terminal::Clone() {
	Terminal *t = new Terminal;
	*t = *this;
	return t;
}


void Terminal::onPositionChange() {
	int h = TTF_FontHeight(m_font) + 5;
	m_log->SetRect(0,0,GetRect().w, GetRect().h-h);
	m_terminal->SetRect(0,GetRect().h-h,GetRect().w,h);
}

}
