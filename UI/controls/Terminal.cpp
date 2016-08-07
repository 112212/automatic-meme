#include "Terminal.hpp"
namespace ng {

Terminal::Terminal() {
	m_log = new TextBox;
	m_terminal = new TextBox;
	
	Anchor a;
	m_log->SetAnchor(a);
	m_terminal->SetAnchor(a);
	
	AddControl(m_log);
	AddControl(m_terminal);
}

void Terminal::Render( Point position, bool isSelected ) {
	
}

Terminal* Terminal::Clone() {
	Terminal *t = new Terminal;
	*t = *this;
	return t;
}

void Terminal::OnMouseMove( int x, int y, bool mouseState ) {
	
}

void Terminal::OnMouseDown( int x, int y ) {
	
}


void Terminal::OnMouseUp( int x, int y ) {
	
}

void Terminal::OnLostFocus() {
	
}

void Terminal::OnMWheel( int updown ) {
	
}

void Terminal::SetValue( int value ) {
	
}

}
