#include "BasicImage.hpp"
#include "Color.hpp"
#include "Font.hpp"
#include <cstring>
#include <cmath>
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

const uint32_t* BasicImage::GetImage() {
	return buffer;
}

BasicImage::BasicImage(int w, int h) : Image() {
	size = {w,h};
	buffer = new uint32_t[w*h];
	memset(buffer, 0, w*h*4);
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
	Point a,b;
	if(!GetDirtyRegion(a,b)) return;
	/*
	int l = size.w * size.h;
	for(int i=0; i < l; i++) {
		buffer[i] = color;
	}
	*/
	if(a.x == -1 or b.x == -1) return;
	// std::cout << "clearing: " << a << b << "\n";
	for(int y=a.y; y < b.y; y++) {
		for(int x=a.x; x < b.x; x++) {
			buffer[y*size.w+x] = color;
		}
	}
	SetFullAffectedRegion();
	ResetDirtyRegion();
}

/*
void TextureProcessor::CopyPictureToPictureWithScale( RGBA* picture1, int w1, int h1, RGBA* picture2, int w2, int h2 ) {
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
			
			// interpolation variables
			xi = ratio_x * x;
			yi = ratio_y * y;
			xint = (int)xi;
			yint = (int)yi;
			dx = xi - xint;
			dy = yi - yint;
			px2 = xint + yint*w1;
			a = picture1[ px2 ];
			b = picture1[ px2+1 ];
			c = picture1[ px2+w1 ];
			d = picture1[ px2 + w1 + 1 ];
			
			// interpolations
			
			// Ca*(1-dx)*(1-dy) + Cb*dx*(1-dy) + Cc*(1-dx)*(dy) + Cd*(dx*dy)
			// (Ca*(1-dx)+Cb*dx)*(1-dy) + (Cc*(1-dx)+Cd*dx)*dy
			
			//picture2[ px ] = a;
			
			// red
			picture2[ px ].r = (a.r*(1-dx)+b.r*dx)*(1-dy) + (c.r*(1-dx)+d.r*dx)*dy;
			// green
			picture2[ px ].g = (a.g*(1-dx)+b.g*dx)*(1-dy) + (c.g*(1-dx)+d.g*dx)*dy;
			// blue
			picture2[ px ].b = (a.b*(1-dx)+b.b*dx)*(1-dy) + (c.b*(1-dx)+d.b*dx)*dy;
			// alpha
			picture2[ px ].a = (a.a*(1-dx)+b.a*dx)*(1-dy) + (c.a*(1-dx)+d.a*dx)*dy;
		}
	}
}
*/

static uint32_t col_mul(uint32_t col, float coef) {
	Color c(col);
	return Color(c.r * coef, c.g * coef, c.b * coef, 0xff).GetUint32();
}

void BasicImage::PutImage(Image* img, ng::Rect dstRegion, ng::Rect srcRegion, uint32_t background_key_color, uint32_t bg_mask_check, unsigned int fg_color) {
	Size s = img->GetImageSize();
	const unsigned int* pixels = img->GetImage();
	// return;
	// srcRegion size == dstRegion size
	// TODO: implement bilinear interpolation or nearest neighbour
	int srcX, srcY, dstX, dstY;
	if(dstRegion.w > size.w || dstRegion.x < 0) return;
	if(dstRegion.y > size.h || dstRegion.y < 0) return;
	
	for(int y=0; y < dstRegion.h; y++) {
		
		dstY = dstRegion.y + y;
		float sY = (float)srcRegion.y + (float)(y * (float)srcRegion.h / (float)dstRegion.h);
		srcY = sY;
		float dy = sY - (float)srcY;
		// const uint32_t mask = 0xffffffff;
		
		for(int x=0; x < dstRegion.w; x++) {
			dstX = dstRegion.x + x;
			
			float sX = (float)srcRegion.x + (float)(x * (float)srcRegion.w / (float)dstRegion.w);
			srcX = sX;
			
			uint32_t i_pixel = pixels[srcY*s.w+srcX];
			uint32_t &o_pixel = buffer[dstY*size.w+dstX];
			// printf("%08X ", i_pixel);
			if((i_pixel&bg_mask_check) == background_key_color) {
				o_pixel = 0;
			} else if(fg_color != 0) {
				
				float dx = sX - (float)srcX;
				
				float fp1 = 0;
				uint32_t p1 = pixels[srcY*s.w+srcX];
				if((p1&bg_mask_check) != background_key_color) {
					fp1 = 1;
				}
				
				float fp2 = 0;
				uint32_t p2 = pixels[srcY*s.w+srcX+1];
				if((p2&bg_mask_check) != background_key_color) {
					fp2 = 1;
				}
				
				float fp3 = 0;
				uint32_t p3 = pixels[(srcY+1)*s.w+srcX];
				if((p3&bg_mask_check) != background_key_color) {
					fp3 = 1;
				}
				
				float fp4 = 0;
				uint32_t p4 = pixels[(srcY+1)*s.w+srcX+1];
				if((p4&bg_mask_check) != background_key_color) {
					fp4 = 1;
				}
				
				float count = fp1+fp2+fp3+fp4;
				float a = fp1 * (1.0f-dx) + fp2 * (dx);
				float b = fp3 * (1.0f-dx) + fp4 * (dx);
				float c = a * (1.0f-dy) + b * dy;
				o_pixel = col_mul(fg_color, std::min(1.0f, count/4.0f));
				
				// o_pixel = fg_color;
			} else {
				o_pixel = i_pixel;
			}
		}
	}
	SetFullAffectedRegion();
}

