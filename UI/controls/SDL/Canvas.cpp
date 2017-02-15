#include "Canvas.hpp"
#include "../../common/SDL/Drawing.hpp"

namespace ng {
Canvas::Canvas() {
	setType( "canvas" );
	pixel_size = 1;
	pixel_color = 0xffffffff;
	// m_drawing = nullptr;
	// tex_drawing = 0;
	last_x = last_y = -1;
	m_is_readonly = false;
	align_to_grid = false;
	display_grid = false;
	maketex = true;
	m_style.background_color = 0;
	grid_color = 0xff808080;
}

Canvas::~Canvas() {
	// Drawing::DeleteTexture(tex_drawing);
	
}

void Canvas::Render( Point pos, bool isSelected ) {
	const Rect& rect = GetRect();
	int x = rect.x + pos.x;
	int y = rect.y + pos.y;
	
	Drawing::FillRect(x, y, rect.w, rect.h, m_style.background_color );
	Control::Render(pos, isSelected);
	/*
	if(display_grid) {
		unsigned int* p = (unsigned int*)m_drawing->pixels;
		int w = m_drawing->w;
		int h;
		// horizontal
		for(int y=0; y < rect.h; y += pixel_size) {
			h = y*w;
			for(int x=0; x < rect.w; x++) {
				p[x + h] = grid_color;
			}
		}
		// vertical
		for(int y=0; y < rect.h; y++) {
			h = y*w;
			for(int x=0; x < rect.w; x += pixel_size) {
				p[x + h] = grid_color;
			}
		}
	}
	*/
	for(int i=MAX_LAYERS-1; i >= 0; i--)
		Drawing::TexRect( x, y, layers[i] );

}

void Canvas::RefreshTexture() {
	maketex = true;
}

void Canvas::OnSetStyle(std::string& style, std::string& value) {
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


void Canvas::OnMouseDown( int mX, int mY, MouseButton button ) {
	if(!m_is_readonly)
		PutPixel(mX-GetRect().x, mY-GetRect().y);
	m_is_mouseDown = true;
}

void Canvas::PutPixel(int x, int y, int layer) {
	if(x < 0 or y < 0) return;
	if(align_to_grid) {
		x = x - x%pixel_size;
		y = y - y%pixel_size;
	}
	int i,j;
	Size &r = layers[0].GetTextureSize();
	for(i=0; i < pixel_size; i++) {
		for(j=0; j < pixel_size; j++) {
			layers[layer].Pixel(Point(x+j,y+i), pixel_color);
		}
	}
}

void Canvas::Clear(int color, int layer) {
	layers[layer].Clear(m_style.background_color);
}

Canvas* Canvas::Clone() {
	Canvas* c = new Canvas();
	copyStyle(c);
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

		layers[0].Line(Point(x, y), Point(last_x, last_y), pixel_color);
		
		last_x = x;
		last_y = y;
		
		emitEvent( "change" );
	}
}

void Canvas::OnMouseUp( int mX, int mY, MouseButton which_button ) {
	m_is_mouseDown = false;
	last_x = last_y = -1;
}

void Canvas::OnLostFocus() {
	m_is_mouseDown = false;
}

void Canvas::onPositionChange() {

	const Rect &r = GetRect();
	for(int i=0; i < MAX_LAYERS; i++) {
		layers[i].Resize( r.w, r.h );
		layers[i].Clear(0);
	}
	
}
}
