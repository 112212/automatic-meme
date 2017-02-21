// #include <SDL2/SDL2_gfxPrimitives.h>
#include "../../common/SDL/Drawing.hpp"
#include "Container.hpp"
#include "../../Gui.hpp"
#include <iostream>

// select only one
#define CLIP_METHOD_STENCIL
// #define CLIP_METHOD_SCISSOR

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
	innerWidget->SetRenderable(false);
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
		}
		else if(m_scroll_h) {
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


#ifdef CLIP_METHOD_SCISSOR
	Rect getIntersectingRectangle(const Rect &a, const Rect &b) {
		int Ax = std::max(a.x, b.x);
		int Ay = std::max(a.y, b.y);
		int Bx = std::min(a.x+a.w,b.x+b.w);
		int By = std::min(a.y+a.h,b.y+b.h);
		if(Ax < Bx && Ay < By) {
			return {Ax,Ay,Bx-Ax,By-Ay};
		}
		return {0,0,0,0};
	}
#endif

void Container::Render( Point pos, bool isSelected ) {
	// Control::Render(pos,isSelected);
	RenderWidget(pos, isSelected);	
	const Rect& r = GetRect();
	int x = r.x + pos.x;
	int y = r.y + pos.y;
	
	bool overflow = overflow_h || overflow_v;
	
	int w = r.w;
	int h = r.h;
	
	if(overflow_h) {
		h -= thickness + 1;
	}
	if(overflow_v) {
		w -= thickness + 1;
	}
	
#ifdef CLIP_METHOD_SCISSOR
	Rect clipRect = {x,y,w,r.h};
	bool was_enabled = glIsEnabled( GL_SCISSOR_TEST );
	Rect old_box;
	
	Drawing::FillRect(x, y, w, h, background_color);
	
	int x1,y1;
	Drawing::GetResolution(x1,y1);
	if(was_enabled) {
		glGetIntegerv( GL_SCISSOR_BOX, (GLint*)&old_box );
		old_box.y = y1 - (old_box.y+old_box.h);
		clipRect = getIntersectingRectangle(old_box, clipRect);
	} else {
		glEnable( GL_SCISSOR_TEST );
	}
	
	glScissor( clipRect.x, y1-(clipRect.y+clipRect.h), clipRect.w, clipRect.h );
#endif
	
#ifdef CLIP_METHOD_STENCIL

	bool was_enabled = glIsEnabled( GL_STENCIL_TEST );
	if(depth == -1) {
		glClearStencil( 0 );
		glClear( GL_STENCIL_BUFFER_BIT );
		depth = 0;
	}
	
	int old_depth;
	if(was_enabled) {
		glGetIntegerv( GL_STENCIL_REF, &old_depth );
	} else {
		glEnable( GL_STENCIL_TEST );
	}
	
	glColorMask( GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE );
	
	glStencilMask(0xff);
	
	if(!was_enabled) {
		glStencilFunc( GL_LESS, 1, 0xff);
		glStencilOp( GL_REPLACE, GL_REPLACE, GL_REPLACE );
	} else {
		glStencilFunc( GL_LEQUAL, depth, 0xff );
		glStencilOp( GL_KEEP, GL_INCR, GL_INCR );
	}
	
	Drawing::FillRect(x, y, w, h, background_color);
	
	glColorMask( GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE );

	glStencilFunc( GL_LESS, depth, 0xff );
	depth++;
	glStencilMask(0);
	glStencilOp( GL_KEEP, GL_KEEP, GL_KEEP );
#endif
	// Drawing::FillRect(x, y, w, h, background_color);
	Control::Render(pos,isSelected);
	innerWidget->Render(Point(x,y),isSelected);
	
	
#ifdef CLIP_METHOD_STENCIL
	if(was_enabled) {
		glStencilFunc( GL_LESS, old_depth, 0xff );
	} else {
		glEnable( GL_STENCIL_TEST );
		glColorMask( GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE );
		// glColorMask( GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE );
		depth = 0;
		glStencilMask(0xff);
		glStencilFunc( GL_NEVER, 0, 0xff );
		glStencilOp( GL_REPLACE, GL_REPLACE, GL_REPLACE );
		Drawing::FillRect(x, y, w, h, 0);
		glColorMask( GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE );
		glStencilMask(0);
		glDisable( GL_STENCIL_TEST );
	}
#endif
	
#ifdef CLIP_METHOD_SCISSOR
	if(!was_enabled) {
		glDisable( GL_SCISSOR_TEST );
	} else {
		glScissor( old_box.x, y1-(old_box.y+old_box.h), old_box.w, old_box.h );
	}
#endif
	
}

void Container::onRectChange() {
	const Rect& r = GetRect();
	if( r.w > max_h ) max_h = r.w;
	if( r.h > max_v ) max_v = r.h;
	
	innerWidget->SetRect(0,0, r.w-10, r.h-10);
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
		if( r.x + r.w - r2.w > max_h )
			max_h = r.x + r.w - r2.w;
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

void Container::onOverflow() {
	
	const Rect& r = GetRect();
	if( !m_scroll_v && overflow_v ) {
		m_scroll_v = new ScrollBar();
		m_scroll_v->SetVertical( true );
		m_scroll_v->SetRect( r.w-thickness, 0, thickness, r.h - thickness );
		m_scroll_v->OnEvent( "change", [this](Control *c, Argv& argv) {
			ScrollBar *sb = (ScrollBar*)c;
			int val = sb->GetValue();
			m_ty = -val * max_v / 100;
			innerWidget->SetOffset(innerWidget->GetOffset().x, m_ty);
		});
		Anchor a = m_scroll_v->GetAnchor();
		a.x = r.w-thickness;
		a.y = 0;
		a.absolute_coordinates = true;
		m_scroll_v->SetAnchor(a);
		Widget::AddControl(m_scroll_v);
	}
	if( !m_scroll_h && overflow_h ) {
		m_scroll_h = new ScrollBar();
		m_scroll_h->SetRect( 0, r.h-thickness, r.w - thickness, thickness );
		m_scroll_h->OnEvent( "change", [this](Control* c, Argv& argv) {
			ScrollBar *sb = (ScrollBar*)c;
			int val = sb->GetValue();
			m_tx = -val * max_h / 100;
			
			innerWidget->SetOffset(m_tx, innerWidget->GetOffset().y);
		});
		Anchor a = m_scroll_h->GetAnchor();
		a.x = 0;
		a.y = r.h-thickness;
		a.absolute_coordinates = true;
		m_scroll_h->SetAnchor(a);
		Widget::AddControl(m_scroll_h);
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
			background_color = Colors::ParseColor(value);
	}
}

}
