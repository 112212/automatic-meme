#ifndef _H_FONTS_
#define _H_FONTS_

#ifdef USE_SFML
	#include <SFML/Graphics/Font.hpp>
#elif USE_SDL
	#include <utility> // std::pair
	#include <SDL2/SDL_ttf.h>
#endif

#include <string>
#include <map>

namespace ng {
class Fonts {
	private:
		// font short names
		#ifdef USE_SFML
		
			static std::map< std::string, sf::Font > fonts;
			static sf::Font nullfont;
			
		#elif USE_SDL
		
			static std::map< typename std::pair<std::string, int> , TTF_Font*> fonts;
			static bool ttfInited;
			
		#endif
		static std::map< std::string, std::string > fonts_path;
	public:
		#ifdef USE_SFML
		
			static const sf::Font& GetFont( std::string short_name );
			static sf::Font& LoadFont( std::string full_path_name, std::string short_name );
			static bool FontExists( std::string short_name );
			
		#elif USE_SDL
		
			static TTF_Font* GetFont( std::string short_name, int font_size );
			static TTF_Font* LoadFont( std::string full_path_name, std::string short_name, int font_size );
			static bool FontExists( std::string short_name, int font_size );
			static int getMaxText( TTF_Font* font, const std::string &text, int width );
			static int getMaxTextRep( TTF_Font* font, char c, int width );
			static int getMaxTextBw( TTF_Font* font, const std::string &text, int width );
			static int getTextSize( TTF_Font* font, const std::string &text );
			// static int get
		#endif
		
};
}
#endif