bool BasicImage::clipLine(Point &a, Point &b) {
	if((a.y > b.y) || (a.y == b.y && a.x > b.x)) {
		Point tmp;
		tmp = a;
		a = b;
		b = tmp;
	}
	
	int wmax = size.w - 1;
	int hmax = size.h - 1;
	if(a.x < 0 || b.x < 0 || a.x >= size.w || b.x >= size.w) {
		if((a.x < 0 && b.x < 0) || (a.x >= size.w && b.x >= size.w)) {
			return false;
		}
		
		if(a.x < 0) {
			a.y = a.y + (-a.x) * (double)(b.y - a.y) / (b.x - a.x);
			a.x = 0;
		} else if(a.x >= size.w) {
			a.y = a.y - (a.x-wmax) * (double)(a.y - b.y) / (a.x - b.x);
			a.x = wmax;
		}
		
		if(b.x < 0) {
			b.y = b.y + (-b.x) * (double)(a.y - b.y) / (a.x - b.x);
			b.x = 0;
		} else if(b.x >= size.w) {
			b.y = b.y - (b.x-wmax) * (double)(b.y - a.y) / (b.x - a.x);
			b.x = wmax;
		}
	}
	
	if(a.y < 0 || b.y < 0 || a.y >= size.y || b.y >= size.h) {
		if((a.y < 0 && b.y < 0) || (a.y >= size.h && b.y >= size.h)) {
			return false;
		}
		if(a.y < 0) {
			a.x = a.x + (-a.y) * (double)(b.x - a.x) / (b.y - a.y);
			a.y = 0;
		}
		
		if(b.y >= size.h) {
			b.x = b.x - (b.y-hmax) * (double)(a.x - b.x) / (a.y - b.y);
			b.y = hmax;
		}
	}
	return true;
}

void BasicImage::Line(Point a, Point b, unsigned int color) {
	if(a == b) {
		return;
	}
	
	// clip and return if whole line lies outside of frame
	if(!clipLine(a,b)) {
		return;
	}
	
	// UpdateAffectedRegion(a);
	// UpdateAffectedRegion(b);
	
	SetFullAffectedRegion();
	
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
			while(d > 0) {
				d -= dy;
				x += v;
				buffer[ y*size.w + x ] = color;
			}
			buffer[ y*size.w + x ] = color;
			d += dx;
		}
	}
}

