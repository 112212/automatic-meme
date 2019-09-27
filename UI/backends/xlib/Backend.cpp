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
	#include "../../extensions/SoundLibSnd.hpp"
#endif
#ifdef USE_LIBPNG
	#include "../../extensions/Image_libpng.hpp"
#endif
#ifdef USE_LIBBMP
	#include "../../extensions/Image_libbmp.hpp"
#endif
#include "Speaker.hpp"
#include "System.hpp"
#include "Backend.hpp"
#include "priv.hpp"

namespace ng {
namespace xlibBackend {
using std::cout;
using std::endl;

::Display* display = 0;
::Visual* visual = 0;
Window window;

bool SetupWindow(const char* window_name, int posx, int posy, int width, int height) {
	display = XOpenDisplay(NULL);
	visual = DefaultVisual(display, 0);
	
	
	window=XCreateSimpleWindow(display, RootWindow(display, 0), 0, 0, width, height, 1, 0, 0);
	XSelectInput(display, window, KeyPress|KeyRelease|ButtonPressMask|ButtonReleaseMask|ExposureMask|PointerMotionMask);
	XMapWindow(display, window);
	RegisterExtensions();
}


// ::Window GetWindow() {
	// return win;
// }


void RegisterExtensions() {
	ng::ResourceManager::RegisterResourceLoader("Font", Font::GetFont);
	#ifdef USE_LIBPNG
	ng::ResourceManager::RegisterResourceLoader("libpng", Image_libpng::LoadPNG);
	ng::ResourceManager::RegisterExtension(".png", "libpng");
	#endif
	
	#ifdef USE_LIBBMP
	ng::ResourceManager::RegisterResourceLoader("libbmp", Image_libbmp::LoadBMP);
	ng::ResourceManager::RegisterExtension(".bmp", "libbmp");
	#endif
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
	
	Priv* priv = (Priv*)screen->priv;
	priv->display = display;
	priv->visual = visual;
	priv->window = window;
	priv->colormap = XCreateColormap(display, window, visual, AllocNone);
	Backend backend(screen, speaker, system);
	gui->SetBackend(backend);
}

void ProcessInput(Gui* gui, XEvent& ev) {
	switch(ev.type) {
		case KeyPress:
		case KeyRelease:
			break;
		case ButtonPress:
			// std::cout << " mdown : " << ev.xbutton.x << " , " <<  ev.xbutton.y << "\n";
			gui->OnMouseDown( ev.xbutton.button );
			break;
		case ButtonRelease:
			// std::cout << " mup : " << ev.xbutton.x << " , " <<  ev.xbutton.y << "\n";
			gui->OnMouseUp( ev.xbutton.button );
			break;
		case MotionNotify:
			// std::cout << " motion : " << ev.xmotion.x << " , " <<  ev.xmotion.y << "\n";
			gui->OnMouseMove( ev.xmotion.x, ev.xmotion.y );
			break;
		default: {}
	}
}

void MainLoop(Gui* gui) {
	
	bool running = true;
	Size s = gui->GetSize();
	auto tp = std::chrono::high_resolution_clock::now();
	while(running) {
		XEvent ev;
		XNextEvent(display, &ev);
		// std::cout << ev.type << "\n";
		if(ev.type == Expose) {
			// std::cout << "expose\n";
			// XClearWindow(display, window);
			gui->Render();
			XFlush(display);
			// XClearWindow(display, window);
		} else {
			ProcessInput(gui, ev);
			auto tp2 = std::chrono::high_resolution_clock::now();
			if(tp2 - tp > std::chrono::milliseconds(500)) {
				
				ev.type = Expose;
				XSendEvent(display, window, false, ExposureMask, &ev);
				tp2 = tp;
			}
		}
		
		// XClearWindow(display, window);
		
	}
}


void CloseBackend() {
	
}


}} // end namespaces
