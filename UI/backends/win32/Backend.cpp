#include "config.h"

#include <iostream>
#include <chrono>

#include "../../backend/Input.hpp"

#include "../../Gui.hpp"
#include "../../managers/Images.hpp"
#include "../../managers/Fonts.hpp"
#include "../../managers/Sounds.hpp"
#include "../../managers/ResourceManager.hpp"

#include "Screen.hpp"
#include "Font.hpp"

#ifdef USE_LIBSND
	#include "../../extensions/SoundLibSnd.hpp"
#endif

#ifdef USE_LIBPNG
	#include "../../extensions/Image_libpng.hpp"
#endif

#ifdef USE_LIBBMP
	#include "../../extensions/Image_libbmp.hpp"
#endif


#include "Speaker.hpp"
#include "System.hpp"
#include "Backend.hpp"

#include <tchar.h> 
namespace ng {
namespace win32Backend {
using std::cout;
using std::endl;

ng::Gui *g_gui = 0;
::HDC g_hdc;
::HDC g_mdc;

HWND window;
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

HINSTANCE hInst;

std::map<int,int> key_mapping;

bool SetupWindow(const char* window_name, int posx, int posy, int width, int height) {
	HINSTANCE hInstance = GetModuleHandle(NULL);
	WNDCLASSEX wcex;
	static TCHAR szWindowClass[] = _T("win32app");
	wcex.cbSize = sizeof(WNDCLASSEX);  
	wcex.style          = CS_HREDRAW | CS_VREDRAW;  
	wcex.lpfnWndProc    = WndProc;  
	wcex.cbClsExtra     = 0;  
	wcex.cbWndExtra     = 0;  
	wcex.hInstance      = hInstance; 
	wcex.hIcon          = LoadIcon(hInstance, IDI_APPLICATION);  
	wcex.hCursor        = LoadCursor(NULL, IDC_ARROW);  
	wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);  
	wcex.lpszMenuName   = NULL;  
	wcex.lpszClassName  = szWindowClass;  
	wcex.hIconSm        = LoadIcon(wcex.hInstance, IDI_APPLICATION);  
  
	if (!RegisterClassEx(&wcex))
	{
		MessageBox(0,
			_T("Call to RegisterClassEx failed!"),  
			_T(" Guided Tour"),  
			0);  

		return 1;
	}

	hInst = hInstance;
	key_mapping[38] = KEY_UP;
	key_mapping[40] = KEY_DOWN;
	key_mapping[37] = KEY_LEFT;
	key_mapping[39] = KEY_RIGHT;
	key_mapping['\b'] = KEY_BACKSPACE;
	key_mapping[36] = KEY_HOME;
	key_mapping[35] = KEY_END;
	key_mapping[33] = KEY_PAGEUP;
	key_mapping[34] = KEY_PAGEDOWN;
	key_mapping[45] = KEY_INSERT;
	key_mapping[46] = KEY_DELETE;
	
	HWND hWnd = CreateWindow(  
		szWindowClass,  
		window_name,  
		WS_OVERLAPPEDWINDOW,  
		posx, posy,  
		width, height,  
		NULL,  
		NULL,  
		hInstance,
		NULL  
	);  
  
	window = hWnd;

