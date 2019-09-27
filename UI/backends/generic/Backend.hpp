#ifndef NG_X11_BACKEND_HPP
#define NG_X11_BACKEND_HPP



namespace ng {
class Gui;
namespace xcbBackend {
	
	bool SetupWindow(const char* window_name, int posx, int posy, int width, int height);
	void SetBackend(Gui* gui);
	void MainLoop(Gui* gui);
	void RegisterExtensions();
	// void* GetWindow();
	void CloseBackend();
	
}}

#endif
