#include <iostream>

#include "Font.hpp"
#include "FontImage.hpp"
#include "priv.hpp"

namespace ng {
namespace xlibBackend {


void Font::BlitText( Image* img, int x, int y, std::string text, uint32_t color ) {
	
}


Resource* Font::GetFont( File* file, Kvp kv ) {
	
	/*
	std::string fs = GetKv(kv, "fontsize");
	int font_size = 13;
	if(!fs.empty()) {
		font_size = std::stoi(fs);
	}
	// std::cout << "getting font: " << file << ", " << font_size << "\n";
	Font* font = new Font(file, font_size);
	if(font->Loaded()) {
		// std::cout << "font loaded correctly\n";
		return (Resource*)font;
	} else {
		return 0;
	}
	*/
	
	int cnt;
	char ** fonts = XListFonts(xlibBackend::display, "*", 10, &cnt);
	for(int i=0; i < cnt; i++) {
		std::cout << "fnt: " << fonts[i] << "\n";
	}
	
	Font* font = new Font(0, 0);
	return font;
	font->fontname = GetKv(kv, "font");
	std::cout << "loading font: " << font->fontname << "\n";
	// ::Font fnt = XLoadFont( xlibBackend::display, font->fontname.c_str() );
	FontPriv* priv = (FontPriv*)font->priv;
	// priv->font = fnt;
	// priv->font_struct = ::XQueryFont(xlibBackend::display, DefaultGC(xlibBackend::display, 0));
	priv->font_struct = ::XLoadQueryFont(xlibBackend::display, font->fontname.c_str());
	if(!priv->font_struct) {
		std::cout << "failed to get font\n";
		return 0;
	} else {
		std::cout << "font loaded\n";
	}
	return (Resource*)font;
}

bool Font::Loaded() {
	return true;
}

	
Image* Font::GetTextImage( std::string text, uint32_t color ) {
	// std::cout << " getting font text " << text << "\n";
	if(text.empty()) {
		text = " ";
	}
	
	// SDL_Surface* surf = TTF_RenderText_Blended( font, text.c_str(), toSDL_Color(color) );
	// if(!surf) {
		// std::cout << "failed getting text image for text: \'" << text << "\'\n";
		// return 0;
	// }
	// return new SDL_Surface_Image(surf);
	return new FontImage(text);
	return 0;
}

Font::Font(File* file, int size) {
	// std::cout << " open font " << filename << ", " << size << "\n";	
	priv=new FontPriv;
}

GlyphMetrics Font::GetGlyphMetrics( uint32_t c ) {
	GlyphMetrics g;
	g.height = 0;
	g.advance = 0;
	// if(!font) {
		// return g;
	// }
	
	FontPriv* priv = (FontPriv*)this->priv;
	// g.height = TTF_FontHeight(font);
	// fontname
	g.height = 10;
	
	//~ g.advance = ::XTextWidth(priv->font_struct, fontname.c_str(), fontname.size());
	g.advance=10;
	
	return g;
}

Font::~Font() {
	// XFreeFont();
}


}}
