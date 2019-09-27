#ifndef NG_WIN32_SYSTEM_HPP
#define NG_WIN32_SYSTEM_HPP

#include "../../backend/System.hpp"

namespace ng {
namespace win32Backend {

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