void BasicImage::LineAA(Point a, Point b, uint32_t color) {
	/*
	function plot(x, y, c) is
    plot the pixel at (x, y) with brightness c (where 0 ≤ c ≤ 1)

	// integer part of x
	function ipart(x) is
		return floor(x)

	function round(x) is
		return ipart(x + 0.5)

	// fractional part of x
	function fpart(x) is
		return x - floor(x)

	function rfpart(x) is
		return 1 - fpart(x)

	*/
	
	if(!clipLine(a,b)) {
		return;
	}
	
	
	// std::cout << "draw line: " << a << b << "\n";
	
	int x0 = a.x, y0 = a.y,
		x1 = b.x, y1 = b.y;
	auto swap = [](int &a, int &b) {
		int t;
		t=a; a=b; b=t;
	};
	bool steep = abs(y1-y0) > abs(x1-x0);
	
	if(steep) {
		swap(x0,y0);
		swap(x1,y1);
	}
	
	if(x0 > x1) {
		swap(x0,x1);
		swap(y0,y1);
	}
	
	double dx = x1 - x0;
	double dy = y1 - y0;
	
	double gradient = dy / dx;
	
	if(dx == 0.0) {
		gradient = 1.0;
	}
	
	int xend = x0;
	int yend = y0 + gradient * (xend - x0);
	double intery = yend + gradient; // first y-intersection for the main loop
	int xpxl1 = x0;
	
	/*
	xend := round(x1)
		yend := y1 + gradient * (xend - x1)
		xgap := fpart(x1 + 0.5)
		xpxl2 := xend //this will be used in the main loop
		ypxl2 := ipart(yend)
	*/
	
	int xpxl2 = x1;
	Color c(color);
	if(steep) {
		int x;
		for(x=xpxl1+1; x < xpxl2; x++) {
			// plot(ipart(intery)  , x, rfpart(intery))
			// plot(ipart(intery)+1, x,  fpart(intery))
			// buffer[ y*size.w + x ] = color;
			// buffer[ y*size.w + x ] = color;
			
			double f = floor(intery);
			int f1 = (int)f;
			double a = intery - f;
			// Pixel({x, f}, (1-f)*0xffffffff);
			// Pixel({x, f+1}, (f)*0xffffffff);
			c.a = (uint8_t)((1.0-a)*0xff);
			Pixel({f1, x}, c.GetUint32());
			c.a = (uint8_t)((a)*0xff);
			Pixel({f1+1, x}, c.GetUint32());
			
			intery += gradient;
	   }
	} else {
		int x;
		for(x=xpxl1+1; x < xpxl2; x++) {
			// plot(x, ipart(intery),  rfpart(intery))
			// plot(x, ipart(intery)+1, fpart(intery))
			double f = floor(intery);
			int f1 = (int)f;
			double a = intery - f;
			// Pixel({x, f}, (1-f)*0xffffffff);
			// Pixel({x, f+1}, (f)*0xffffffff);
			c.a = (uint8_t)((1.0-a)*0xff);
			Pixel({x, f1-1}, c.GetUint32());
			c.a = (uint8_t)((a)*0xff);
			Pixel({x, f1}, c.GetUint32());
			
			intery += gradient;
		}
	}
	

	/*
	function drawLine(x0,y0,x1,y1) is
		boolean steep := abs(y1 - y0) > abs(x1 - x0)
		
		if steep then
			swap(x0, y0)
			swap(x1, y1)
		end if
		if x0 > x1 then
			swap(x0, x1)
			swap(y0, y1)
		end if
		
		dx := x1 - x0
		dy := y1 - y0
		gradient := dy / dx
		if dx == 0.0 then
			gradient := 1.0
		end if

		// handle first endpoint
		xend := round(x0)
		yend := y0 + gradient * (xend - x0)
		xgap := rfpart(x0 + 0.5)
		xpxl1 := xend // this will be used in the main loop
		ypxl1 := ipart(yend)
		if steep then
			plot(ypxl1,   xpxl1, rfpart(yend) * xgap)
			plot(ypxl1+1, xpxl1,  fpart(yend) * xgap)
		else
			plot(xpxl1, ypxl1  , rfpart(yend) * xgap)
			plot(xpxl1, ypxl1+1,  fpart(yend) * xgap)
		end if
		intery := yend + gradient // first y-intersection for the main loop
		
		// handle second endpoint
		xend := round(x1)
		yend := y1 + gradient * (xend - x1)
		xgap := fpart(x1 + 0.5)
		xpxl2 := xend //this will be used in the main loop
		ypxl2 := ipart(yend)
		if steep then
			plot(ypxl2  , xpxl2, rfpart(yend) * xgap)
			plot(ypxl2+1, xpxl2,  fpart(yend) * xgap)
		else
			plot(xpxl2, ypxl2,  rfpart(yend) * xgap)
			plot(xpxl2, ypxl2+1, fpart(yend) * xgap)
		end if
		
		// main loop
		if steep then
			for x from xpxl1 + 1 to xpxl2 - 1 do
			   begin
					plot(ipart(intery)  , x, rfpart(intery))
					plot(ipart(intery)+1, x,  fpart(intery))
					intery := intery + gradient
			   end
		else
			for x from xpxl1 + 1 to xpxl2 - 1 do
			   begin
					plot(x, ipart(intery),  rfpart(intery))
					plot(x, ipart(intery)+1, fpart(intery))
					intery := intery + gradient
			   end
		end if
	end function
	*/
	// UpdateAffectedRegion(a);
	// UpdateAffectedRegion(b);
	SetFullAffectedRegion();
}

