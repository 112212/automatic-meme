#include "Canvas.hpp"
#include "../../common/SDL/Drawing.hpp"

//#define SELECTION_MARK
namespace ng {
Canvas::Canvas() {
	setType( TYPE_CANVAS );
	pixel_size = 1;
	pixel_color = 0xffffffff;
	m_drawing = nullptr;
	tex_drawing = 0;
	last_x = last_y = -1;
	m_is_readonly = false;
	align_to_grid = false;
	display_grid = false;
	maketex=true;
	background_color = 0;
	grid_color = 0xff808080;
}

Canvas::~Canvas() {
}

void Canvas::Render( SDL_Rect pos, bool isSelected ) {
	int x = m_rect.x + pos.x;
	int y = m_rect.y + pos.y;
	
	// crtanje sdl draw
	Drawing::FillRect(x, y, m_rect.w, m_rect.h, background_color );
	Drawing::Rect( x, y, m_rect.w, m_rect.h, Colors::Gray );
	
	if(display_grid) {
		unsigned int* p = (unsigned int*)m_drawing->pixels;
		int w = m_drawing->w;
		int h;
		// horizontalno
		for(int y=0; y < m_rect.h; y+=pixel_size) {
			h = y*w;
			for(int x=0; x < m_rect.w; x++) {
				p[x + h] = grid_color;
			}
		}
		// vertikalno
		for(int y=0; y < m_rect.h; y++) {
			h = y*w;
			for(int x=0; x < m_rect.w; x+=pixel_size) {
				p[x + h] = grid_color;
			}
		}
	}
	
	if(maketex) {
		// TODO: fix this
		// m_tex_drawing = SDL_CreateTextureFromSurface( ren, m_drawing );
		
		maketex = false;
		SDL_Rect r = { x, y, m_rect.w, m_rect.h };
		// TODO: fix this
		// SDL_RenderCopy( ren, m_tex_drawing, NULL, &r );
		tex_drawing = Drawing::GetTextureFromSurface( m_drawing, tex_drawing );
		Drawing::TexRect( r.x, r.y, r.w, r.h, tex_drawing );
	} else if(m_drawing && tex_drawing) {
		//CSurface::OnDraw( ren, m_drawing, x, y );
		
		// TODO: fix this
		// CSurface::OnDraw(ren, m_tex_drawing, m_drawing, x, y);
		
		Drawing::TexRect( x, y, m_drawing->w, m_drawing->h, tex_drawing );
	}

	
}

void Canvas::STYLE_FUNC(value) {
	STYLE_SWITCH {
		_case("grid"):
			display_grid = (value == "true" );
			maketex=true;
		_case("pixel_size"):
			SetPixelSize( std::stoi( value ) );
		_case("align_to_grid"):
			SetAlignToGrid( value == "true" );
		_case("color"):
			if(value[0] == '#') SetPixelColor( 0xff000000 | std::stoi(value.substr(1), 0, 16) ); 
		_case("background_color"):
			SetBackgroundColor( Colors::ParseColor(value) );
		_case("grid_color"):
			grid_color = Colors::ParseColor(value);
	}
}

void Canvas::SetBackgroundColor(int color) {
	background_color = color;
}

void Canvas::SetPixelSize(int size) {
	pixel_size = size;
}


void Canvas::OnMouseDown( int mX, int mY ) {
	if(!m_is_readonly)
		put_pixel(mX-m_rect.x, mY-m_rect.y);
	
	m_is_mouseDown = true;
}

void Canvas::put_pixel(int x, int y) {
	if(align_to_grid) {
		x = x - x%pixel_size;
		y = y - y%pixel_size;
	}
	int i,j;
	if(m_drawing) {
		unsigned int *pixels = (unsigned int*)m_drawing->pixels;
		for(i=0; i < pixel_size; i++) {
			if(y+i < m_drawing->h)
			for(j=0; j < pixel_size; j++) {
				pixels[(y+i)*m_drawing->w + x+j] = pixel_color;
			}
		}
	}
	maketex = true;
}

void Canvas::put_pixel_interpolate(int x, int y, int last_x, int last_y) {
	int diffx = last_x-x;
	int diffy = last_y-y;
	float step = 1/(float)std::max(abs(diffx), abs(diffy));
	float interp;
	for(interp=0; interp < 1; interp+=step) {
		put_pixel(x+diffx*interp, y+diffy*interp);
	}
}

void Canvas::Clear(int color) {
	SDL_FillRect(m_drawing, 0, color);
}


void Canvas::OnMouseMove( int mX, int mY, bool mouseState ) {
	
	if(!m_is_readonly && mouseState && check_collision(mX, mY)) {
		
		int x = mX - m_rect.x;
		int y = mY - m_rect.y;
		
		if(last_x < 0 || last_y < 0) {
			last_x = x;
			last_y = y;
		}

		put_pixel_interpolate(x, y, last_x, last_y);
		
		last_x = x;
		last_y = y;
		
		emitEvent( EVENT_CANVAS_CHANGE );
	}
}

void Canvas::OnMouseUp( int mX, int mY ) {
	m_is_mouseDown = false;
	last_x = last_y = -1;
}

void Canvas::OnLostFocus() {
	m_is_mouseDown = false;
}


void Canvas::onPositionChange() {

	int R=0x00ff0000,
		G=0x0000ff00,
		B=0x000000ff,
		A=0xff000000;
	if(!m_drawing) {
		m_drawing = SDL_CreateRGBSurface(0,m_rect.w,m_rect.h,32,R,G,B,A );
		maketex = true;
		SDL_FillRect( m_drawing, NULL, 0 );
	}
}
}
