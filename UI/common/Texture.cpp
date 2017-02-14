#include "Texture.hpp"
#include <cstring>
#include <algorithm>
namespace ng {
	
Texture::Texture(int w, int h) {
	size = Size(w,h);
}

Texture::Texture(unsigned int* buffer, int w, int h) {
	size = Size(w,h);
	this->buffer = buffer;
}

// TODO: bresenham
void Texture::DrawLine(Point a, Point b, unsigned int color) {
	if(a.y == b.y && a.x == b.y) return;
	
	if(a.y > b.y) {
		Point tmp;
		tmp = a;
		a = b;
		b = tmp;
	}
	
	// horizontal line
	if(a.y == b.y) {
		int x1 = std::min(a.x, b.x);
		int x2 = std::max(a.x, b.x);
		int y = y*size.w;
		while(x1 < x2) {
			// plot(x1, a.y);
			buffer[y + x1] = color;
			x1++;
		}
		return;
	}
	
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

void Texture::DrawRect(Point a, Point b, unsigned int color, bool fill) {
	unsigned int *upper_line = buffer + (size.w * a.y) + a.x;
	unsigned int *lower_line = buffer + (size.w * b.y) + a.x;
	int c = b.x-a.x;
	for(int i=0; i < c; i++) {
		upper_line[i] = color;
		lower_line[i] = color;
	}
	// TODO: vert lines
}

void Texture::Clear(unsigned int color) {
	memset(buffer, 0, size.w * size.h);
}

void Texture::DrawCircle(Point a, int radius) {
	// TODO
}

void Texture::DrawImage(Point dstPoint, int* buffer, int w, int h, Point srcPoint, Size srcRegion) {
	// TODO
}

Size Texture::GetTextureSize() {
	return size;
}

unsigned int* Texture::GetTexture() {
	return buffer;
}

unsigned int Texture::GetTextureId() {
	return tex_id;
}

void Texture::SetTextureId(unsigned int texid) {
	tex_id = texid;
}

void Texture::GetUpdateRegion(Point& a, Point &b) {
	a = c1;
	b = c2;
	c1 = c2;
}
}
