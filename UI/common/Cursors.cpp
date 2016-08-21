#include "Cursors.hpp"


// CCursors
SDL_Cursor* Cursors::defaultCursor = 0;
SDL_Cursor* Cursors::textCursor = 0;
SDL_Cursor* Cursors::zoomCursor = 0;

void Cursors::InitCursors() {
	defaultCursor = SDL_GetCursor();
 
	textCursor = xpm_to_cursor( cursor, 25 );
	zoomCursor = xpm_to_cursor( zoom, 25 );
}

void Cursors::CleanupCursors() {
	SDL_FreeCursor( defaultCursor );
}


/* XPM */
static const char* Cursors::cursor[] = {
  // width height num_colors chars_per_pixel
  "    32    20        3            1",
  // colors
  "X c #000000",
  ". c #ffffff",
  "  c None",
  // pixels
  ".......                         ",
  "   .                            ",
  "   .                            ",
  "   .                            ",
  "   .                            ",
  "   .                            ",
  "   .                            ",
  "   .                            ",
  "   .                            ",
  "   .                            ",
  "   .                            ",
  "   .                            ",
  "   .                            ",
  ".......                         ",
  "                                ",
  "                                ",
  "                                ",
  "                                ",
  "                                ",
  "                                ",
  "0,0"
};

/* XPM */
static const char* Cursors::zoom[] = {
  // width height num_colors chars_per_pixel
  "    32    20        3            1",
  // colors
  "X c #000000",
  ". c #ffffff",
  "  c None",
  // pixels
	"  .......                       ",
	" .........                      ",
	"....   ....                     ",
	"...     ...                     ",
	"..       ..                     ",
	"..       ...                    ",
	"..       ...                    ",
	"..       ...                    ",
	" ..     ...                     ",
	" ...   ....                     ",
	"  ..........                    ",
	"   ..........                   ",
	"    ...   ...                   ",
	"           ...                  ",
	"            ...                 ",
	"             ...                ",
	"              ..                ",
	"                                ",
	"                                ",
	"                                ",
	"0,0"
};

/*
SDL_Cursor* Cursors::xpm_to_cursor(const char **xpm, int len) {
	
	int xpat = -1;
	int ypat = -1;
	int num = 0;
	int width,height,num_col,cpx=-1,w,h;
	
	int hot_x, hot_y;
	int i,j;
	// read first header
	for(i=0; xpm[0][i]; i++) {
		if( xpm[0][i] != 0x20 ) {
			if( xpat==-1 )
				xpat = i;
		} else if( xpat != -1 ) {
			//printChars( (char*)xpm[0], xpat, i );
			ypat = basic_bcd_to_binary( xpm[0], xpat, i );
			
			cout << endl;
			switch(num++) {
				case 0:
					width = ypat;
					break;
				case 1:
					height = ypat;
					break;
				case 2:
					num_col = ypat;
					break;
				case 3:
					cpx = ypat;
					break;
			}
			xpat = -1;
		}
	}
	
	if(xpat != -1) {
		ypat = basic_bcd_to_binary( xpm[0], xpat, i );
		cpx = ypat;
	}
	
	// allocate
	Uint8 *data = new Uint8[ width*height/8+1 ];
	Uint8 *mask = new Uint8[ width*height/8+1 ];
	
	i=-1;
	for(h=0; h < height; h++) {
		for(j=0; j < width; j++) {
			if( j % 8 ) {
				mask[i] <<= 1;
				data[i] <<= 1;
			} else {
				i++;
				mask[i] = data[i] = 0;
			}
			switch(xpm[h+num_col+1][j]) {
				case 'X':
					data[i] |= 1;
					mask[i] |= 1;
					break;
				case '.':
					mask[i] |= 1;
					break;
				case ' ':
					break;
			}
		}
	}
	j = num_col + height + 1;
	xpat = -1;
	num = 0;
	for(i=0; xpm[ j ][i]; i++) {
		if( xpm[j][i] != ',' && xpm[j][i] != ' ' ) {
			if(xpat == -1)
				xpat = i;
		} else if(xpat != -1) {
			ypat = basic_bcd_to_binary( xpm[j], xpat, i );
			switch(num) {
				case 0:
					hot_x = ypat;
					break;
				case 1:
					hot_y = ypat;
					break;
			}
			xpat = -1;
		}
	}
	if( xpat != -1 ) {
		ypat = basic_bcd_to_binary( xpm[j], xpat, i );
		hot_y = ypat;
	}
	
	SDL_Cursor* curs = SDL_CreateCursor(data, mask, width, height, hot_x, hot_y);
	if(!curs) {
		cout << "nemas cursor ..." << endl;
	}
	delete[] data;
	delete[] mask;
	
	return curs;
}
*/
