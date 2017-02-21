#include "Texture.hpp"
#include <cstring>
#include <algorithm>
#include <iostream>
namespace ng {
	
Texture::Texture() {
	size = Size(0,0);
	buffer = 0;
	tex_id = NO_TEXTURE;
	c1 = Point(size.w, size.h);
	c2 = Point(0,0);
}

Texture::Texture(int w, int h) {
	size = Size(w,h);
	buffer = new unsigned int[w*h];
	tex_id = NO_TEXTURE;
	c1 = Point(size.w, size.h);
	c2 = Point(0,0);
}

Texture::Texture(unsigned int* buffer, int w, int h) {
	size = Size(w,h);
	this->buffer = buffer;
	tex_id = NO_TEXTURE;
	c1 = Point(size.w, size.h);
	c2 = Point(0,0);
}

Texture::Texture(Texture&& tex) {
	this->buffer = tex.buffer;
	this->size.w = tex.size.w;
	this->size.h = tex.size.h;
	this->tex_id = tex.tex_id;
	tex.size.w = tex.size.h = 0;
	tex.buffer = 0;
	tex.tex_id = NO_TEXTURE;
	c1 = Point(size.w, size.h);
	c2 = Point(0,0);
}

Texture::~Texture() {
	if(buffer) delete[] buffer;
}

void Texture::Resize(int w, int h) {
	if(size.w > 0 && size.h > 0) {
		delete[] buffer;
	}
	buffer = new unsigned int[w*h];
	size.w = w;
	size.h = h;
	tex_id = NO_TEXTURE;
	c1 = Point(size.w, size.h);
	c2 = Point(0,0);
}

// TODO: bresenham
void Texture::Line(Point a, Point b, unsigned int color) {
	if(a.y == b.y && a.x == b.y) return;
	// return;
	if(a.y > b.y || (a.y == b.y && a.x > b.x)) {
		Point tmp;
		tmp = a;
		a = b;
		b = tmp;
	}
	
	c1 = std::min<Point>(c1, a);
	c2 = std::max<Point>(c2, b);
	
	if(a.x < 0 || b.x < 0 || a.x > size.w || b.x > size.w) return;
	if(a.y < 0 || b.y < 0 || a.y > size.y || b.y > size.h) return;
	
	
	if(a.y == b.y) { // horizontal line
		int x1 = a.x;
		int x2 = b.x;
		int y = a.y*size.w;
		while(x1 <= x2) {
			// plot(x1, a.y);
			buffer[y + x1] = color;
			x1++;
		}
	} else if(a.x == b.x) { // vertical line
		int y1 = a.y;
		int y2 = b.y;
		while(y1 <= y2) {
			buffer[y1*size.w + a.x] = color;
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
				buffer[ y*size.w + x ] = color;
				// plot(x,y);
			}
			// plot(x,y);
			buffer[ y*size.w + x ] = color;
			d -= dx;
		}
	}
}

void Texture::Pixel(Point a, unsigned int color) {
	if(a.x < 0 || a.x >= size.w || a.y < 0 || a.y >= size.h) return;
	buffer[ a.y * size.w + a.x ] = color;
	c1 = std::min<Point>(c1, a);
	c2 = std::max<Point>(c2, Point(a.x+1, a.y+1));
}

void Texture::Rect(Point a, Point b, unsigned int color, bool fill) {
	a.x = std::min(a.x, b.x);
	a.y = std::min(a.y, b.y);
	b.x = std::max(a.x, b.x);
	b.y = std::max(a.y, b.y);
	unsigned int *upper_line = buffer + (size.w * a.y) + a.x;
	unsigned int *lower_line = buffer + (size.w * b.y) + a.x;
	int c = b.x-a.x;
	for(int i=0; i < c; i++) {
		upper_line[i] = color;
		lower_line[i] = color;
	}
	for(int i=a.y; i <= b.y; i++) {
		buffer[(size.w * i) + a.x] = color;
		buffer[(size.w * i) + b.x] = color;
	}
	
}

void Texture::Clear(unsigned int color) {
	c1 = Point(0,0);
	c2 = Point(size.w, size.h);
	int l = size.w * size.h;
	for(int i=0; i < l; i++) {
		buffer[i] = color;
	}
}

void Texture::Circle(Point a, int radius) {
	// TODO
}

void Texture::Image(Point dstPoint, int* buffer, int w, int h, Point srcPoint, Size srcRegion) {
	// TODO
}

// void Texture::Text(std::string text_line, int x, int y) {
	
// }


Size& Texture::GetTextureSize() {
	return size;
}

const unsigned int* Texture::GetTexture() {
	return buffer;
}

unsigned int Texture::GetTextureId() {
	return tex_id;
}

void Texture::SetTextureId(unsigned int texid) {
	tex_id = texid;
}

bool Texture::GetUpdateRegion(Point& a, Point &b) {
	a = c1;
	b = c2;
	c1 = Point(size.w, size.h);
	c2 = Point(0,0);
	// return tex_id == NO_TEXTURE || (a.x < b.x && a.y < b.y);
	return true;
}
}
