#include "BasicImage.hpp"
#include "Color.hpp"
#include <cstring>
namespace ng {
void BasicImage::Free() {
	if(buffer) {
		delete[] buffer;
		buffer = 0;
	}
	size.w = size.h = 0;
}

Size BasicImage::GetImageSize() {
	return size;
}

const unsigned int* BasicImage::GetImage() {
	return buffer;
}

BasicImage::BasicImage(int w, int h) : Image() {
	size.w = w;
	size.h = h;
	buffer = new unsigned int[w*h];
	int len = w*h;
	memset(buffer, 0, len*4);
	SetFullAffectedRegion();
}

BasicImage::BasicImage(BasicImage&& b) {
	buffer = b.buffer;
	b.buffer = 0;
	size = b.size;
}

BasicImage::~BasicImage() {
	if(buffer) {
		Free();
	}
}


void BasicImage::Clear(unsigned int color) {
	SetFullAffectedRegion();
	int l = size.w * size.h;
	for(int i=0; i < l; i++) {
		buffer[i] = color;
	}
}

/*
void TextureProcessor::CopyPictureToPictureWithScale( RGBA* slika1, int w1, int h1, RGBA* slika2, int w2, int h2 ) {
	float ratio_x = (float)(w1-1) / (float)w2;
	float ratio_y = (float)(h1-1) / (float)h2;
	
	//cout << "ratio_x: " << ratio_x << endl;
	//cout << "ratio_y: " << ratio_y << endl;
	
	int x,y,xint,yint;
	float dx, dy;
	float xi, yi;
	RGBA a,b,c,d;
	unsigned int px,px2;
	for(y=0; y < h2; y++) {
		for(x=0; x < w2; x++) {
			
			
			px = x+y*w2;
			
			// podaci za interpolaciju
			xi = ratio_x * x;
			yi = ratio_y * y;
			xint = (int)xi;
			yint = (int)yi;
			dx = xi - xint;
			dy = yi - yint;
			px2 = xint + yint*w1;
			a = slika1[ px2 ];
			b = slika1[ px2+1 ];
			c = slika1[ px2+w1 ];
			d = slika1[ px2 + w1 + 1 ];
			
			// interpolacije
			
			// Ca*(1-dx)*(1-dy) + Cb*dx*(1-dy) + Cc*(1-dx)*(dy) + Cd*(dx*dy)
			// sredjeno: (Ca*(1-dx)+Cb*dx)*(1-dy) + (Cc*(1-dx)+Cd*dx)*dy
			
			//slika2[ px ] = a;
			
			// crvena boja
			slika2[ px ].r = (a.r*(1-dx)+b.r*dx)*(1-dy) + (c.r*(1-dx)+d.r*dx)*dy;
			// zelena
			slika2[ px ].g = (a.g*(1-dx)+b.g*dx)*(1-dy) + (c.g*(1-dx)+d.g*dx)*dy;
			// plava
			slika2[ px ].b = (a.b*(1-dx)+b.b*dx)*(1-dy) + (c.b*(1-dx)+d.b*dx)*dy;
			// alpha
			slika2[ px ].a = (a.a*(1-dx)+b.a*dx)*(1-dy) + (c.a*(1-dx)+d.a*dx)*dy;
			
			
		}
	}
}
*/

static uint32_t col_mul(uint32_t col, float coef) {
	Color c(col);
	return Color(c.r * coef, c.g * coef, c.b * coef, 0xff).GetUint32();
}

void BasicImage::PutImage(Image* img, ng::Rect dstRegion, ng::Rect srcRegion, unsigned int background_key_color, unsigned int fg_color) {
	Size s = img->GetImageSize();
	const unsigned int* pixels = img->GetImage();
	
	// srcRegion size == dstRegion size
	// TODO: implement bilinear interpolation or nearest neighbour
	
	for(int y=0; y < dstRegion.h; y++) {
		
		int srcY = srcRegion.y + y;
		int dstY = dstRegion.y + y;
		float sY = srcRegion.y + (float)(y * srcRegion.h) / dstRegion.h;
		srcY = sY;
		float dy = sY - (float)srcY;
		
		for(int x=0; x < dstRegion.w; x++) {
			int dstX = dstRegion.x + x;
			int srcX = srcRegion.x + x;
			
			float sX = (float)srcRegion.x + x * (float)srcRegion.w / (float)dstRegion.w;
			srcX = sX;
			
			const uint32_t mask = 0xffffff;
			uint32_t i_pixel = pixels[srcY*s.w+srcX];
			uint32_t &o_pixel = buffer[dstY*size.w+dstX];
			if((i_pixel&mask) == background_key_color) {
				// o_pixel = 0;
			} else if(fg_color != 0) {
				float dx = sX - (float)srcX;
				
				float fp1 = 0;
				uint32_t p1 = pixels[srcY*s.w+srcX];
				if((p1&mask) != background_key_color) {
					fp1 = 1;
				}
				
				float fp2 = 0;
				uint32_t p2 = pixels[srcY*s.w+srcX+1];
				if((p2&mask) != background_key_color) {
					fp2 = 1;
				}
				
				float fp3 = 0;
				uint32_t p3 = pixels[(srcY+1)*s.w+srcX];
				if((p3&mask) != background_key_color) {
					fp3 = 1;
				}
				
				float fp4 = 0;
				uint32_t p4 = pixels[(srcY+1)*s.w+srcX+1];
				if((p4&mask) != background_key_color) {
					fp4 = 1;
				}
				
				float a = fp1 * (1-dx) + fp2 * (dx);
				float b = fp3 * (1-dx) + fp4 * (dx);
				float c = a * (1-dy) + b * dy;
				o_pixel = col_mul(fg_color, c);
				// o_pixel = fg_color;
				
			} else {
				o_pixel = i_pixel;
			}
		}
		
	}
	SetFullAffectedRegion();
}


void BasicImage::Line(Point a, Point b, unsigned int color) {
	if(a.y == b.y && a.x == b.y) return;
	if(a.y > b.y || (a.y == b.y && a.x > b.x)) {
		Point tmp;
		tmp = a;
		a = b;
		b = tmp;
	}
	SetFullAffectedRegion();
	// UpdateAffectedRegion(a);
	// UpdateAffectedRegion(b);
	
	if(a.x < 0 || b.x < 0 || a.x > size.w || b.x > size.w) return;
	if(a.y < 0 || b.y < 0 || a.y > size.y || b.y > size.h) return;
	
	
	if(a.y == b.y) { // horizontal line
		int x1 = a.x;
		int x2 = b.x;
		int y = a.y*size.w;
		while(x1 <= x2) {
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
			}
			buffer[ y*size.w + x ] = color;
			d -= dx;
		}
	}
}


void BasicImage::Resize(int w, int h) {
	if(size.w > 0 && size.h > 0) {
		delete[] buffer;
	}
	buffer = new unsigned int[w*h];
	size.w = w;
	size.h = h;
	FreeCache();
	SetFullAffectedRegion();
}

void BasicImage::Pixel(Point a, unsigned int color) {
	if(a.x < 0 || a.x >= size.w || a.y < 0 || a.y >= size.h) return;
	buffer[ a.y * size.w + a.x ] = color;
	// UpdateAffectedRegion(a);
	SetFullAffectedRegion();
}

void BasicImage::Rect(Point a, Point b, unsigned int color, bool fill) {
	a.x = std::min(a.x, b.x);
	a.y = std::min(a.y, b.y);
	b.x = std::max(a.x, b.x);
	b.y = std::max(a.y, b.y);
	UpdateAffectedRegion(a);
	UpdateAffectedRegion(b);
	
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

}
