// #include <SDL2/SDL2_gfxPrimitives.h>
#include "../common/SDL/Drawing.hpp"
#include "Container.hpp"
#include <SFML/OpenGL.hpp>
#include "../Gui.hpp"

// select only one
#define CLIP_METHOD_STENCIL
// #define CLIP_METHOD_SCISSOR

namespace ng {
Container::Container() {
	m_is_mouseDown = false;
	#ifdef USE_SFML
		m_rectShape.setFillColor( sf::Color::Black );
		m_rectShape.setOutlineColor( sf::Color::White );
		m_rectShape.setOutlineThickness( 1 );
	#elif USE_SDL
	#endif
	m_num_controls = 0;
	overflow_h = overflow_v = false;
	m_scroll_h = m_scroll_v = 0;
	max_v = max_h = 0;
	m_tx = m_ty = 0;
	innerWidget = new Widget;
	innerWidget->SetVisible(false);
	background_color = 0;
	Widget::AddControl(innerWidget);
	setInterceptMask(mwheel | mouse_down | mouse_move);
}


Container::~Container() {
	delete innerWidget;
}

void Container::OnMWheel( int updown ) {
	// TODO: scroll vertically default and horizontally if pointed on it
	if(!getEngine()) return;
	if(getEngine()->GetSelectedWidget() == (Control*)innerWidget or 
		isThisWidgetSelected()) {
			
		if(m_scroll_v && !(m_scroll_h && getEngine()->GetSelectedControl() == m_scroll_h)) {
			m_scroll_v->OnMWheel(updown);
		}
		else if(m_scroll_h) {
			m_scroll_h->OnMWheel(updown);
		}
	}
}

void Container::OnMouseDown( int mX, int mY ) {
	// intercept();
	
}

void Container::OnMouseUp( int x, int y ) {
	// intercept();
}

void Container::OnMouseMove( int x, int y, bool lmb ) {
	// cout << "hehehehe\n";
	// sendGuiCommand(GUI_UNSELECT_WIDGET);
	// intercept();
}

int Container::depth = -1;


const int thickness = 8;

#ifdef USE_SFML
void Container::Render( sf::RenderTarget& ren, sf::RenderStates states, bool isSelected ) {
	int x = m_rect.x;
	int y = m_rect.y;
	
	int w = m_rect.w;
	int h = m_rect.h;
	
	if(overflow_h) {
		h -= thickness + 1;
	}
		
	if(overflow_v) {
		w -= thickness + 1;
	}
	
	bool overflow = overflow_h || overflow_v;
	
	if(depth == 0) {
		glClearStencil( 0 );
		glClear( GL_STENCIL_BUFFER_BIT );
	} else if(depth > 200) {
		depth = 0;
		glClearStencil( 0 );
		glClear( GL_STENCIL_BUFFER_BIT );
	}
	
	ren.draw( m_rectShape );
	
	glColorMask( GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE );
	glEnable( GL_STENCIL_TEST );
	glStencilOp( GL_INCR, GL_INCR, GL_INCR );
	glStencilFunc( GL_NEVER, 0, 0 );
	
	if( overflow )
		m_rectShape.setSize( sf::Vector2f( w, h ) );
	
	ren.draw( m_rectShape );
	
	glColorMask( GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE );
	glStencilFunc( GL_LESS, depth++, 0xffffffff );
	glStencilOp( GL_KEEP, GL_KEEP, GL_KEEP );
	
	// TODO: translate states by m_rect.{x,y}
	states.transform.translate(x, y);
	innerWidget->Render(ren,states,isSelected);
	
	glDisable( GL_STENCIL_TEST );
	
	states.transform.translate(-x, -y);
	RenderWidget(ren, sf::RenderStates::Default, false);
}
void Container::onPositionChange() {
	m_rectShape.setPosition( m_rect.x, m_rect.y );
	m_rectShape.setSize( sf::Vector2f( m_rect.w, m_rect.h ) );
	
	if( m_rect.w > max_h ) max_h = 0;//m_rect.w;
	if( m_rect.h > max_v ) max_v = 0;//m_rect.h;
	innerWidget->SetRect(0,0, m_rect.w-10, m_rect.h-10);
}

#elif USE_SDL

#ifdef CLIP_METHOD_SCISSOR
Rect getIntersectingRectangle(Rect &a, Rect &b) {
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

void Container::Render(  SDL_Rect pos, bool isSelected ) {
	int x = m_rect.x + pos.x;
	int y = m_rect.y + pos.y;
	
	bool overflow = overflow_h || overflow_v;
	
	int w = m_rect.w;
	int h = m_rect.h;
	
	if(overflow_h) {
		h -= thickness + 1;
	}
	if(overflow_v) {
		w -= thickness + 1;
	}
	
	Drawing::Rect(x,y,m_rect.w, m_rect.h, 0xffffffff);
	
#ifdef CLIP_METHOD_STENCIL
	bool was_enabled = glIsEnabled( GL_STENCIL_TEST );
	if(depth == -1) {
		glClearStencil( 0 );
		glClear( GL_STENCIL_BUFFER_BIT );
		depth = 0;
	}
#endif
	
#ifdef CLIP_METHOD_SCISSOR
	Rect clipRect = {x,y,w,m_rect.h};
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
	int old_depth;
	if(was_enabled) {
		glGetIntegerv( GL_STENCIL_REF, &old_depth );
	} else {
		glEnable( GL_STENCIL_TEST );
	}
	
	// glColorMask( GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE );
	
	
	if(!was_enabled) {
		glStencilFunc( GL_LESS, 1, 0xffffffff );
		glStencilOp( GL_REPLACE, GL_REPLACE, GL_REPLACE );
	} else {
		glStencilFunc( GL_LEQUAL, depth, 0xffffffff );
		glStencilOp( GL_KEEP, GL_INCR, GL_INCR );
	}
	
	Drawing::FillRect(x, y, w, h, background_color);
	
	// glColorMask( GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE );

	glStencilFunc( GL_LESS, depth, 0xffffffff );
	depth++;
	glStencilOp( GL_KEEP, GL_KEEP, GL_KEEP );
#endif

	innerWidget->Render({x,y},isSelected);
	
	
#ifdef CLIP_METHOD_STENCIL
	if(was_enabled) {
		glStencilFunc( GL_LESS, old_depth, 0xffffffff );
	} else {
		glEnable( GL_STENCIL_TEST );
		glColorMask( GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE );
		depth = 0;
		glStencilFunc( GL_ALWAYS, 0, 0xffffffff );
		glStencilOp( GL_REPLACE, GL_REPLACE, GL_REPLACE );
		Drawing::FillRect(x, y, w, h, 0);
		glColorMask( GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE );
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
	
	
	RenderWidget(pos, isSelected);
}

void Container::onPositionChange() {
	if( m_rect.w > max_h ) max_h = 0; //m_rect.w;
	if( m_rect.h > max_v ) max_v = 0; //m_rect.h;
	
	innerWidget->SetRect(0,0, m_rect.w-10, m_rect.h-10);
}

#endif

void Container::AddControl( Control* control ) {
	AddItem(control);
}
void Container::AddItem( Control* control ) {

	innerWidget->AddControl( control );
	
	// detect overflow
	const Rect &r = control->GetRect();
	if( r.x + r.w > m_rect.w ) {
		if( r.x + r.w - m_rect.w > max_h )
			max_h = r.x + r.w - m_rect.w;
		overflow_h = true; // horizontal overflow
	}
	if( r.y + r.h > m_rect.h ) {
		if( r.y + r.h - m_rect.h > max_v ) {
			max_v = r.y + r.h - m_rect.h;
		}
		overflow_v = true; // vertical overflow
	}
	onOverflow();
	m_num_controls++;
}

void Container::onOverflow() {
	if( !m_scroll_v && overflow_v ) {
		m_scroll_v = new ScrollBar();
		m_scroll_v->SetVertical( true );
		m_scroll_v->SetRect( m_rect.w-thickness, 0, thickness, m_rect.h - thickness );
		m_scroll_v->SubscribeEvent( EVENT_SCROLLBAR_CHANGE, [this](Control *c) {
			ScrollBar *sb = (ScrollBar*)c;
			int val = sb->GetValue();
			m_ty = -val * max_v / 100;
			// innerWidget->SetPosition(innerWidget->GetRect().x, m_ty);
			innerWidget->SetOffset(innerWidget->GetOffset().x, m_ty);
		});
		Widget::AddControl(m_scroll_v);
	}
	if( !m_scroll_h && overflow_h ) {
		m_scroll_h = new ScrollBar();
		m_scroll_h->SetRect( 0, m_rect.h-thickness, m_rect.w - thickness, thickness );
		m_scroll_h->SubscribeEvent( EVENT_SCROLLBAR_CHANGE, [this](Control* c) {
			ScrollBar *sb = (ScrollBar*)c;
			int val = sb->GetValue();
			m_tx = -val * max_h / 100;
			// innerWidget->SetPosition(m_tx, innerWidget->GetRect().y);
			innerWidget->SetOffset(m_tx, innerWidget->GetOffset().y);
		});
		Widget::AddControl(m_scroll_h);
	}
}

void Container::STYLE_FUNC(value) {
	STYLE_SWITCH {
		_case("background_color"):
			background_color = std::stoi(value);
	}
}

// void Container::SetGrid( int x, int y ) {
	// m_grid_w = x;
	// m_grid_h = y;
// }

}
