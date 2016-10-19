#include "Canvas.hpp"
#include "../../common/SDL/Drawing.hpp"

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
	m_style.background_color = 0;
	grid_color = 0xff808080;
}

Canvas::~Canvas() {
	Drawing::DeleteTexture(tex_drawing);
}

void Canvas::Render( Point pos, bool isSelected ) {
	const Rect& rect = GetRect();
	int x = rect.x + pos.x;
	int y = rect.y + pos.y;
	
	Drawing::FillRect(x, y, rect.w, rect.h, m_style.background_color );
	
	if(display_grid) {
		unsigned int* p = (unsigned int*)m_drawing->pixels;
		int w = m_drawing->w;
		int h;
		// horizontal
		for(int y=0; y < rect.h; y+=pixel_size) {
			h = y*w;
			for(int x=0; x < rect.w; x++) {
				p[x + h] = grid_color;
			}
		}
		// vertical
		for(int y=0; y < rect.h; y++) {
			h = y*w;
			for(int x=0; x < rect.w; x+=pixel_size) {
				p[x + h] = grid_color;
			}
		}
	}
	
	if(maketex) {
		
		maketex = false;
		SDL_Rect r = { x, y, rect.w, rect.h };
		tex_drawing = Drawing::GetTextureFromSurface( m_drawing, tex_drawing );
		Drawing::TexRect( r.x, r.y, r.w, r.h, tex_drawing );
	} else if(m_drawing && tex_drawing) {
		Drawing::TexRect( x, y, m_drawing->w, m_drawing->h, tex_drawing );
	}
	
	Control::Render(pos, isSelected);	
}

void Canvas::RefreshTexture() {
	maketex = true;
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
			if(value[0] == '#') SetPixelColor( Colors::ParseColor(value)  ); 
		_case("grid_color"):
			grid_color = Colors::ParseColor(value);
		_case("readonly"):
			SetReadOnly( value == "true" );
	}
}

void Canvas::SetBackgroundColor(int color) {
	m_style.background_color = color;
}

void Canvas::SetPixelSize(int size) {
	pixel_size = size;
}


void Canvas::OnMouseDown( int mX, int mY ) {
	if(!m_is_readonly)
		PutPixel(mX-GetRect().x, mY-GetRect().y);
	
	m_is_mouseDown = true;
}

void Canvas::PutPixel(int x, int y) {
	if(x < 0 or y < 0) return;
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
				if(x+j < m_drawing->w)
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
		PutPixel(x+diffx*interp, y+diffy*interp);
	}
}

void Canvas::Clear(int color) {
	SDL_FillRect(m_drawing, 0, color);
}

Canvas* Canvas::Clone() {
	Canvas* c = new Canvas;
	*c = *this;
	return c;
}

void Canvas::OnMouseMove( int mX, int mY, bool mouseState ) {
	
	if(!m_is_readonly && mouseState && check_collision(mX, mY)) {
		
		int x = mX - GetRect().x;
		int y = mY - GetRect().y;
		
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
		m_drawing = SDL_CreateRGBSurface(0,GetRect().w,GetRect().h,32,R,G,B,A );
		maketex = true;
		SDL_FillRect( m_drawing, NULL, 0 );
	}
}
}
