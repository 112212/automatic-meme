#include "config.h"

#include <SDL.h>
#ifdef USE_OPENGL
	#include <GL/glew.h>
	#include <SDL2/SDL_opengl.h>
	#include "SDLOpenGLScreen.hpp"
#else
	#include "SDLScreen.hpp"
#endif
#include <iostream>

#include "Backend.hpp"
#include "SDLSpeaker.hpp"
#include "SDL_Surface_Image.hpp"
#include "SDLSound.hpp"
#include "SDLSystem.hpp"
#include "../../Gui.hpp"
#include "../../managers/Images.hpp"
#include "../../managers/Fonts.hpp"
#include "../../managers/Sounds.hpp"

#ifdef USE_SDL2_ttf
	#include "SDL_TTF_Font.hpp"
#endif

#ifdef USE_LIBSND
	#include "../../extensions/SoundLibSnd.hpp"
#endif

#ifdef USE_LIBPNG
	#include "../../extensions/Image_libpng.hpp"
#endif

#ifdef USE_LIBBMP
	#include "../../extensions/Image_libbmp.hpp"
#endif

#ifdef EMSCRIPTEN
	#include <emscripten.h>
#endif

namespace ng {
namespace SDL2Backend {
using std::cout;
using std::endl;

SDL_Window* win = 0;
SDL_Renderer* ren = 0;
void a_loop();

bool SetupWindow(const char* window_name, int posx, int posy, int width, int height) {
	// if(SDL_Init(SDL_INIT_EVERYTHING) != 0) {
        // throw std::string("Failed to initialize SDL: ") + SDL_GetError();
    // }
	if(SDL_Init(SDL_INIT_VIDEO|SDL_INIT_EVENTS|SDL_INIT_TIMER) != 0) {
        throw std::string("Failed to initialize SDL: ") + SDL_GetError();
    }
	
    std::cout << "creating sdl window\n";
    
    bool adv_gl = true;
#ifndef USE_OPENGL
	adv_gl = false;
#endif
    
    int flags = SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE;
#ifdef USE_OPENGL
    if(adv_gl) {
		std::cout << "using opengl\n";
		// SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
		// SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
		// SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
		SDL_SetHint("SDL_HINT_RENDER_VSYNC", "0");
		SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);
		SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 5);
		SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 5);
		SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 5);
		SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
		SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
		SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	}
	flags |= SDL_WINDOW_OPENGL;
#endif
	
	
	
	std::cout << "creating window\n";
	win = SDL_CreateWindow(window_name, posx, posy, width, height, flags);
	if(!win) {
		cout << "creating window error\n";
		SDL_Quit();
		return true;
	}
	// SDL_CreateRender
	if(!adv_gl) {
		// ren = SDL_CreateRenderer( win, -1, SDL_RENDERER_ACCELERATED  );
		ren = SDL_CreateRenderer( win, -1, SDL_RENDERER_ACCELERATED  );
		if(ren == 0) {
			std::cout << "failed to create rendered\n";
		}
	} else {
		
#ifdef USE_OPENGL
		SDL_GLContext glcontext = SDL_GL_CreateContext(win);
		
		if (!glcontext) {
			cout << "Couldn't create context: %s" << SDL_GetError() << endl;
			SDL_DestroyWindow(win);
			SDL_Quit();
			return false;
		}
		
		SDL_GL_MakeCurrent(win, glcontext);
#endif
	}
	
#ifdef USE_OPENGL
	// glewExperimental = GL_TRUE;
	glewInit();
#endif
	
	std::cout << "RegisterExtensions\n";
    RegisterExtensions();
    return true;
}

SDL_Window* GetWindow() {
	return win;
}



void RegisterExtensions() {
#ifdef USE_SDL2_ttf
	ng::ResourceManager::RegisterResourceLoader("SDL2_ttf", ng::SDL_TTF_Font::GetFont);
	ng::ResourceManager::RegisterExtension(".ttf", "SDL2_ttf");
#endif
	
#ifdef USE_SDL2_image
	ng::ResourceManager::RegisterResourceLoader("SDL2_image", ng::SDL_Surface_Image::LoadIMG);
	ng::ResourceManager::RegisterExtension(".jpg", "SDL2_image");
	
	#ifndef USE_LIBBMP
	ng::ResourceManager::RegisterResourceLoader("SDL2_image", ng::SDL_Surface_Image::LoadIMG);
	ng::ResourceManager::RegisterExtension(".bmp", "SDL2_image");
	#endif
	
	#ifdef USE_LIBPNG
		ng::ResourceManager::RegisterResourceLoader("SDL2_libpng", ng::SDL_Surface_Image::LoadPNG);
		ng::ResourceManager::RegisterExtension(".png", "SDL2_libpng");
	#else
		#ifdef USE_SDL2_image
			ng::ResourceManager::RegisterExtension(".png", "SDL2_image");
		#endif
	#endif	
#endif
	
#ifdef USE_LIBPNG
	ng::ResourceManager::RegisterResourceLoader("libpng", Image_libpng::LoadPNG);
	ng::ResourceManager::RegisterExtension(".png", "libpng");
#endif


#ifdef USE_LIBBMP
	std::cout << "using libbmp\n";
	ng::ResourceManager::RegisterResourceLoader("libbmp", Image_libbmp::LoadBMP);
	ng::ResourceManager::RegisterExtension(".bmp", "libbmp");
#endif
	
	ng::ResourceManager::RegisterResourceLoader("sdlwav", SDLSound::LoadSound);
	ng::ResourceManager::RegisterExtension(".wav", "sdlwav");
	// ng::ResourceManager::RegisterResourceLoader("SDL2", ng::SDL_Surface_Image::LoadBMP);
	// ng::ResourceManager::RegisterExtension(".bmp", "SDL2");
}

