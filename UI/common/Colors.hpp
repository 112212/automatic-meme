#ifndef CCOLORS_H
#define CCOLORS_H

#include "common.hpp"
#include <string>

#ifdef USE_SDL
#include <SDL2/SDL.h>
#endif

namespace Colors {
	
		extern Uint32 White;
		extern Uint32 Gray;
		extern Uint32 Dgray;
		extern Uint32 Cyan;
		extern Uint32 Red;
		extern Uint32 Yellow;
		extern Uint32 Blue;
		
		Uint32 GetColor(unsigned char r, unsigned char g, unsigned char b, unsigned char a = 0xff);
		Uint32 ParseColor(std::string str);
		SDL_Color toSDL_Color(uint32_t color);
};

#endif
