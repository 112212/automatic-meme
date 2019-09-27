#ifndef NG_BACKEND_WIN32_HPP
#define NG_BACKEND_WIN32_HPP

#include <windows.h>

#define MAIN int CALLBACK WinMain(   \
    _In_ HINSTANCE hInstance,  		 \
    _In_ HINSTANCE hPrevInstance,    \
    _In_ LPSTR     lpCmdLine,        \
    _In_ int       nCmdShow          \
)  

namespace ng {
class Gui;
namespace win32Backend {
	
	bool SetupWindow(const char* window_name, int posx, int posy, int width, int height);
	void SetBackend(Gui* gui);
	void MainLoop(Gui* gui);
	void RegisterExtensions();
	// void* GetWindow();
	void CloseBackend();
	
}}

#endif
