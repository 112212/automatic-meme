#include "Container.hpp"
#include "../Gui.hpp"
#include <iostream>


namespace ng {
Container::Container() {
	setType( "container" );
	m_is_mouseDown = false;
	m_num_controls = 0;
	overflow_h = overflow_v = false;
	m_scroll_h = m_scroll_v = 0;
	max_v = max_h = 0;
	m_tx = m_ty = 0;
	innerWidget = createControl("control", "container");
	innerWidget->SetLayout("0,0,W,H");
	background_color = 0;
	Control::AddControl(innerWidget);
	setInterceptMask(imask::mwheel);
}

Container::~Container() {
}

void Container::OnMWheel( int updown ) {
	std::cout << "onmwheel sel: " << GetId() << " " << getEngine()->GetSelectedWidget()->GetId() << "\n";
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
	Control::Render(pos,isSelected);
	RenderWidget(pos, isSelected);
}

void Container::onRectChange() {
	const Rect& r = GetRect();

	checkOverflow();
}


void Container::AddControl( Control* control, bool processlayout ) {
	AddItem(control);
}

void Container::AddItem( Control* control ) {
	innerWidget->AddControl( control );
	
	const Rect& r = GetRect();
	
	ProcessLayout(false);
	
	// detect overflow
	const Rect &cr = control->GetRect();
	
	if( cr.x + cr.w > r.w ) {
		if( cr.x + cr.w - r.w > max_h ) {
			max_h = cr.x + cr.w - r.w;
		}
		overflow_h = true; // horizontal overflow
		// std::cout << "container AddItem: h " << control->GetId() << "\n";
		// std::cout << "max h: " << max_h << ", " << cr.x+cr.w << " , " << r.w << "\n";
	}
	if( cr.y + cr.h > r.h ) {
		if( cr.y + cr.h - r.h > max_v ) {
			max_v = cr.y + cr.h - r.h;
		}
		overflow_v = true; // vertical overflow
		// std::cout << "container AddItem: v\n";
	}
	
	onOverflow();
	m_num_controls++;
}

void Container::checkOverflow() {
	const Rect& r2 = GetRect();
	
	auto controls = innerWidget->GetControls();
	max_h = 0;
	max_v = 0;
	for(auto &control : controls) {
		// detect overflow
		const Rect &r = control->GetRect();
		
		if( r.x + r.w > r2.w ) {
			max_h = std::max(r.x + r.w - r2.w + thickness + 10, max_h);
			overflow_h = true; // horizontal overflow
			
			// std::cout << GetId() << " hoverflow\n";
		}
		if( r.y + r.h > r2.h ) {
			max_v = std::max(r.y + r.h - r2.h + thickness + 10, max_v);
			overflow_v = true; // vertical overflow
			// std::cout << GetId() << " voverflow\n";
		}
	}
	onOverflow();
}

void Container::onOverflow() {
	
	Rect r = GetRect();
	if( !m_scroll_v && overflow_v ) {
		m_scroll_v = createControl<ScrollBar>("scrollbar", "vscroll");
		m_scroll_v->SetVertical( true );
		m_scroll_v->OnEvent( "change", [this](Args& args) {
			ScrollBar *sb = (ScrollBar*)args.control;
			int val = sb->GetValue();
			m_ty = -val * max_v / 100;
			innerWidget->SetOffset(innerWidget->GetOffset().x, m_ty);
		});
		
		Control::AddControl(m_scroll_v);
	}
	
	// if horizontal scrollbar needs to be added
	if( !m_scroll_h && overflow_h ) {
		m_scroll_h = createControl<ScrollBar>("scrollbar", "hscroll");
		m_scroll_h->OnEvent( "change", [this](Args& args) {
			ScrollBar *sb = (ScrollBar*)args.control;
			int val = sb->GetValue();
			m_tx = -val * max_h / 100;
			
			innerWidget->SetOffset(m_tx, innerWidget->GetOffset().y);
		});
		
		
		Control::AddControl(m_scroll_h);
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
			Layout vl("R,T");
			vl.SetSize(thickness, 0, horiz, 1);
			m_scroll_v->SetLayout(vl);
		}
		
		if(overflow_h) {
			Layout hl("L,B");
			hl.SetSize(vert, 1, thickness, 0);
			m_scroll_h->SetLayout(hl);
		}
		
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
	Container* c = new Container();
	copyStyle(c);
	return c;
}

void Container::OnSetStyle(std::string& style, std::string& value) {
	STYLE_SWITCH {
		_case("background_color"):
			background_color = Color(value).GetUint32();
	}
}

}
