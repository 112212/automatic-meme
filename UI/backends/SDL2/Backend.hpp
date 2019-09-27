#ifndef NG_SDL2_BACKEND_HPP
#define NG_SDL2_BACKEND_HPP

#include "SDL_Surface_Image.hpp"
#include "SDLSound.hpp"

#define MAIN int main(int argc, char* argv[])

namespace ng {
class Gui;
namespace SDL2Backend {
	bool SetupWindow(const char* window_name, int posx, int posy, int width, int height);
	void SetBackend(Gui* gui);
	void MainLoop(Gui* gui);
	void RegisterExtensions();
	void SDLProcessInput(Gui* gui, SDL_Event& e);
	void Update();
	SDL_Window* GetWindow();
	void CloseBackend();
}
}

#endif
