#include "Drawi.hpp"
#include "../common.hpp" // bcd_to_binary
#include <sstream> // stristream
#include <cstri> // strtok
#include <iostream>

#ifdef USE_SFML
/*
	limitations: 
		char_width_per_color = 1
		color must be 6 hexadecimal digits (#ffffff), instead of short #fff, #000
*/
sf::Image loadImageFromXPM3( const char** xpm ) {
	// first line contains meta informations
	// width, height, num_colors, char_width_per_color
	
	// header
	std::stringstream ss;
	int params[6] = {0};
	ss.str( xpm[0] );
	for(int i=0; i < 6; i++)
		ss >> params[i];
	if(params[3] != 1) return sf::Image();
	sf::Image img;
	img.create( params[0],params[1] );	
	// palette
	unsigned int color_palette[256];
	for(int i=1; i <= params[2]; i++) {
		const char* p = std::strchr( xpm[i], '#' );
		const char &cc = xpm[i][0];
		if(p && p[0] == '#')
			color_palette[cc] = (hex_to_binary( p, 1, 6 ) << 8) + 255;
	}
	// image
	for(int i=0; i < params[1]; i++) {
		const char* p = xpm[1+params[2]+i];
		for( int j=0; j < params[0]; j++ )
			img.setPixel( j, i, sf::Color( color_palette[ p[j] ] ) );
	}
	return img;
}

#elif USE_SDL
#endif
