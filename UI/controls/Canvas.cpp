#include "Canvas.hpp"

namespace ng {
Canvas::Canvas() {
	setType( "canvas" );
	pixel_size = 1;
	pixel_color = 0xffffffff;

	last_x = last_y = -1;
	m_is_readonly = false;
	align_to_grid = false;
	display_grid = false;
	m_should_update_texture_sizes = false;
	maketex = true;
	m_style.background_color = 0;
	grid_color = 0xff808080;
	AddLayers(1);
	Clear(0,0);
}

Canvas::~Canvas() {
	
}

void Canvas::Render( Point pos, bool isSelected ) {
	const Rect& rect = GetRect();
	int x = rect.x + pos.x;
	int y = rect.y + pos.y;
	
	Drawing().FillRect(x, y, rect.w, rect.h, m_style.background_color );
	Control::Render(pos, isSelected);
	// 
	for(auto rit = layers.rbegin(); rit != layers.rend(); rit++) {
		Drawing().TexRect( x, y, &(*rit) );
	}

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
			if(value[0] == '#') SetPixelColor( Color::ParseColor(value)  ); 
		_case("grid_color"):
			grid_color = Color::ParseColor(value);
		_case("readonly"):
			SetReadOnly( value == "true" );
		_case("layers"):
			SetLayers(std::stoi(value));
	}
}

void Canvas::SetBackgroundColor(int color) {
	m_style.background_color = color;
}

void Canvas::SetPixelSize(int size) {
	pixel_size = size;
}

BasicImage& Canvas::GetLayer(int layer) {
	if(layer < layers.size()) {
		return layers[layer];
	} else {
		const Rect &r = GetRect();
		layers.emplace_back(r.w, r.h);
		return layers.back();
	}
}

void Canvas::AddLayers(int layers) {
	const auto& r = GetRect();
	for(int i=0; i < layers; i++) {
		this->layers.emplace_back(r.w, r.h);
	}
}

void Canvas::SetLayers(int layers) {
	const auto& r = GetRect();
	while(layers < this->layers.size()) {
		this->layers.pop_back();
	}
	
	while(layers > this->layers.size()) {
		this->layers.emplace_back(r.w, r.h);
	}
}

void Canvas::OnMouseDown( int mX, int mY, MouseButton button ) {
	if(!m_is_readonly) {
		PutPixel(mX-GetRect().x, mY-GetRect().y);
	}
	m_is_mouseDown = true;
}

void Canvas::PutPixel(int x, int y, int layer) {
	if(x < 0 or y < 0) return;
	if(layer >= layers.size()) {
		return;
	}
	
	x -= pixel_size/2;
	y -= pixel_size/2;
	
	if(align_to_grid) {
		x = x - x%pixel_size;
		y = y - y%pixel_size;
	}
	int i,j;
	// Size &r = layers[0].GetTextureSize();
	for(i=0; i < pixel_size; i++) {
		for(j=0; j < pixel_size; j++) {
			layers[layer].Pixel(Point(x+j,y+i), pixel_color);
		}
	}
}

void Canvas::updateTextureSizes() {
	const Rect &r = GetRect();
	for(BasicImage& t : layers) {
		t.Resize( r.w, r.h );
		t.Clear(0);
	}
}

void Canvas::Clear(int color, int layer) {
	layers[layer].Clear(color);
}

Canvas* Canvas::Clone() {
	Canvas* c = new Canvas();
	copyStyle(c);
	return c;
}

void Canvas::OnMouseMove( int mX, int mY, bool mouseState ) {
	
	if(!m_is_readonly && mouseState && CheckCollision(mX, mY)) {
		int x = mX - GetRect().x;
		x = clip(x, 0, GetRect().w-1);
		int y = mY - GetRect().y;
		y = clip(y, 0, GetRect().h-1);
		
		if(last_x < 0 || last_y < 0) {
			last_x = x;
			last_y = y;
		}

		
		layers.front().Line(Point(x, y), Point(last_x, last_y), pixel_color);
		
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

void Canvas::onRectChange() {
	updateTextureSizes();
}

}
