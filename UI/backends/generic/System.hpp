#ifndef NG_X11_SYSTEM_HPP
#define NG_X11_SYSTEM_HPP

#include "../System.hpp"

namespace ng {

namespace xcbBackend {
class System : public ng::System {
	private:
	public:
		System();
		virtual void SetCursorVisibility(bool visible);
		virtual std::string GetClipboard();
		virtual void SetClipboard(std::string text);
		virtual bool GetKeystate(Keyboard_Scancode c);
};

}}
#endif
