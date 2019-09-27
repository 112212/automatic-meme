#ifndef NG_FONTS_HPP
#define NG_FONTS_HPP


#include <utility> // std::pair
#include <string>
#include <map>
#include <functional>

#include "../common.hpp"

namespace ng {
/*
struct File {
	std::string name;
	std::istream *stream;
};
*/
class Font;
// typedef std::function<Font*(File,int)> FontConstructor;
class Fonts {
	private:
		// font short names
		static std::map< typename std::pair<std::string, int> , Font*> fonts;
		static std::map< std::string, std::string > fonts_path;
		// static std::map< std::string, FontConstructor > registered_font_engines;
		
	public:
		static Font* GetFont( std::string short_name, int font_size );
		static void PutFont( std::string short_name, Font* font, int font_size );
		static Font* LoadFont( std::string full_path_name, std::string short_name, int font_size );
		static Font* GetParsedFont( std::string fontString );
		static bool FontExists( std::string short_name, int font_size );
		
		// static void RegisterFontExtension( std::string ext, FontConstructor font_constructor);
};


}
#endif



