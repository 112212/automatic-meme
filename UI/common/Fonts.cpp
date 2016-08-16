#include "Fonts.hpp"
namespace ng {
std::map< std::string, std::string > Fonts::fonts_path;
#ifdef USE_SFML

	std::map< std::string, sf::Font > Fonts::fonts;
	sf::Font Fonts::nullfont;

	const sf::Font& Fonts::GetFont( std::string short_name ) {
		auto f = fonts.find( short_name );
		if( f == fonts.end() )
			return nullfont;
		else
			return f->second;
	}

	sf::Font& Fonts::LoadFont( std::string full_path_name, std::string short_name ) {
		auto f = fonts.find( short_name );
		if( f == fonts.end() ) {
			sf::Font fnt;
			if( !fnt.loadFromFile( full_path_name ) )
				return nullfont;
			else {
				fonts[ short_name ] = fnt;
				fonts_path[ short_name ] = full_path_name;
				return fonts[ short_name ];
			}
		}
		else
			return f->second;
	}
	
	bool Fonts::FontExists( std::string short_name ) {
		return fonts.find( short_name ) != fonts.end();
	}

#elif USE_SDL

	
	std::map< std::pair<std::string,int>, TTF_Font* > Fonts::fonts;
	bool Fonts::ttfInited = false;
	
	TTF_Font* Fonts::GetFont( std::string short_name, int font_size ) {
		auto key = make_pair(short_name, font_size);
		auto f = fonts.find( key );
		if( f == fonts.end() ) {
			if(fonts_path.find(short_name) != fonts_path.end()) {
				TTF_Font* fnt;
				if( !(fnt = TTF_OpenFont( fonts_path[ short_name ].c_str(), font_size )) )
					return 0;
				fonts[ key ] = fnt;
				return fnt;
			} else return 0;
		}
		else
			return f->second;
	}
	
	/*
		loads new font, or replaces existing font
		use GetFont to retrieve font
	*/
	TTF_Font* Fonts::LoadFont( std::string full_path_name, std::string short_name, int font_size ) {
		if(!ttfInited) {
			ttfInited = true;
			TTF_Init();
		}
		auto key = make_pair(short_name, font_size);
		auto f = fonts.find( key );
		if( f == fonts.end() ) {
			TTF_Font* fnt;
			if( !(fnt = TTF_OpenFont( full_path_name.c_str(), font_size )) )
				return 0;
			else {
				fonts[ key ] = fnt;
				fonts_path[ short_name ] = full_path_name;
				return fnt;
			}
		} else {
			RemoveFont(short_name);
			return LoadFont( full_path_name, short_name, font_size );
		}
	}
	
	void Fonts::RemoveFont( std::string fontname ) {
		bool found = false;
		int n = 0;
		TTF_Font* f = 0;
		std::pair<std::string, int> key;
		for(auto &p : fonts) {
			if(p.first.first == fontname) {
				n++;
				f = p.second;
				key = p.first;
			}
		}
		if(n == 1) {
			TTF_CloseFont( f );
			fonts.erase(key);
		}
	}
	
	TTF_Font* Fonts::GetParsedFont( std::string fontString ) {
		size_t sep = fontString.find(",");
		int font_size = 13;
		if(sep != fontString.npos) {
			font_size = std::stoi(fontString.substr(sep+1));
		}
		std::string font_name = fontString.substr(0, sep);
		return LoadFont( font_name, font_name, font_size );
	}
	
	int Fonts::getMaxText( TTF_Font* font, const std::string &text, int width ) {
		
		int advance;
		int len = text.length();
		int sum=0;
		for(int i=0; i < len; i++) {
			TTF_GlyphMetrics( font, text[i], 0, 0, 0, 0, &advance);
			if( sum > width ) {
				return i;
			}
			sum += advance;
		}
		return len;
	}
	
	int Fonts::getMaxTextRep( TTF_Font* font, char c, int width ) {
		
		int advance;
		int sum=0;
		int i=0;
		do {
			TTF_GlyphMetrics( font, c, 0, 0, 0, 0, &advance);
			sum += advance;
			i++;
		} while( sum < width );
				
		return i+1;
	}
	
	int Fonts::getTextSize( TTF_Font* font, const std::string &text ) {
		int wx = 0;
		int advance;
		for(auto it = text.begin(); it != text.end(); it++) {
			TTF_GlyphMetrics( font, *it, 0, 0, 0, 0, &advance);
			wx += advance;
		}
		return wx;
	}


	int Fonts::getMaxTextBw(TTF_Font* font, const std::string &text, int width) {
		
		int advance;
		int len = text.length();
		int sum=0;
		for(int i=len-1; i >= 0; i--) {
			TTF_GlyphMetrics( font, text[i], 0, 0, 0, 0, &advance);
			if( sum > width ) {
				return len-i;
			}
			sum += advance;
		}
		return len;
	}
	
	bool Fonts::FontExists( std::string short_name, int font_size ) {
		auto key = make_pair(short_name, font_size);
		return fonts.find( key ) != fonts.end();
	}
#endif
}	

