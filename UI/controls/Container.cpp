#include "Container.hpp"
#include "../Gui.hpp"
#include <iostream>


namespace ng {
Container::Container() {
	setType( "dialog" );
	m_is_mouseDown = false;
	m_num_controls = 0;
	overflow_h = overflow_v = false;
	m_scroll_h = m_scroll_v = 0;
	max_v = max_h = 0;
	m_tx = m_ty = 0;
	innerWidget = new Widget();
	innerWidget->SetId( GetId() + "_innerWidget" );
	background_color = 0;
	Widget::AddControl(innerWidget);
	setInterceptMask(imask::mwheel);
}


Container::~Container() {
}

void Container::OnMWheel( int updown ) {
	if(!getEngine()) return;
	if(getEngine()->GetSelectedWidget() == (Control*)innerWidget or isSelected()) {
		if(m_scroll_v && !(m_scroll_h && getEngine()->GetSelectedControl() == m_scroll_h)) {
			m_scroll_v->OnMWheel(updown);
		} else if(m_scroll_h) {
			m_scroll_h->OnMWheel(updown);
		}
	}
}

void Container::OnMouseDown( int mX, int mY, MouseButton which_button ) {
	// intercept();
}

void Container::OnMouseUp( int x, int y, MouseButton which_button ) {
	// intercept();
}

void Container::OnMouseMove( int x, int y, bool lmb ) {
	// intercept();
}

int Container::depth = -1;


const int thickness = 8;

void Container::Render( Point pos, bool isSelected ) {
	// Control::Render(pos,isSelected);
	RenderWidget(pos, isSelected);
}

void Container::onRectChange() {
	const Rect& r = GetRect();

	checkOverflow();
}


void Container::AddControl( Control* control ) {
	AddItem(control);
}

void Container::AddItem( Control* control ) {
	innerWidget->AddControl( control );
	
	const Rect& r2 = GetRect();
	
	// detect overflow
	const Rect &r = control->GetRect();
	if( r.x + r.w > r2.w ) {
		if( r.x + r.w - r2.w > max_h ) {
			max_h = r.x + r.w - r2.w;
		}
		overflow_h = true; // horizontal overflow
	}
	if( r.y + r.h > r2.h ) {
		if( r.y + r.h - r2.h > max_v ) {
			max_v = r.y + r.h - r2.h;
		}
		overflow_v = true; // vertical overflow
	}
	
	onOverflow();
	m_num_controls++;
}

void Container::checkOverflow() {
	const Rect& r2 = GetRect();
	
	auto controls = innerWidget->GetControls();
	for(auto &control : controls) {
		// detect overflow
		const Rect &r = control->GetRect();
		max_h = 0;
		max_v = 0;
		if( r.x + r.w > r2.w ) {
			max_h = std::max(r.x+r.w-r2.w + thickness + 10, max_h);
			overflow_h = true; // horizontal overflow
		}
		if( r.y + r.h > r2.h ) {
			max_v = std::max(r.y + r.h - r2.h + thickness + 10, max_v);
			overflow_v = true; // vertical overflow
		}
	}
	onOverflow();
}

void Container::onOverflow() {
	
	Rect r = GetRect();
	if( !m_scroll_v && overflow_v ) {
		m_scroll_v = new ScrollBar();
		m_scroll_v->SetVertical( true );
		m_scroll_v->OnEvent( "change", [this](Args& args) {
			ScrollBar *sb = (ScrollBar*)args.control;
			int val = sb->GetValue();
			m_ty = -val * max_v / 100;
			innerWidget->SetOffset(innerWidget->GetOffset().x, m_ty);
		});
		
		Widget::AddControl(m_scroll_v);
	}
	
	// if horizontal scrollbar needs to be added
	if( !m_scroll_h && overflow_h ) {
		m_scroll_h = new ScrollBar();
		m_scroll_h->OnEvent( "change", [this](Args& args) {
			ScrollBar *sb = (ScrollBar*)args.control;
			int val = sb->GetValue();
			m_tx = -val * max_h / 100;
			
			innerWidget->SetOffset(m_tx, innerWidget->GetOffset().y);
		});
		
		
		Widget::AddControl(m_scroll_h);
	}
	
	if(overflow_v || overflow_h) {
		int vert = 0;
		int horiz = 0;
		
		if(overflow_v) {
			vert = -thickness;
		}
		if(overflow_h) {
			horiz = -thickness;
		}
		
		Layout l = innerWidget->GetLayout();
		l.SetSize(vert,1,horiz,1);
		innerWidget->SetLayout(l);
		
		if(overflow_v) {
			Layout vl = Layout::parseRect("R,0");
			vl.SetSize(thickness, 0, horiz, 1);
			m_scroll_v->SetLayout(vl);
		}
		
		if(overflow_h) {
			Layout hl = Layout::parseRect("0,B");
			hl.SetSize(vert, 1, thickness, 0);
			m_scroll_h->SetLayout(hl);
		}
		
		innerWidget->SetId( GetId() + "_innerWidget" );
		ProcessLayout();
		
		Rect r = GetRect();
		if(overflow_h) {
			int m = max_h + r.w;
			if(m && r.w > 0) {
				m_scroll_h->SetSliderSize( r.w*r.w / m );
			}
		}
		if(overflow_v) {
			int m = max_v + r.h;
			if(m > r.h && r.h > 0) {
				m_scroll_v->SetSliderSize( r.h*r.h / m );
			}
		}
	}
	
}

Container* Container::Clone() {
	Container* c = new Container;
	copyStyle(c);
	return c;
}

void Container::OnSetStyle(std::string& style, std::string& value) {
	STYLE_SWITCH {
		_case("background_color"):
			background_color = Color::ParseColor(value);
	}
}

}
