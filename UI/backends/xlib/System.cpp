#include "System.hpp"
#include "priv.hpp"

namespace ng {
namespace xlibBackend {	
System::System() {
	
}
void System::SetCursorVisibility(bool visible) {
	// X11_ShowCursor(visible);
}

std::string System::GetClipboard() {
	return "";
}

void System::SetClipboard(std::string text) {
	
}

bool System::GetKeystate(Keyboard_Scancode c) {
	// X11_PumpEvents();
	// return X11_GetKeyboardState(NULL)[(uint32_t)c];
	return false;
}

}}
