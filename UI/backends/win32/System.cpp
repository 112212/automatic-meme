#include "System.hpp"
#include "priv.hpp"
#include <iostream>
namespace ng {
namespace win32Backend {
	
System::System() {
	
}
void System::SetCursorVisibility(bool visible) {
	// _ShowCursor(visible);
}

void ClipboardText(const char *text)
{
     HGLOBAL glob;
     glob = GlobalAlloc(GMEM_DDESHARE|GMEM_MOVEABLE, strlen(text)+1);
     char *tmp = (char*)GlobalLock(glob);
     strcpy(tmp, text);
     GlobalUnlock(glob);
     OpenClipboard(NULL);
	 EmptyClipboard();
	 SetClipboardData(CF_TEXT, glob);
	 CloseClipboard();
}
void ClipboardGetData(char *data)
{
     if(OpenClipboard(NULL))
     {
		 HANDLE clp = GetClipboardData(CF_TEXT);
		 strcpy(data, (char*)clp);
		 CloseClipboard();
     }
}

std::string System::GetClipboard() {
	char data[1000];
	ClipboardGetData(data);
	return std::string(data);
}


void System::SetClipboard(std::string text) {
	ClipboardText(text.c_str());
}

bool System::GetKeystate(Keyboard_Scancode c) {
	// _PumpEvents();
	// return _GetKeyboardState(NULL)[(uint32_t)c];
	return ::GetKeyState(c);
	// return false;
}

}}