Screen* screen = 0;
SDLSpeaker* speaker = 0;
SDLSystem* system = 0;
bool inited = false;

void SetBackend(Gui* gui) {
	if(!screen) {
		#ifdef USE_OPENGL
			screen = new SDLOpenGLScreen();
		#else
			SDLScreen* scr = new SDLScreen();
			scr->SetSDLRenderer(ren);
			screen = scr;
		#endif
	}
	if(!speaker) {
		speaker = new SDLSpeaker();
	}
	if(!system) {
		system = new SDLSystem();
	}
	
	Backend backend(screen, speaker, system);
	gui->SetBackend(backend);
}

void SDLProcessInput(Gui* gui, SDL_Event& e) {
	switch(e.type) {
		
		case SDL_MOUSEMOTION:
			gui->OnMouseMove( e.motion.x, e.motion.y );
			break;
			
		case SDL_KEYDOWN:
			gui->OnKeyDown( (Keycode) e.key.keysym.sym, (Keymod)e.key.keysym.mod );
			break;
			
		case SDL_TEXTEDITING:
			break;
			
		case SDL_TEXTINPUT:
			gui->OnText( e.text.text );
			break;
			
		case SDL_KEYUP:
			//std::cout << "keyup\n";
			gui->OnKeyUp( (Keycode)e.key.keysym.sym, (Keymod)e.key.keysym.mod );
			break;
			
		case SDL_MOUSEBUTTONUP:
			gui->OnMouseUp( e.button.button );
			break;
			
		case SDL_MOUSEBUTTONDOWN:
			gui->OnMouseDown( e.button.button );
			break;
			
		case SDL_MOUSEWHEEL:
			gui->OnMWheel( e.wheel.y );
			break;
			
		case SDL_WINDOWEVENT:
			switch(e.window.event) {
				case SDL_WINDOWEVENT_RESIZED:
				{
				// case SDL_WINDOWEVENT_SIZE_CHANGED: // probably not needed
					Size s{e.window.data1, e.window.data2};
					// if(gui->GetBackend().screen) {
						// gui->GetBackend().screen->SetResolution(s.w,s.h);
					// }
					gui->SetSize(s.w,s.h);
					break;
				}
			}
	}
}

Gui* g_gui;
bool running;

/*
#include <bind.h>

std::string getExceptionMessage(intptr_t exceptionPtr) {
  return std::string(reinterpret_cast<std::exception *>(exceptionPtr)->what());
}

EMSCRIPTEN_BINDINGS(Bindings) {
  emscripten::function("getExceptionMessage", &getExceptionMessage);
};
*/

void a_loop() {
	// std::cout << "a_loop enter" << "\n";
	try {
		Gui* gui = (Gui*)g_gui;
		if(!gui) return;
		Size s = gui->GetSize();
	#ifdef USE_OPENGL
		glViewport(0, 0, s.w, s.h);
		glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
		glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
	#else

		SDL_SetRenderDrawColor(ren, 0, 0, 0, 0xff);
		SDL_RenderClear(ren);
	#endif
		
		Update(gui);
		gui->Render();
		
	#ifdef USE_OPENGL
		SDL_GL_SwapWindow(win);
	#else
		SDL_RenderPresent(ren);
	#endif
	} catch (std::exception& ex) {
		std::cout << "(a_loop) exception: " << ex.what() << "\n";
	}
	// std::cout << "a_loop leave" << "\n";
}


void Update(Gui* gui) {
	// std::cout << "Update gui" << "\n";
	try {
		SDL_PumpEvents();
		SDL_Event e;

		while(SDL_PollEvent(&e)) {
			if(e.type == SDL_KEYDOWN) {
				if(e.key.keysym.sym == SDLK_ESCAPE) {
					running = false;
				}
			} else if(e.type == SDL_QUIT) {
				running = false;
			}
			
			SDLProcessInput(gui, e);
		}
	} catch (std::exception& ex) {
		std::cout << "(Update) exception: " << ex.what() << "\n";
	}
}

void MainLoop(Gui* gui) {
	try {
		g_gui = gui;
		SDL_StartTextInput();
		
		running = true;
	#ifdef EMSCRIPTEN
		std::cout << "MainLoop emscripten_set_main_loop" << "\n";
		emscripten_set_main_loop(a_loop, 60, 1);
		// emscripten_set_main_loop(a_loop, 0, 1);
	#else
		SDL_GL_SetSwapInterval(1);
		while(running) {
			a_loop();
		}
	#endif
		// emscripten_set_main_loop_arg(some_loop, gui, 60, 1);
	} catch (std::exception& ex) {
		std::cout << "(MainLoop) exception: " << ex.what() << "\n";
	}

	// SDL_SetHint( SDL_HINT_RENDER_SCALE_QUALITY, "1" );
	// if(SDL_SetRelativeMouseMode((SDL_bool)1) != 0) {
		// return;
	// }
	
	
	CloseBackend();
}


void CloseBackend() {
	SDL_StopTextInput();
	if(ren) {
		SDL_DestroyRenderer(ren);
	}
	if(win) {
		SDL_DestroyWindow(win);
	}
	SDL_Quit();
}


}
}
