#include "Colors.hpp"
// CColors
namespace Colors {
	Uint32 White;
	Uint32 Gray;
	Uint32 Dgray;
	Uint32 Cyan;
	Uint32 Red;
	Uint32 Yellow;
	Uint32 Blue;

	Uint32 GetColor(unsigned char r, unsigned char g, unsigned char b) {
		return (((unsigned int)(r) << 24) | ((unsigned int)(g) << 16) | (unsigned int)(b) << 8) | 0x000000ff;
	}
	
	bool is_inited = false;
	
	void InitColors() {
		if(!is_inited) {
			White = GetColor( 255,255,255);
			Gray = GetColor( 200,200,200 );
			Dgray= GetColor( 64,64,64);
			Cyan = GetColor( 32,255,255);
			Red = GetColor( 255, 0, 0);
			Yellow = GetColor( 0xFD, 0xF5, 0x0B);
			Blue = GetColor(  0x00, 0x00, 0xff );
		}
	}
		
};
