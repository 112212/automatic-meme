#ifndef NG_CURSORS_HPP
#define NG_CURSORS_HPP
#include <string>
#include <vector>

#include "Image.hpp"
#include "common.hpp"
namespace ng {
enum CursorType {
	pointer,
	hand,
	resize_x,
	resize_y,
	resize_diag,
	resize_diag2,
	move,
	textinput,
	max_cursors
};
	
class Cursor {
private:
	Point m_cursor_position;
	bool m_relative_mode;
	CursorType m_cur_cursor;
	struct CursorDesc {
		Size size;
		Point offset;
		Point orig_offset;
		Image* img;
	};
	std::vector<CursorDesc> m_cursors;
public:
	
	Cursor();
	void SetCursor(int x, int y); // TODO: -> SetCursorPosition
	void SetRelativeMode(bool relativemode);
	void SetSize(int w, int h);
	void SetSize(CursorType type, int w, int h);
	void SetCursorType(CursorType type);
	
	Point GetCursor();
	void MoveCursor(int dx, int dy);
	void SetCursorImage(CursorType type, std::string filename, Point offset);
	void SetCursorImage(CursorType type, Image* img, Point offset);
	void Render(Screen* screen);

	static void Init();
	static void Deinit();
};
}
#endif
