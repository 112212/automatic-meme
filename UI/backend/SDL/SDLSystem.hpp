#ifndef SDL_SYSTEM_HPP
#define SDL_SYSTEM_HPP

#include "../System.hpp"

namespace ng {
	
class SDLSystem : public System {
	private:
	public:
		SDLSystem();
		virtual void SetCursorVisibility(bool visible);
		virtual std::string GetClipboard();
		virtual void SetClipboard(std::string text);
		virtual bool GetKeystate(Keyboard_Scancode c);
};

}
#endif
