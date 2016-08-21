#ifndef CCURSORS_H
#define CCURSORS_H

#include <SDL2/SDL.h>

class Cursors {
private:

	static SDL_Cursor* xpm_to_cursor(const char **xpm, int len);
	
public:
	static SDL_Cursor* defaultCursor;
	static SDL_Cursor* textCursor;
	static SDL_Cursor* zoomCursor;
	
	static void InitCursors();
	static void CleanupCursors();

};

#endif
