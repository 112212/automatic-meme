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
		}
		else
			return f->second;
	}
	
	bool Fonts::FontExists( std::string short_name, int font_size ) {
		auto key = make_pair(short_name, font_size);
		return fonts.find( key ) != fonts.end();
	}
#endif
}	

