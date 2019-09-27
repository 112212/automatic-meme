#include "config.h"

#include <iostream>

#include "../../Gui.hpp"
#include "../../managers/Images.hpp"
#include "../../managers/Fonts.hpp"
#include "../../managers/Sounds.hpp"

#include "../../managers/ResourceManager.hpp"

#include <chrono>
#include "Screen.hpp"
#include "Font.hpp"

#ifdef USE_LIBSND
	#include "../extensions/SoundLibSnd.hpp"
#endif

#include "Speaker.hpp"
#include "System.hpp"
#include "Backend.hpp"
#include "priv.hpp"

namespace ng {
namespace xcbBackend {
using std::cout;
using std::endl;

xcb_connection_t *g_connection;
xcb_screen_t *g_screen;
xcb_drawable_t  g_window;
xcb_gcontext_t  g_foreground;

bool SetupWindow(const char* window_name, int posx, int posy, int width, int height) {
	
	xcb_connection_t *connection;
	xcb_screen_t *screen;
	xcb_drawable_t  window;
	xcb_gcontext_t  foreground;

	/* Open the connection to the X server */
	connection = xcb_connect (NULL, NULL);

	/* Get the first screen */
	screen = xcb_setup_roots_iterator (xcb_get_setup (connection)).data;

	/* Create black (foreground) graphic context */
	window     = screen->root;
	foreground = xcb_generate_id (connection);
	uint32_t        mask       = XCB_GC_FOREGROUND | XCB_GC_GRAPHICS_EXPOSURES;
	uint32_t        values[2]  = {screen->black_pixel, 0};

	xcb_create_gc (connection, foreground, window, mask, values);

	/* Create a window */
	window = xcb_generate_id (connection);

	mask = XCB_CW_BACK_PIXEL | XCB_CW_EVENT_MASK;
	values[0] = screen->white_pixel;
	values[1] = XCB_EVENT_MASK_EXPOSURE | XCB_EVENT_MASK_BUTTON_PRESS   |
                                    XCB_EVENT_MASK_BUTTON_RELEASE | XCB_EVENT_MASK_POINTER_MOTION |
                                    XCB_EVENT_MASK_ENTER_WINDOW   | XCB_EVENT_MASK_LEAVE_WINDOW   |
                                    XCB_EVENT_MASK_KEY_PRESS      | XCB_EVENT_MASK_KEY_RELEASE;

	xcb_create_window (connection,                    /* connection          */
					   XCB_COPY_FROM_PARENT,          /* depth               */
					   window,                        /* window Id           */
					   screen->root,                  /* parent window       */
					   posx, posy,                          /* x, y                */
					   width, height,                      /* width, height       */
					   10,                            /* border_width        */
					   XCB_WINDOW_CLASS_INPUT_OUTPUT, /* class               */
					   screen->root_visual,           /* visual              */
					   mask, values );                /* masks */


	xcb_map_window (connection, window);
	xcb_flush (connection);
	
	g_connection = connection;
	g_screen = screen;
	g_window = window;
	g_foreground = foreground;

	RegisterExtensions();
}


// ::Window GetWindow() {
	// return win;
// }

void RegisterExtensions() {
	ng::ResourceManager::RegisterResourceLoader("Font", Font::GetFont);
	// ng::ResourceManager::RegisterExtension(".bmp", "SDL2");
}

Screen* screen = 0;
Speaker* speaker = 0;
System* system = 0;
bool inited = false;
void SetBackend(Gui* gui) {
	if(!screen) {
		screen = new Screen();
	}
	if(!speaker) {
		speaker = new Speaker();
	}
	if(!system) {
		system = new System();
	}
	
	/*
	Priv* priv = (Priv*)screen->priv;
	priv->display = display;
	priv->visual = visual;
	priv->window = window;
	priv->colormap = XCreateColormap(display, window, visual, AllocNone);
	*/
	Backend backend(screen, speaker, system);
	gui->SetBackend(backend);
}

void ProcessInput(Gui* gui, xcb_generic_event_t *event) {
	switch (event->response_type & ~0x80) {
	case XCB_BUTTON_PRESS: {
		xcb_button_press_event_t *bp = (xcb_button_press_event_t *)event;
		// print_modifiers (bp->state);
		// std::cout << "mouse down\n";
		gui->OnMouseDown( 1 );
		switch (bp->detail) {
		case 4:
			
			break;
		case 5:
			
			break;
		default:
			
			break;
		}
		break;
	}
	case XCB_BUTTON_RELEASE: {
		xcb_button_release_event_t *br = (xcb_button_release_event_t *)event;
		// print_modifiers(br->state);
		// std::cout << "mouse up\n";
		gui->OnMouseUp( 1 );
	 
		break;
	}
	case XCB_MOTION_NOTIFY: {
		xcb_motion_notify_event_t *motion = (xcb_motion_notify_event_t *)event;
		// std::cout << "mouse move " << motion->event_x << " "<< motion->event_y << "\n";
		gui->OnMouseMove( motion->event_x, motion->event_y );
	   
		break;
	}
	case XCB_ENTER_NOTIFY: {
		xcb_enter_notify_event_t *enter = (xcb_enter_notify_event_t *)event;

		
		break;
	}
	case XCB_LEAVE_NOTIFY: {
		xcb_leave_notify_event_t *leave = (xcb_leave_notify_event_t *)event;

	   
		break;
	}
	case XCB_KEY_PRESS: {
		xcb_key_press_event_t *kp = (xcb_key_press_event_t *)event;
		// print_modifiers(kp->state);

	   
		break;
	}
	case XCB_KEY_RELEASE: {
		xcb_key_release_event_t *kr = (xcb_key_release_event_t *)event;
		// print_modifiers(kr->state);

	   
		break;
	}
	default:
		break;
	}
}


void MainLoop(Gui* gui) {
	auto tp = std::chrono::high_resolution_clock::now();
	/* draw primitives */
	xcb_generic_event_t *event;
	while ((event = xcb_wait_for_event (g_connection))) {
		switch (event->response_type & ~0x80) {
		case XCB_EXPOSE:
			
			break;
		default: 
			ProcessInput(gui, event);
			break;
		}

		auto tp2 = std::chrono::high_resolution_clock::now();
		if(tp2 - tp > std::chrono::milliseconds(500)) {
			gui->Render();
			xcb_flush (g_connection);
		}
		
	
		free (event);
	}
	
	
}


void CloseBackend() {
	
}


}} // end namespaces
