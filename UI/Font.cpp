

#include "Font.hpp"
#include <iostream>
#include "utf8/utf8.h"

namespace ng {

Font::Font() {
	
}

GlyphMetrics Font::GetGlyphMetrics( uint32_t c ) {
	
}

void Font::BlitText( Image* img, int x, int y, std::string text, uint32_t color ) {
	
}

Image* Font::GetTextImage( std::string text, uint32_t color ) {
	
}

Font::~Font() {
	
}


int Font::GetMaxText( const std::string &text, int width ) {
	GlyphMetrics metric;
	int len = 0;
	int sum=0;
	auto it_end = text.end();
	for(auto it = text.begin(); it != it_end; ) {
		uint32_t glyph = utf8::next(it, it_end);
		metric = GetGlyphMetrics(glyph);
		if( sum > width ) {
			return len;
		}
		sum += metric.advance;
		len++;
	}
	return len;
}

int Font::GetMaxTextRep( char c, int width ) {
	GlyphMetrics metric;
	int sum=0;
	int i=0;
	metric = GetGlyphMetrics(c);
	do {
		sum += metric.advance;
		i++;
	} while( sum < width );
			
	return i+1;
}

int Font::GetTextSize( const std::string &text ) {
	int wx = 0;
	GlyphMetrics metric;
	const auto it_end = text.end();
	for(auto it = text.begin(); it != it_end; ) {
		uint32_t glyph = utf8::next(it, it_end);
		metric = GetGlyphMetrics(glyph);
		wx += metric.advance;
	}
	return wx;
}



int Font::GetMaxTextBw(const std::string &text, int width) {
	GlyphMetrics metric;
	int sum=0;
	auto it = text.end();
	const auto it_start = text.begin();
	int len = utf8::distance(it_start, it);
	
	while( it != it_start ) {
		uint32_t glyph = utf8::previous(it, it_start);
		metric = GetGlyphMetrics(glyph);
		if( sum > width ) {
			return len;
		}
		len--;
		sum += metric.advance;
	}
	return len;
}

int Font::GetHeight() {
	return GetGlyphMetrics('A').height;
}

}
