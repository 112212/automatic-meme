#include "Canvas.hpp"

namespace ng {
Canvas::Canvas() {
	setType( "canvas" );
	pixel_size = 1;
	m_pixel_color = 0xffffffff;

	last_pt = {-1,-1};
	m_is_readonly = false;
	m_align_to_grid = false;
	m_display_grid = false;
	m_grid_size_mode = false;
	m_should_update_texture_sizes = false;
	maketex = true;
	m_pixel_size = {1,1};
	m_grid_size = {1,1};
	m_style.background_color = 0;
	m_grid_color = 0xff808080;
	AddLayers(1);
	Clear(0,0);
}

Canvas::~Canvas() {
	
}

void Canvas::drawGrid(Point p) {
	Rect r = GetRect();
	
	float fw = (float)r.w / (float)m_grid_size.w;
	float fh = (float)r.h / (float)m_grid_size.h;
	
	// horizontal lines
	for(float y=0; y < r.h; y+=fh) {
		Drawing().Line( Point(p.x, p.y + y), Point(p.x + r.w, p.y + y), m_grid_color );
	}
	
	// vertical lines
	for(float x=0; x < r.w; x+=fw) {
		Drawing().Line( Point(p.x + x, p.y), Point(p.x + x, p.y + r.h), m_grid_color );
	}
}

void Canvas::Render( Point p, bool isSelected ) {
	const Rect& rect = GetRect();
	Point pt = rect + p;
	
	Control::Render(p, isSelected);
	
	// render layers in reverse
	for(auto rit = layers.rbegin(); rit != layers.rend(); rit++) {
		Drawing().TexRect( pt.x, pt.y, &(*rit) );
	}
	
	if(m_display_grid) {
		drawGrid(pt);
	}
}

void Canvas::RefreshTexture() {
	maketex = true;
}

void Canvas::SetGridNum(Size size) {
	m_grid_size_mode = true;
	Rect r = GetRect();
	m_pixel_size = { (float)r.w / size.w, (float)r.h / size.h };
	m_grid_size = size;
}

void Canvas::SetGridSize(Size size) {
	m_grid_size_mode = true;
	Rect r = GetRect();
	m_pixel_size = { (float)size.w, (float)size.h };
	m_grid_size = Size( r.w / size.w, r.h / size.h );
}

void Canvas::OnSetStyle(std::string& style, std::string& value) {
	STYLE_SWITCH {
		_case("grid"):
			m_display_grid = toBool(value);
			maketex=true;
		_case("pixel_size"):
			SetPixelSize( std::stoi(value) );
		_case("align_to_grid"):
			SetAlignToGrid( toBool(value) );
		_case("color"):
			if(value[0] == '#') {
				SetPixelColor(Color::ParseColor(value)); 
			}
		_case("grid_num"):
			{
				std::vector<std::string> wh;
				split_string(value, wh, ',');
				SetGridNum(Size(std::stoi(wh[0]), std::stoi(wh[1])));
			}
		_case("grid_size"):
			{
				std::vector<std::string> wh;
				split_string(value, wh, ',');
				SetGridSize(Size(std::stoi(wh[0]), std::stoi(wh[1])));
			}
		_case("grid_color"):
			m_grid_color = Color::ParseColor(value);
		_case("readonly"):
			SetReadOnly(toBool(value));
		_case("layers"):
			SetLayers(std::stoi(value));
	}
}

void Canvas::SetBackgroundColor(int color) {
	m_style.background_color = color;
}

void Canvas::SetPixelSize(int size) {
	m_pixel_size = {(float)size,(float)size};
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
		PutPixel(mX, mY);
	}
	m_is_mouseDown = true;
}

int Canvas::GetPixel(int x, int y, int layer) {
	if(layer >= layers.size()) {
		return 0;
	}

	Point p(x,y);
	if(m_align_to_grid) {
		alignToGrid(p);
	}
	
	auto &l = layers[layer];
	auto *img = l.GetImage();
	auto s = l.GetImageSize();
	return img[p.y * s.w + p.x];
}

int Canvas::GetGridPixel(int x, int y, int layer) {
	if(layer >= layers.size()) {
		return 0;
	}
	
	Point p(x*m_pixel_size.w,y*m_pixel_size.h);
	alignToGrid(p);
	
	auto &l = layers[layer];
	auto *img = l.GetImage();
	auto s = l.GetImageSize();
	return img[p.y * s.w + p.x];
}

void Canvas::alignToGrid(Point& p) {
	p.x = (int)(p.x / m_pixel_size.w) * m_pixel_size.w;
	p.y = (int)(p.y / m_pixel_size.h) * m_pixel_size.h;
}

void Canvas::PutPixel(int x, int y, int layer) {
	if(x < 0 or y < 0 or layer >= layers.size()) {
		return;
	}
	
	// Point p(x + m_pixel_size.w/2,y + m_pixel_size.h/2);
	Point p(x,y);
	if(m_align_to_grid) {
		alignToGrid(p);
	}
	
	auto &l = layers[layer];
	for(int y1=0; y1 < m_pixel_size.h; y1++) {
		for(int x1=0; x1 < m_pixel_size.w; x1++) {
			l.Pixel(Point(p.x+x1, p.y+y1), m_pixel_color);
		}
	}
	l.Refresh();
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
	const Rect &r = GetRect();
	if(!m_is_readonly && mouseState && CheckCollision(Point(mX,mY)+r)) {
		int c = 1;
		int x = clip(mX, c, r.w-c);
		int y = clip(mY, c, r.h-c);
		
		Point pt(x,y);
		
		if(last_pt.x < 0 || last_pt.y < 0) {
			last_pt = pt;
		}

		if(m_grid_size_mode) {
			PutPixel(pt.x, pt.y);
		} else {
			layers.front().Line(pt, last_pt, m_pixel_color);
		}
		last_pt = pt;
		
		emitEvent("change", {pt} );
	}
}

void Canvas::OnMouseUp( int mX, int mY, MouseButton btn ) {
	m_is_mouseDown = false;
	last_pt = {-1,-1};
}

void Canvas::OnLostFocus() {
	m_is_mouseDown = false;
}

void Canvas::onRectChange() {
	updateTextureSizes();
	if(m_grid_size_mode) {
		SetGridSize(m_grid_size);
	}
}

}
