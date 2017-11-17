#include "SDLSystem.hpp"
#include <SDL2/SDL.h>

namespace ng {
	
SDLSystem::SDLSystem() {
	
}
void SDLSystem::SetCursorVisibility(bool visible) {
	SDL_ShowCursor(visible);
}

std::string SDLSystem::GetClipboard() {
	return std::string( SDL_GetClipboardText() );
}

void SDLSystem::SetClipboard(std::string text) {
	SDL_SetClipboardText(text.c_str());
}

bool SDLSystem::GetKeystate(Keyboard_Scancode c) {
	SDL_PumpEvents();
	return SDL_GetKeyboardState(NULL)[(uint32_t)c];
}

}
