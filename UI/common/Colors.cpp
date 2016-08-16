#include "Colors.hpp"

namespace Colors {

	Uint32 GetColor(unsigned char r, unsigned char g, unsigned char b, unsigned char a) {
		return (((unsigned int)(r) << 16) | ((unsigned int)(g) << 8) | (unsigned int)(b) & 0xff) | ((unsigned int)a << 24);
	}
	
	Uint32 White = GetColor( 255,255,255 );
	Uint32 Gray = GetColor( 200,200,200 );
	Uint32 Dgray= GetColor( 64,64,64 );
	Uint32 Cyan = GetColor( 32,255,255 );
	Uint32 Red = GetColor( 255, 0, 0 );
	Uint32 Yellow = GetColor( 0xFD, 0xF5, 0x0B );
	Uint32 Blue = GetColor( 0x00, 0x00, 0xff );
	
	Uint32 ParseColor(std::string str) {
		Uint32 color = std::stoul(str.substr(1,8), 0, 16);
		if(str.size() < 9)
			color |= 0xff000000;
		return color;
	}
};
