#include "Screen.hpp"
#include "../common.hpp"
#include <algorithm>
#include <iostream>
namespace ng {

Screen::Screen() {
	using_clip_region = false;
	current_alpha = 1.0;
}

void Screen::SetResolution( int w, int h ) {
	
}

void Screen::GetResolution( int &w, int &h ) {
	
}

void Screen::Init() {
	
}

void Screen::Rect(int x, int y, int w, int h, unsigned int color) {
	
}

void Screen::FillRect(int x, int y, int w, int h, unsigned int color) {
	
}

void Screen::FillCircle(int x, int y, float radius, unsigned int color) {
	
}

void Screen::Circle(int x, int y, float radius, unsigned int color) {
	
}

void Screen::Circle(Point p, float radius, uint32_t color) {
	Circle(p.x,p.y,radius,color);
}

void Screen::Line(Point a, Point b, int color) {
	Line(a.x,a.y,b.x,b.y,color);
}

void Screen::PushMaxAlpha(float alpha) {
	maxAlpha.push(current_alpha);
	SetMaxAlpha(alpha);
	current_alpha=alpha;
}
void Screen::PopMaxAlpha() {
	if(maxAlpha.empty()) {
		SetMaxAlpha(1.0);
	} else {
		current_alpha = maxAlpha.top();
		SetMaxAlpha(current_alpha);
		maxAlpha.pop();
	}
}

Point c1;
Point c2;
Size size;

void Screen::Line(int xA, int yA, int xB, int yB, unsigned int color) {
	Point a(xA,yA);
	Point b(xB,yB);
	
	if(a.y == b.y && a.x == b.y) return;
	if(a.y > b.y || (a.y == b.y && a.x > b.x)) {
		Point tmp;
		tmp = a;
		a = b;
		b = tmp;
	}
	
	// c1 = std::min<Point>(c1, a);
	// c2 = std::max<Point>(c2, b);
	
	if(a.x < 0 || b.x < 0 || a.x > size.w || b.x > size.w) return;
	if(a.y < 0 || b.y < 0 || a.y > size.y || b.y > size.h) return;
	
	
	if(a.y == b.y) { // horizontal line
		int x1 = a.x;
		int x2 = b.x;
		int y = a.y*size.w;
		while(x1 <= x2) {
			// buffer[y + x1] = color;
			PutPixel(x1, y, color);
			x1++;
		}
	} else if(a.x == b.x) { // vertical line
		int y1 = a.y;
		int y2 = b.y;
		while(y1 <= y2) {
			// buffer[y1*size.w + a.x] = color;
			PutPixel(a.x, y1, color);
			y1++;
		}
	} else {
		// bresenham line drawing algorithm
		int dx = b.x-a.x;
		int dy = b.y-a.y;
		int d = 0;
		int x = a.x;
		int v = 1;
		if(dx < 0) {
			dx = -dx;
			v = -1;
		}
		for(int y=a.y; y <= b.y; y++) {
			while(d <= 0) {
				d += dy;
				x += v;
				if(x > size.w) {
					return;
				}
				// buffer[ y*size.w + x ] = color;
				PutPixel(x, y, color);
			}
			// buffer[ y*size.w + x ] = color;
			PutPixel(x, y, color);
			d -= dx;
		}
	}
}

void Screen::HLine(int x1, int x2, int y1, unsigned int color) {
	while(x1 <= x2) {
		PutPixel(x1, y1, color);
		x1++;
	}
}

void Screen::VLine(int x1, int y1, int y2, unsigned int color) {
	while(y1 <= y2) {
		PutPixel(x1, y1, color);
		y1++;
	}
}

void Screen::SetMaxAlpha(float max_alpha) {
	
}

void Screen::SetRotation(float rotation, int cx, int cy) {
	
}

void Screen::SetRotationPoint(int x, int y) {
	
}

void Screen::PutPixel(int x, int y, unsigned int color)  {
	// TODO: MUST implement
}

void Screen::TexRect(int x, int y, int w, int h, Image* tex, bool repeat, int texWidth, int texHeight ) {
	
}
void Screen::TexRect(int x, int y, Image* tex, bool repeat ) {
	
}

void Screen::SetClipRegion(int x, int y, int w, int h, bool enable) {
	
}

bool Screen::GetClipRegion(int &x, int &y, int &w, int &h) {
	return false;
}


void Screen::EnableClipRegion() {
	if(using_clip_region) {
		SetClipRegion(-1,-1,-1,-1,true);
	}
}

void Screen::PushClipRegion(int x, int y, int w, int h) {
	ng::Rect r;
	if(GetClipRegion(r.x,r.y,r.w,r.h)) {
		clip_stack.push(r);
	}
	SetClipRegion(x,y,w,h);
	using_clip_region = true;
}

void Screen::PopClipRegion() {
	if(!clip_stack.empty()) {
		ng::Rect r = clip_stack.top();
		clip_stack.pop();
		SetClipRegion(r.x, r.y, r.w, r.h);
	} else {
		using_clip_region = false;
		DisableClipRegion();
	}
}

void Screen::DisableClipRegion() {
	SetClipRegion(0,0,0,0,false);
}

void Screen::DeleteTexture(unsigned int cache_id) {
	
}

uint32_t 	Screen::CompileShader(std::string vertexShader, std::string fragmentShader) {return 0;}
void 		Screen::SetShader(uint32_t shader_id) {}
bool 		Screen::IsShadersSupported() {return false;}

int	 Screen::SetNewOffScreenRender() {return 0;}
bool Screen::FreeOffScreenRender(int id) {return false;}
bool Screen::SetOffScreenRender(int id) {return false;}

void Screen::SetCacheId(ng::Image* img, uint32_t cache_id) {
	if(!img) return;
	// std::cout << "Screen::SetCacheId: " << img << " " << cache_id << "\n";
	if(img->screen != this) img->screen = this;
	img->SetCacheId(cache_id);
}

Image* Screen::GetOffScreenTexture(int id) {
	return 0;
}


void Screen::RemoveFromCache(uint32_t cache_id) {
	std::unique_lock<std::mutex> l(mtx);
	cache_remove.push_back(cache_id);
}

void Screen::ProcessQueue() {
	std::unique_lock<std::mutex> l(mtx);
	for(uint32_t c : cache_remove) {
		DeleteTexture(c);
	}
	cache_remove.clear();
}

}
