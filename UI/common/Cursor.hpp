#ifndef CURSORS_HPP
#define CURSORS_HPP

#include <SDL2/SDL.h>
#include "common.hpp"
#include <string>
namespace ng {
class Cursor {
private:

	static SDL_Cursor* xpm_to_cursor(const char **xpm, int len);
	Rect m_pointer;
	unsigned int m_pointer_tex;
	bool m_relative_mode;
	
public:
	Cursor();
	void SetCursor(int x, int y);
	void SetRelativeMode(bool relativemode);
	void SetSize(int w, int h); 
	// void SetCursor( std::string cursor_name );
	
	Point GetCursor();
	void  MoveCursor(int dx, int dy);
	
	static SDL_Cursor* defaultCursor;
	static SDL_Cursor* textCursor;
	static SDL_Cursor* zoomCursor;

	static void InitCursors();
	static void CleanupCursors();


};
}
#endif
