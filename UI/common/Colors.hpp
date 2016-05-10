#ifndef CCOLORS_H
#define CCOLORS_H

#include "common.hpp"
#include <string>

namespace Colors {
	
		extern Uint32 White;
		extern Uint32 Gray;
		extern Uint32 Dgray;
		extern Uint32 Cyan;
		extern Uint32 Red;
		extern Uint32 Yellow;
		extern Uint32 Blue;
		
		// init
		void InitColors( );
		Uint32 GetColor(unsigned char r, unsigned char g, unsigned char b);
		Uint32 ParseColor(std::string str);
};

#endif
