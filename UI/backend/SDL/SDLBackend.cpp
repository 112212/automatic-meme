#include <GL/glew.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>
#include <iostream>

#include "SDLBackend.hpp"
#include "SDLSpeaker.hpp"
#include "SDLScreen.hpp"
#include "SDL_TTF_Font.hpp"
#include "SDL_Surface_Image.hpp"
#include "SDLSound.hpp"
#include "SDLSystem.hpp"
#include "../drivers/SoundLibSnd.hpp"
#include "../../Gui.hpp"
#include "../../managers/Images.hpp"
#include "../../managers/Fonts.hpp"
#include "../../managers/Sounds.hpp"

namespace ng {
namespace SDLBackend {
using std::cout;
using std::endl;

SDL_Window* win = 0;
SDL_Renderer* ren = 0;
bool SetupSDLWindow(const char* window_name, int posx, int posy, int width, int height) {
	if(SDL_Init(SDL_INIT_EVERYTHING) != 0) {
        throw std::string("Failed to initialize SDL: ") + SDL_GetError();
    }

    std::cout << "creating sdl window\n";
    
    bool adv_gl = true;
    if(adv_gl) {
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
		SDL_SetHint("SDL_HINT_RENDER_VSYNC", "1");
		SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);
		SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 5);
		SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 5);
		SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 5);
		SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
		SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
		SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	}
	int sizeX = 1280;
	int sizeY = 900;
	win = SDL_CreateWindow(window_name, posx, posy, width, height, SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL  | SDL_WINDOW_RESIZABLE);
	if(!win) {
		cout << "creating window error\n";
		SDL_Quit();
		return true;
	}
	// SDL_CreateRender
	if(!adv_gl) {
		ren = SDL_CreateRenderer( win, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC );
	} else {
		SDL_GLContext glcontext = SDL_GL_CreateContext(win);
		
		if (!glcontext) {
			cout << "Couldn't create context: %s" << SDL_GetError() << endl;
			// SDL_DestroyRenderer(ren);
			SDL_DestroyWindow(win);
			SDL_Quit();
			return false;
		}
		
		SDL_GL_MakeCurrent(win, glcontext);
	}
	// glewExperimental = GL_TRUE; 
	glewInit();
	
    RegisterExtensions();
}

void RegisterExtensions() {
	ng::Fonts::RegisterFontExtension(".ttf", ng::SDL_TTF_Font::GetFont);
    ng::Images::RegisterImageExtension(".jpg", ng::SDL_Surface_Image::LoadImage);
    ng::Images::RegisterImageExtension(".png", ng::SDL_Surface_Image::LoadPNG);
	ng::Sounds::RegisterSoundExtension( ".wav", SDLSound::LoadSound);
	ng::Sounds::RegisterSoundExtension( ".ogg", SoundLibSnd::LoadSound);
}

SDLScreen* screen = 0;
SDLSpeaker* speaker = 0;
SDLSystem* system = 0;
bool inited = false;
void SetSDLBackend(Gui* gui) {
	if(!screen) {
		screen = new SDLScreen();
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
				// case SDL_WINDOWEVENT_SIZE_CHANGED: // probably not needed
					if(gui->GetBackend().screen) {
						gui->GetBackend().screen->SetResolution(e.window.data1, e.window.data2);
					}
					gui->ProcessLayout();
					break;
			}
	}
}

void SDLMainLoop(Gui* gui) {
	
	SDL_GL_SetSwapInterval(1);
	// SDL_EnableUNICODE(1);
	// SDL_StopTextInput();
	SDL_StartTextInput();

	bool running = true;
	while(running) {
		SDL_Event e;
		while(SDL_PollEvent(&e)) {
			if(e.type == SDL_KEYDOWN) {
				if(e.key.keysym.sym == 'q' || e.key.keysym.sym == SDLK_ESCAPE) {
					running = false;
				}
			} else if(e.type == SDL_QUIT) {
				running = false;
			}
			
			SDLProcessInput(gui, e);
		}
		Size s = gui->GetSize();
		
		glViewport(0, 0, s.w, s.h);
		glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
        glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
        
		gui->Render();
		SDL_GL_SwapWindow(win);
	}
	SDL_StopTextInput();
}


void CloseBackend() {
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
