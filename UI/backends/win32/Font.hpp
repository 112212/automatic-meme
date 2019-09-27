#ifndef NG_WIN32_TTF_FONT_HPP
#define NG_WIN32_TTF_FONT_HPP

#include <string>

#include "../../managers/Fonts.hpp"
#include "../../managers/ResourceManager.hpp"
#include "../../Font.hpp"
#include "../../Resource.hpp"

namespace ng {
namespace win32Backend {

class Font : public ng::Font {
	private:
		std::string fontname;
		// void* priv;
	public:
		Font(File* file, int size);
		~Font();
		bool Loaded();
		virtual GlyphMetrics GetGlyphMetrics( uint32_t c );
		virtual void BlitText( ng::Image* img, int x, int y, std::string text, uint32_t color );
		virtual ng::Image* GetTextImage( std::string text, uint32_t color );
		static Resource* GetFont( File* file, Kvp kv );
};

}}


#endif
