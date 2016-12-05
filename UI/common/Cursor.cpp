#include "Cursor.hpp"

namespace ng {
SDL_Cursor* Cursor::defaultCursor = 0;
SDL_Cursor* Cursor::textCursor = 0;
SDL_Cursor* Cursor::zoomCursor = 0;


#include "Cursor_resources.hpp"
Cursor::Cursor() : m_relative_mode(false) {
}

void Cursor::InitCursors() {
	defaultCursor = SDL_GetCursor();
 
	textCursor = xpm_to_cursor( cursor, 25 );
	zoomCursor = xpm_to_cursor( zoom, 25 );
}
void Cursor::CleanupCursors() {
	SDL_FreeCursor( defaultCursor );
}


// -----------------

void Cursor::SetRelativeMode(bool relativemode) {
	m_relative_mode = relativemode;
}

void Cursor::SetCursor(int x, int y) {
	m_pointer.x = x;
	m_pointer.y = y;
}

void Cursor::SetSize(int w, int h) {
	m_pointer.w = w;
	m_pointer.h = h;
}

Point Cursor::GetCursor() {
	return m_pointer;
}

void Cursor::MoveCursor(int dx, int dy) {
	if(!m_relative_mode) {
		SetCursor(dx,dy);
	} else {
		m_pointer.x += dx;
		m_pointer.y += dy;
	}
}


// -----------------
SDL_Cursor* Cursor::xpm_to_cursor(const char **xpm, int len) {
	
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
			ypat = bcd_to_binary( xpm[0], xpat, i );
			
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
		ypat = bcd_to_binary( xpm[0], xpat, i );
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
			ypat = bcd_to_binary( xpm[j], xpat, i );
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
		ypat = bcd_to_binary( xpm[j], xpat, i );
		hot_y = ypat;
	}
	
	SDL_Cursor* curs = SDL_CreateCursor(data, mask, width, height, hot_x, hot_y);
	delete[] data;
	delete[] mask;
	
	return curs;
}

}