	RegisterExtensions();
}


// ::Window GetWindow() {
	// return win;
// }

void RegisterExtensions() {
	ng::ResourceManager::RegisterResourceLoader("Font", Font::GetFont);
	
	#ifdef USE_LIBPNG
	ng::ResourceManager::RegisterResourceLoader("libpng", Image_libpng::LoadPNG);
	ng::ResourceManager::RegisterExtension(".png", "libpng");
	#endif
	
	#ifdef USE_LIBBMP
	ng::ResourceManager::RegisterResourceLoader("libbmp", Image_libbmp::LoadBMP);
	ng::ResourceManager::RegisterExtension(".bmp", "libbmp");
	#endif
}

Screen* screen = 0;
Speaker* speaker = 0;
System* system = 0;
bool inited = false;
void SetBackend(Gui* gui) {
	if(!screen) {
		screen = new Screen();
	}
	if(!speaker) {
		speaker = new Speaker();
	}
	if(!system) {
		system = new System();
	}
	
	// Priv* priv = (Priv*)screen->priv;
	// priv->display = display;
	// priv->visual = visual;
	// priv->window = window;
	// priv->colormap = XCreateColormap(display, window, visual, AllocNone);
	Backend backend(screen, speaker, system);
	gui->SetBackend(backend);
}

// TODO: maybe move to utilities in core
static int mapSingleMaskToValue(int value, std::vector<int> from_mask, std::vector<int> to) {
	for(int i=0; i < from_mask.size(); i++) {
		if ((value & from_mask[i]) == from_mask[i]) {
			return to[i];
		}
	}
	return 0;
}

static int mouseKeyMap(int value, int def) {
	int res = mapSingleMaskToValue(value, 
		{MK_LBUTTON,MK_MBUTTON,MK_RBUTTON,0},
		{MouseButton::BUTTON_LEFT, MouseButton::BUTTON_MIDDLE, MouseButton::BUTTON_RIGHT,def});
	// std::cout << "key mapped to : " << value << " => " << res << " def is " << def  << "\n";
	return res;
}


static int getKeyState() {
	int lctrl = (GetKeyState(VK_LCONTROL) & (1<<7)) ? ng::Keymod::KEYMOD_LCTRL : 0;
	int rctrl = (GetKeyState(VK_RCONTROL) & (1<<7)) ? ng::Keymod::KEYMOD_RCTRL : 0;
	int lshift = (GetKeyState(VK_LSHIFT) & (1<<7)) ? ng::Keymod::KEYMOD_LSHIFT : 0;
	int rshift = (GetKeyState(VK_RSHIFT) & (1<<7)) ? ng::Keymod::KEYMOD_RSHIFT : 0;
	int shift = lshift | rshift | lctrl | rctrl;
	return shift;
}

HDC mdc = 0;
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
	PAINTSTRUCT ps;  
	HDC hdc;
	// TCHAR greeting[] = _T("Hello, World!");
	RECT rect;
	GetClientRect(hWnd, &rect);
	switch (message) {
		
	case WM_ERASEBKGND:
		return true;
		
	case WM_RBUTTONUP:
		g_gui->OnMouseUp( mouseKeyMap(wParam, MouseButton::BUTTON_RIGHT) );
		InvalidateRect(window, 0, true);
		InvalidateRect(hWnd, NULL, TRUE);
		break;
		
	case WM_RBUTTONDOWN:
		g_gui->OnMouseDown( mouseKeyMap(wParam, MouseButton::BUTTON_RIGHT) );
		InvalidateRect(window, 0, true);
		InvalidateRect(hWnd, NULL, TRUE);
		break;
		
	case WM_LBUTTONDOWN:
		g_gui->OnMouseDown( mouseKeyMap(wParam, MouseButton::BUTTON_LEFT) );
		InvalidateRect(window, 0, true);
		InvalidateRect(hWnd, NULL, TRUE);
		break;
		
	case WM_LBUTTONUP:
		g_gui->OnMouseUp( mouseKeyMap(wParam, MouseButton::BUTTON_LEFT) );
		InvalidateRect(window, 0, true);
		InvalidateRect(hWnd, NULL, TRUE); 
		break;
		
	case WM_MOUSEWHEEL:
		g_gui->OnMWheel( GET_WHEEL_DELTA_WPARAM(wParam) / WHEEL_DELTA * 4 );
		break;

	case WM_KEYUP:
		g_gui->OnKeyUp((ng::Keycode)wParam, (ng::Keymod)0);
		InvalidateRect(hWnd, NULL, TRUE); 
		break;
	case WM_CHAR:
		{
			int shift = getKeyState();
			std::string s(1,(char)wParam);
			if(s[0] == '\r') break;
			if(s[0] == '\b') break;
			
			int mod = getKeyState();
			if(mod & KEYMOD_CTRL) {
				break;
			}
			try {
				g_gui->OnText(s);
			} catch(std::exception e) {
				
			}
		}
		break;
		
	case WM_KEYDOWN:
		{
			if (wParam == 27) { // ESC QUIT
				PostQuitMessage(0);
			}
			
			// std::cout << "kpress: " << wParam << " " << (char)wParam << "\n";
			auto key = key_mapping.find(wParam);
			if(key != key_mapping.end()) {
				g_gui->OnKeyDown((ng::Keycode)key->second, (ng::Keymod)getKeyState());
			} else {
				g_gui->OnKeyDown((ng::Keycode)tolower(wParam), (ng::Keymod)getKeyState());
			}
			
			break;
		}
	case WM_PAINT: {
			hdc = BeginPaint(hWnd, &ps);
			if(!mdc) {
				return 0;
			}
			g_hdc = mdc;
			Size s = g_gui->GetSize();
			screen->FillRect(0,0,s.w, s.h, 0xff000000);
			g_gui->Render();
			// std::cout << "copying: " << rect.right << ", " << rect.bottom << "\n";
			int ret = BitBlt(hdc, 0, 0, rect.right, rect.bottom, mdc, 0, 0, SRCCOPY);
			if(ret == 0) {
				std::cout << "error bitblt " << GetLastError() << "\n";
			}
			EndPaint(hWnd, &ps);
			break;
		}
	case WM_MOUSEMOVE: {
			int y = HIWORD(lParam);
			int x = LOWORD(lParam);
			g_gui->OnMouseMove( x,y );
			InvalidateRect(hWnd, 0, true);
			break;
		}
	case WM_DESTROY:  
		PostQuitMessage(0);  
		break;  
	default:  
		return DefWindowProc(hWnd, message, wParam, lParam);  
	}
	return 0;  
}  

HBITMAP mbmp,moldbmp;
void MainLoop(Gui* gui) {
	g_gui = gui;
	bool running = true;
	Size s = gui->GetSize();
	auto tp = std::chrono::high_resolution_clock::now();
	
	 // The parameters to ShowWindow explained:  
	// hWnd: the value returned from CreateWindow  
	// nCmdShow: the fourth parameter from WinMain  
	// ShowWindow(hWnd,  nCmdShow);
	ShowWindow(window, 1);
	UpdateWindow(window);

	std::cout << "1st gui size: " << s << "\n";
	HDC hdcMain = GetDC(window);
	mdc = CreateCompatibleDC(NULL);
	g_mdc = mdc;
	// mbmp =  CreateBitmap(s.w, s.h,1,24,NULL); // width, height, 1, bit_depth, NULL
	mbmp =  CreateCompatibleBitmap(hdcMain, s.w, s.h); // width, height, 1, bit_depth, NULL
	moldbmp = (HBITMAP)SelectObject(mdc,mbmp);
	
	MSG msg;
	while(GetMessage(&msg, NULL, 0, 0)) {  
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	
	SelectObject(mdc, moldbmp);
	DeleteObject(mbmp);
	DeleteDC(mdc);
}


void CloseBackend() {
	
}


}} // end namespaces
