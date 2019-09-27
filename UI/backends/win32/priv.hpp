#include <windows.h>
namespace ng {
namespace win32Backend {
/*
struct Priv {
	::Display* display;
	::Visual* visual;
	::Window window;
	::Colormap colormap;
};

struct FontPriv {
	::Font font;
	::XFontStruct *font_struct;
};

*/
	extern HDC g_hdc;
	extern HDC g_mdc;

}}