void BasicImage::Resize(int w, int h) {
	if(size.w > 0 && size.h > 0 && buffer) {
		delete[] buffer;
	}
	buffer = new unsigned int[w*h];
	size = {w,h};
	FreeCache();
	SetFullAffectedRegion();
}

void BasicImage::Pixel(Point a, unsigned int color) {
	if((a.x < 0) || (a.x >= size.w) || (a.y < 0) || (a.y >= size.h)) return;
	// buffer[ a.y * size.w + a.x ] = color;
	int idx=a.y * size.w + a.x;
	if((idx > size.w*size.h) || (idx < 0)) return;
	buffer[ idx ] = color;
	// UpdateAffectedRegion(a);
	// UpdateAffectedRegion({a.x+1,a.y+1});
	// SetFullAffectedRegion();
	
}

void BasicImage::Refresh() {
	SetFullAffectedRegion();
}

void BasicImage::DrawRect(Point a, Point b, unsigned int color, bool fill) {
	// upper left corner
	if((a.x < 0 && b.x < 0) || (a.x >= size.w && b.x >= size.w) ||
	   (a.y < 0 && b.y < 0) || (a.y >= size.h && b.y >= size.h))
	{
		return;
	}
	
	if(a.x == b.x) {
		b.x+=2;
	}
	if(a.y == b.y) {
		b.y+=2;
	}
	
	a = {std::max(0, std::min(a.x, b.x)), std::max(0, std::min(a.y, b.y))};
	b = {std::min(size.w-1, std::max(a.x, b.x)), std::min(size.y-1, std::max(a.y, b.y))};
	
	UpdateAffectedRegion(a);
	UpdateAffectedRegion(b);
	
	
	if(fill) {
		for(int y=a.y; y < b.y; y++) {
			unsigned int *line = buffer + (size.w * y);
			for(int x=a.x; x < b.x; x++) {
				line[x] = color;
			}
		}
	} else {
		unsigned int *upper_line = buffer + (size.w * a.y) + a.x;
		unsigned int *lower_line = buffer + (size.w * b.y) + a.x;
		int c = b.x - a.x;
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

void BasicImage::Text(std::string str, Point p, Font *font, uint32_t color) {
	Image* img = font->GetTextImage(str, color);
	Size s = img->GetImageSize();
	PutImage(img, {p.x, p.y, s.w, s.h}, {0,0,s.w,s.h}, 0x00000000, 0xff000000 );
}




static int compare_int(const void *a, const void *b)
{
    return (*(const int *)a) - (*(const int *)b);
}

void BasicImage::DrawPoly(std::vector<Point> points, uint32_t color) {
    int i;
    int y;
    int miny, maxy;
    int x1, y1;
    int x2, y2;
    int ind1, ind2;
    int ints;
    int n = points.size();
    int *polyints = new int[n];
    if (polyints == nullptr) {
        return;
    }

	
    /* Determine Y maxima */
    // miny = vy[0];
    // maxy = vy[0];
    miny = points[0].y;
    maxy = points[0].y;
    for (i=1; (i < n); i++)
    {
        miny = std::min(miny, points[i].y);
        maxy = std::max(maxy, points[i].y);
    }

    /* Draw, scanning y */
    for(y=miny; (y <= maxy); y++) {
        ints = 0;
        for (i=0; (i < n); i++) {
            if (!i) {
                ind1 = n-1;
                ind2 = 0;
            } else {
                ind1 = i-1;
                ind2 = i;
            }
            y1 = points[ind1].y;
            y2 = points[ind2].y;
            if (y1 < y2) {
                x1 = points[ind1].x;
                x2 = points[ind2].x;
            } else if (y1 > y2) {
                y2 = points[ind1].y;
                y1 = points[ind2].y;
                x2 = points[ind1].x;
                x1 = points[ind2].x;
            } else {
                continue;
            }
            if ((y >= y1) && (y < y2)) {
                polyints[ints++] = (y-y1) * (x2-x1) / (y2-y1) + x1;
            } else if ((y == maxy) && (y > y1) && (y <= y2)) {
                polyints[ints++] = (y-y1) * (x2-x1) / (y2-y1) + x1;
            }
        }
        qsort(polyints, ints, sizeof(int), compare_int);

        for (i=0; (i<ints); i+=2) {
            // drawhorzlineclip(dst, color, polyints[i], y, polyints[i+1]);
            Point pstart(polyints[i], y);
            Point pend(polyints[i+1], y);
            // std::cout << "drw: " << pstart << pend << "\n";
            Line(pstart, pend, color);
        }
    }
    // PyMem_Free(polyints);
    delete[] polyints;
}



}


