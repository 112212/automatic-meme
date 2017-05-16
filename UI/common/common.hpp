#ifndef COMMON_HPP
#define COMMON_HPP

typedef unsigned int Uint32;

namespace ng {
	
	
struct Point {
	union {
		int x,min,w;
	};
	union {
		int y,max,h;
	};
	Point() {}
	Point(int _x, int _y) : x(_x),y(_y) {}
	bool operator< (const Point& b) const { return x < b.x || (x == b.x && y < b.y); }
	bool operator==(const Point& b) const { return b.x == x && b.y == y; }
	Point Offset(const Point& r) const { return Point(x+r.x, y+r.y); }
};

typedef Point Size;

struct Rect : Point {
	int w;
	int h;
	Rect() {}
	Rect(int _x, int _y, int _w, int _h) : Point(_x,_y),w(_w),h(_h) {}
};


constexpr unsigned int hash(const char *s, int off = 0) {
    return s[off] ? (hash(s, off+(s[off+1] == '_' ? 2 : 1))*33) ^ s[off] : 5381;
}

#ifdef USE_SFML
	char SFMLCodeToChar( unsigned char sf_code, bool shift );
#endif

#ifdef USE_SDL
	char SDLCodeToChar( int code, bool shift );
#endif

}	
#endif
