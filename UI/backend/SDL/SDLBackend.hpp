#ifndef SDL_BACKEND_HPP
#define SDL_BACKEND_HPP


#include "SDL_Surface_Image.hpp"
#include "SDL_TTF_Font.hpp"
#include "SDLScreen.hpp"
#include "SDLSpeaker.hpp"
#include "SDLSound.hpp"

namespace ng {
class Gui;
namespace SDLBackend {
	bool SetupSDLWindow(const char* window_name, int posx, int posy, int width, int height);
	void SetSDLBackend(Gui* gui);
	void SDLMainLoop(Gui* gui);
	void RegisterExtensions();
	void SDLProcessInput(Gui* gui, SDL_Event& e);
	void CloseBackend();
}
}

#endif
