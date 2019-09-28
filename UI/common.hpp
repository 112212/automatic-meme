#ifndef NG_COMMON_HPP
#define NG_COMMON_HPP

#include <string>
#include <iostream>
#include <vector>
typedef unsigned int Uint32;

namespace ng {

enum class CacheUpdateFlag {
	all = 0,
	position = 1,
	attributes = 2
};

template<typename T=int>
struct Point_t {
	union {
		T x,min,w;
	};
	union {
		T y,max,h;
	};
	Point_t() : x(0), y(0) {}
	Point_t(T xy) : x(xy), y(xy) {}
	Point_t(T x, T y) : x(x),y(y) {}
	friend Point_t<T> operator-(const Point_t<T>& a, const Point_t<T>& b) { return Point_t<T>(a.x-b.x, a.y-b.y); }
	Point_t& operator+=(const Point_t p) { x += p.x; y += p.y; return *this; }
	Point_t& operator-=(const Point_t p) { x -= p.x; y -= p.y; return *this; }
	friend Point_t<T> operator+(const Point_t<T>& a, const Point_t<T>& b) { return Point_t<T>(a.x+b.x, a.y+b.y); }
	bool operator< (const Point_t<T>& b) const { return x < b.x || (x == b.x && y < b.y); }
	bool operator==(const Point_t<T>& b) const { return b.x == x && b.y == y; }
	Point_t Offset(const Point_t<T>& r) const { return Point_t<T>(x+r.x, y+r.y); }
	friend std::ostream& operator<< (std::ostream& out, const Point_t<T>& v) {
		return out << "(" << v.x << ", " << v.y << ")";
	}
	operator std::string() const {
		return std::to_string(x) + ", " + std::to_string(y);
	}
};

void split_string(const std::string& str, std::vector<std::string>& values, char delimiter);
void find_and_replace(std::string& source, std::string const& find, std::string const& replace);

typedef Point_t<> Point;

template<typename T=int>
struct Size_t : Point_t<T> {
	Size_t() : Point_t<T>() {}
	Size_t(T w, T h) : Point_t<T>(w,h) {}
};

typedef Size_t<> Size;

template<typename T=int>
struct Rect_t : Point_t<T> {
	int w;
	int h;
	Rect_t() : w(0), h(0), Point_t<T>(0,0) {}
	Rect_t(T _x, T _y, T _w, T _h) : Point_t<T>(_x,_y),w(_w),h(_h) {}
	bool operator==(const Rect_t<T>& b) const { return b.x == Point_t<T>::x && b.y == Point_t<T>::y && b.w == w && b.h == h; }
	friend std::ostream& operator<< (std::ostream& out, const Rect_t<T>& v) {
		return out << "(" << v.x << ", " << v.y << ", " << v.w << ", " << v.h << ")";
	}
};


template<typename T=int>
struct Range_t : public Point_t<T> {
	Range_t(T min, T max) : Point_t<T>(min,max) { }
	Range_t() { }
};

typedef Rect_t<> Rect;
typedef Range_t<> Range;

Rect getIntersectingRectangle(const Rect &a, const Rect &b);
bool toBool(std::string str);
int toBoolOrInt(std::string str);

std::string getString(const char* format, ...);

template <typename T>
T clip(const T& n, const T& lower, const T& upper) {
  return std::max(lower, std::min(n, upper));
}

constexpr unsigned int hash(const char *s, int off = 0) {
    return s[off] ? (hash(s, off+(s[off+1] == '_' ? 2 : 1))*33) ^ s[off] : 5381;
}

}	
#endif
