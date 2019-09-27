#include <iostream>
#include <cstdarg>
#include "common.hpp"
namespace ng {
	
void printChars(char* str, int start, int end) {
	for(int i=start; i < end-start; i++) {
		std::cout << str[i];
	}
}

void split_string(const std::string& str, std::vector<std::string>& values, char delimiter) {
	size_t prevpos = 0;
	size_t pos = str.find(delimiter, prevpos);
	while(pos != str.npos) {
		int len = pos-prevpos;
		if(len != 0) {
			values.push_back( str.substr(prevpos, len) );
		}
		prevpos = pos + 1;
		pos = str.find(delimiter, prevpos);
	}
	if(prevpos < str.size()) {
		values.push_back( str.substr(prevpos) );
	}
}

void find_and_replace(std::string& source, std::string const& find, std::string const& replace) {
    for(std::string::size_type i = 0; (i = source.find(find, i)) != std::string::npos; i += replace.length()) {
        source.replace(i, find.length(), replace);
    }
}
bool toBool(std::string str) {
	return str == "true" || str == "1" || str=="t";
}

Rect getIntersectingRectangle(const Rect &a, const Rect &b) {
	int Ax = std::max(a.x, b.x);
	int Ay = std::max(a.y, b.y);
	int Bx = std::min(a.x+a.w,b.x+b.w);
	int By = std::min(a.y+a.h,b.y+b.h);
	if(Ax < Bx && Ay < By) {
		return {Ax,Ay,Bx-Ax,By-Ay};
	}
	return {0,0,0,0};
}

std::string getString(const char* format, ...) {
	char buffer[200];
	va_list list;
	va_start(list, format);
	vsprintf(buffer, format, list);
	return std::string(buffer);
}



}
