// i686-w64-mingw32-gcc wingl.c -o wingl.exe -Wl,-subsystem,windows -mwindows -lopengl32
#include <windows.h>
#include <GL/gl.h>

#pragma comment (lib, "opengl32.lib")

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
#define __in
#define __in_opt
int WINAPI WinMain( __in HINSTANCE hInstance, __in_opt HINSTANCE hPrevInstance, __in_opt LPSTR lpCmdLine, __in int nShowCmd )
{
	MSG msg          = {0};
	WNDCLASS wc      = {0}; 
	wc.lpfnWndProc   = WndProc;
	wc.hInstance     = hInstance;
	wc.hbrBackground = (HBRUSH)(COLOR_BACKGROUND);
	wc.lpszClassName = "oglversionchecksample";
	wc.style = CS_OWNDC;
	if( !RegisterClass(&wc) )
		return 1;
	CreateWindowW(L"oglversionchecksample",L"openglversioncheck",WS_OVERLAPPEDWINDOW|WS_VISIBLE,0,0,640,480,0,0,hInstance,0);
	// CreateWindow(wc.lpszClassName,L"openglversioncheck",WS_OVERLAPPEDWINDOW|WS_VISIBLE,0,0,640,480,0,0,hInstance,0);

	while( GetMessage( &msg, NULL, 0, 0 ) > 0 )
		DispatchMessage( &msg );

	return 0;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch(message)
	{
	case WM_CREATE:
		{
		PIXELFORMATDESCRIPTOR pfd =
		{
			sizeof(PIXELFORMATDESCRIPTOR),
			1,
			PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER,    //Flags
			PFD_TYPE_RGBA,        // The kind of framebuffer. RGBA or palette.
			32,                   // Colordepth of the framebuffer.
			0, 0, 0, 0, 0, 0,
			0,
			0,
			0,
			0, 0, 0, 0,
			24,                   // Number of bits for the depthbuffer
			8,                    // Number of bits for the stencilbuffer
			0,                    // Number of Aux buffers in the framebuffer.
			PFD_MAIN_PLANE,
			0,
			0, 0, 0
		};

		HDC ourWindowHandleToDeviceContext = GetDC(hWnd);

		int  letWindowsChooseThisPixelFormat;
		letWindowsChooseThisPixelFormat = ChoosePixelFormat(ourWindowHandleToDeviceContext, &pfd); 
		SetPixelFormat(ourWindowHandleToDeviceContext,letWindowsChooseThisPixelFormat, &pfd);

		HGLRC ourOpenGLRenderingContext = wglCreateContext(ourWindowHandleToDeviceContext);
		wglMakeCurrent (ourWindowHandleToDeviceContext, ourOpenGLRenderingContext);

		MessageBoxA(0,(char*)glGetString(GL_VERSION), "OPENGL VERSION",0);

		wglDeleteContext(ourOpenGLRenderingContext);
		PostQuitMessage(0);
		}
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}
