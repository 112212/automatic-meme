#ifndef CCURSORS_H
#define CCURSORS_H

#include <SDL/SDL.h>

class Cursors {
private:
	static const char *cursor[];
	static const char *zoom[];

	static SDL_Cursor* xpm_to_cursor(const char **xpm, int len);
	
public:
	static SDL_Cursor* defaultCursor;
	static SDL_Cursor* textCursor;
	static SDL_Cursor* zoomCursor;
	
	static void InitCursors();
	static void CleanupCursors();

};






#endif
