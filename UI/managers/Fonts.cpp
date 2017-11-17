#include "Fonts.hpp"
#include <iostream>
namespace ng {
	std::map< std::string, std::string > Fonts::fonts_path;
	std::map< std::pair<std::string,int>, Font* > Fonts::fonts;
	std::map< std::string, FontConstructor > Fonts::registered_font_engines;
	
	void Fonts::PutFont( std::string short_name, Font* font, int font_size ) {
		fonts[std::make_pair(short_name, font_size)] = font;
	}
	
	Font* Fonts::GetFont( std::string short_name, int font_size ) {
		auto key = std::make_pair(short_name, font_size);
		// std::cout << "looking for font: " << key.first << ", " << key.second << "\n";
		auto f = fonts.find( key );
		
		if( f == fonts.end() ) {
			auto it = fonts_path.find(short_name);
			if(it != fonts_path.end()) {
				Font* fnt;
				
				fnt = LoadFont( it->second, it->first, font_size );
				
				
				fonts[ key ] = fnt;
				return fnt;
			} else {
				return 0;
			}
			
		} else {
			return f->second;
		}
	}
	
	Font* Fonts::LoadFont( std::string full_path_name, std::string short_name, int font_size ) {
		
		std::string::size_type ext_pos = full_path_name.rfind(".");
		if(ext_pos == std::string::npos) {
			return 0;
		}
		
		std::string extension = full_path_name.substr(ext_pos);
		
		Font* fnt = 0;
		/*
		if(full_path_name == short_name) {
			auto key = make_pair(short_name, font_size);
			auto it = fonts_path.find(short_name);
			if(it != fonts_path.end()) {
				fnt = GetFont(short_name, font_size);
			}
		}
		*/
		
		if(!fnt) {
			auto key = make_pair(short_name, font_size);
			auto f = fonts.find( key );
			if( f == fonts.end() ) {
				Font* fnt = 0;
				auto it_font_engine = registered_font_engines.find(extension);
				// std::cout << "Loading font " << full_path_name << ", ext \'" <<  extension  << "\' size " << font_size << "\n";
				if(it_font_engine != registered_font_engines.end()) {
					fnt = (it_font_engine->second)( {full_path_name, 0}, font_size );
					if(fnt) {
						// std::cout << "Loaded\n";
					}
				}
				
					
				if(!fnt) {
					return 0;
				} else {
					fonts[ key ] = fnt;
					fonts_path[ short_name ] = full_path_name;
					return fnt;
				}
			} else {
				return f->second;
			}
		} else {
			return fnt;
		}
	}
	
	Font* Fonts::GetParsedFont( std::string fontString ) {
		size_t sep = fontString.find(",");
		int font_size = 13;
		if(sep != fontString.npos) {
			font_size = std::stoi(fontString.substr(sep+1));
		}
		std::string font_name = fontString.substr(0, sep);
		// return LoadFont( font_name, font_name, font_size );
		
		return GetFont( font_name, font_size );
	}
	
	
	
	bool Fonts::FontExists( std::string short_name, int font_size ) {
		auto key = make_pair(short_name, font_size);
		return fonts.find( key ) != fonts.end();
	}
	
	void Fonts::RegisterFontExtension( std::string ext, FontConstructor font_constructor ) {
		registered_font_engines[ext] = font_constructor;
	}
}	

