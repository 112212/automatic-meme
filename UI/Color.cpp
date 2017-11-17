#include "Color.hpp"

namespace ng {

uint32_t Color::GetColor(unsigned char r, unsigned char g, unsigned char b, unsigned char a) {
	return (((unsigned int)(r) << 16) | ((unsigned int)(g) << 8) | (unsigned int)(b) & 0xff) | ((unsigned int)a << 24);
}

const uint32_t Color::White =  Color::GetColor( 255,255,255 );
const uint32_t Color::Gray =   Color::GetColor( 200,200,200 );
const uint32_t Color::Dgray =  Color::GetColor( 64,64,64 );
const uint32_t Color::Cyan =   Color::GetColor( 32,255,255 );
const uint32_t Color::Green =   Color::GetColor( 32,255,32 );
const uint32_t Color::Red  =   Color::GetColor( 255, 0, 0 );
const uint32_t Color::Yellow = Color::GetColor( 0xFD, 0xF5, 0x0B );
const uint32_t Color::Blue =   Color::GetColor( 0x00, 0x00, 0xff );

uint32_t Color::ParseColor(std::string str) {
	if(str.empty()) { return 0; }
	if(str[0] != '#') {
		if(str == "white") {
			return White;
		} else if(str == "blue") {
			return Blue;
		} else if(str == "cyan") {
			return Cyan;
		} else if(str == "red") {
			return Red;
		} else if(str == "green") {
			return Green;
		} else if(str == "gray") {
			return Gray;
		} else if(str == "Dgray") {
			return Dgray;
		}
		return 0;
	}
	
	if(str.size() == 4) {
		str = std::string("#") + str[1] + "0" + str[2] + "0" + str[3] + "0";
	}
	
	uint32_t color = std::stoul(str.substr(1,8), 0, 16);
	if(str.size() < 9) {
		color |= 0xff000000;
	}
	return color;
}

Color::Color() {
}

Color::Color(uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
	this->r = r;
	this->g = g;
	this->b = b;
	this->a = a;
}

uint32_t Color::GetUint32() {
	return (((unsigned int)(r) << 16) | ((unsigned int)(g) << 8) | (unsigned int)(b) & 0xff) | ((unsigned int)a << 24);
}

Color::Color(uint32_t argb) {
	a = (argb >> 24) & 0xff;
	r = (argb >> 16) & 0xff;
	g = (argb >> 8) & 0xff;
	b = (argb) & 0xff;
}

Color::Color(std::string str) {
	*this = Color(ParseColor(str));
}

}
