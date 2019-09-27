#include <iostream>
#include <SDL2/SDL_ttf.h>

#include "SDL_TTF_Font.hpp"
#include "SDL_Surface_Image.hpp"
#include "RWOpsFromFile.hpp"
namespace ng {


void SDL_TTF_Font::BlitText( Image* img, int x, int y, std::string text, uint32_t color ) {
	
}



Resource* SDL_TTF_Font::GetFont( File* file, Kvp kv ) {
	
	std::string fs = GetKv(kv, "fontsize");
	int font_size = 13;
	if(!fs.empty()) {
		font_size = std::stoi(fs);
	}
	// std::cout << "getting font: " << file << ", " << font_size << "\n";
	SDL_TTF_Font* font = new SDL_TTF_Font(file, font_size);
	if(font->Loaded()) {
		// std::cout << "font loaded correctly\n";
		return (Resource*)font;
	} else {
		return 0;
	}
}

bool SDL_TTF_Font::Loaded() {
	return font != 0;
}

SDL_Color toSDL_Color(uint32_t color) {
	SDL_Color c;
	c.r = (color >> 16) & 0xff;
	c.g = (color >> 8) & 0xff;
	c.b = color & 0xff;
	c.a = (color >> 24) & 0xff;
	return c;
}
	
Image* SDL_TTF_Font::GetTextImage( std::string text, uint32_t color ) {
	// std::cout << " getting font text " << text << "\n";
	if(text.empty()) {
		text = " ";
	}
	// SDL_Surface* surf = TTF_RenderText_Blended( font, text.c_str(), toSDL_Color(color) );
	SDL_Surface* surf = TTF_RenderUTF8_Blended( font, text.c_str(), toSDL_Color(color) );
	if(!surf) {
		// std::cout << "failed getting text image for text: \'" << text << "\'\n";
		return 0;
	}
	return new SDL_Surface_Image(surf);
}

SDL_TTF_Font::SDL_TTF_Font(File* file, int size) {
	static bool inited = false;
	if(!inited) {
		TTF_Init();
		inited = true;
	}
	// std::cout << " open font " << filename << ", " << size << "\n";
	font = TTF_OpenFontRW(GetRWOps(file), 1, size);
}

GlyphMetrics SDL_TTF_Font::GetGlyphMetrics( uint32_t c ) {
	GlyphMetrics g;
	g.height = 0;
	g.advance = 0;
	if(!font) {
		return g;
	}
	
	g.height = TTF_FontHeight(font);
	int minx; 
	int maxx;
	int miny;
	int maxy;
	int advance;
	
	TTF_GlyphMetrics(font, c, &minx, &maxx, &miny, &maxy, &advance);
	g.advance = advance;
	return g;
}

SDL_TTF_Font::~SDL_TTF_Font() {
	TTF_CloseFont(font);
}


}
