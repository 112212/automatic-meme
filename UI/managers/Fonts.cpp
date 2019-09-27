#include "Fonts.hpp"
#include <iostream>
#include "ResourceManager.hpp"
namespace ng {
	std::map< std::string, std::string > Fonts::fonts_path;
	std::map< std::pair<std::string,int>, Font* > Fonts::fonts;
	
	void Fonts::PutFont( std::string name, Font* font, int font_size ) {
		ResourceManager::PutResource(name, (Resource*)font);
	}
	
	Font* Fonts::GetFont( std::string name, int font_size ) {
		return (Font*)ResourceManager::GetResource(name);
	}
	
	Font* Fonts::LoadFont( std::string filename, std::string name, int font_size ) {
		return (Font*)ResourceManager::LoadResource(filename, name, {{"fontsize", std::to_string(font_size)}});
	}
	
	Font* Fonts::GetParsedFont( std::string fontString ) {
		// std::cout << "getting font: " << fontString << "\n";
		size_t sep = fontString.find(",");
		int font_size = 13;
		if(sep != fontString.npos) {
			font_size = std::stoi(fontString.substr(sep+1));
		}
		std::string font_name = fontString.substr(0, sep);
		// return LoadFont( font_name, font_name, font_size );
		
		return GetFont( font_name, font_size );
	}
	
	
	
	bool Fonts::FontExists( std::string name, int font_size ) {
		return ResourceManager::GetResource(name);
	}
	
}	

