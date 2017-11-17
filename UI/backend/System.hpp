#ifndef NG_SYSTEM_HPP
#define NG_SYSTEM_HPP

#include "Input.hpp"
#include <string>
namespace ng {
class System {
	private:
	
	public:
		virtual void SetCursorVisibility(bool visible) {}
		virtual std::string GetClipboard() {}
		virtual void SetClipboard(std::string text) {}
		virtual bool GetKeystate(Keyboard_Scancode c) {return false;}
};
}
#endif
