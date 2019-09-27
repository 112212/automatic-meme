#ifndef NG_SDL_TTF_FONT_HPP
#define NG_SDL_TTF_FONT_HPP
#include <string>
#include <SDL_ttf.h>

#include "../../Font.hpp"
#include "../../managers/ResourceManager.hpp"
#include "../../Resource.hpp"
#include "../../managers/Fonts.hpp"

namespace ng {
class SDL_TTF_Font : public Font {
	private:
		TTF_Font* font;
	public:
		SDL_TTF_Font(File* file, int size);
		~SDL_TTF_Font();
		bool Loaded();
		virtual GlyphMetrics GetGlyphMetrics( uint32_t c );
		virtual void BlitText( Image* img, int x, int y, std::string text, uint32_t color );
		virtual Image* GetTextImage( std::string text, uint32_t color );
		static Resource* GetFont( File* file, Kvp kv );
	
};
}

#endif
