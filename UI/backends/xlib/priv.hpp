#include <X11/Xlib.h>
namespace ng {
namespace xlibBackend {

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


extern ::Display* display;

}}
