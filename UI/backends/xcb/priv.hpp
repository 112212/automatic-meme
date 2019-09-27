#include <xcb/xcb.h>

namespace ng {
namespace xcbBackend {


// struct FontPriv {
	// ::Font font;
	// ::XFontStruct *font_struct;
// };


extern xcb_connection_t *g_connection;
extern xcb_screen_t *g_screen;
extern xcb_drawable_t  g_window;
extern xcb_gcontext_t  g_foreground;

}}
