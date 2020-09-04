#include "Cursor.hpp"
#include "backend/Screen.hpp"
#include "backend/System.hpp"
#include "managers/Images.hpp"
namespace ng {

// #include "Cursor_resources.hpp"
Cursor::Cursor() : m_relative_mode(false) {
	m_cursors.resize(CursorType::max_cursors);
	for(CursorDesc& d : m_cursors) {
		d.img = 0;
	}
	m_cur_cursor = CursorType::pointer;
}

void Cursor::Render(Screen* screen) {
	CursorDesc& c = m_cursors[(int)m_cur_cursor];
	if(c.img) {
		screen->TexRect(m_cursor_position.x+c.offset.x, m_cursor_position.y+c.offset.y, c.size.w, c.size.h, c.img);
	}
}

void Cursor::SetCursorImage(CursorType type, std::string filename, Point offset) {
	// CursorDesc prev_cursor = m_cursors[(int)type];
	CursorDesc d;
	d.img = Images::GetImage( filename );
	if(!d.img) return;
	d.offset = offset;
	d.orig_offset = offset;
	Rect s = d.img->GetImageCropRegion();
	d.size.w = s.w;
	d.size.h = s.h;
	m_cursors[(int)type] = d;
}

void Cursor::SetCursorType(CursorType type) {
	if(m_cursors[(int)type].img) {
		m_cur_cursor = type;
	}
}

void Cursor::SetCursorImage(CursorType type, Image* img, Point offset) {
	CursorDesc d;
	d.img = img;
	if(!d.img) return;
	d.offset = offset;
	d.orig_offset = offset;
	Rect s = d.img->GetImageCropRegion();
	d.size.w = s.w;
	d.size.h = s.h;
	m_cursors[(int)type] = d;
}

// static init
void Cursor::Init() {
}
void Cursor::Deinit() {
}


// -----------------

void Cursor::SetRelativeMode(bool relativemode) {
	m_relative_mode = relativemode;
}

void Cursor::SetCursor(int x, int y) {
	m_cursor_position.x = x;
	m_cursor_position.y = y;
}

void Cursor::SetSize(int w, int h) {
	for(int i=0; i < m_cursors.size(); i++) {
		CursorDesc &d = m_cursors[(int)i];
		if(d.img) {
			SetSize((CursorType)i, w, h);
		}
	}
}

void Cursor::SetSize(CursorType type, int w, int h) {
	CursorDesc &d = m_cursors[(int)type];
	if(!d.img) return;
	Rect s = d.img->GetImageCropRegion();
	d.offset.x = d.orig_offset.x * w / s.w;
	d.offset.y = d.orig_offset.y * h / s.h;
	d.size.w = w;
	d.size.h = h;
}

Point Cursor::GetPosition() {
	return m_cursor_position;
}

void Cursor::MoveCursor(int dx, int dy) {
	if(!m_relative_mode) {
		SetCursor(dx,dy);
	} else {
		m_cursor_position.x += dx;
		m_cursor_position.y += dy;
	}
}

}
