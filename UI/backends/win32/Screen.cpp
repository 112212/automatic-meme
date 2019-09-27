// #include "Drawing.hpp"
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>

#include "Screen.hpp"
#include "../../common.hpp"
#include "../../Color.hpp"


#include "priv.hpp"

namespace ng {
namespace win32Backend {
	HDC s_mdc;
	
	Screen::Screen() {
		using_scissor = false;
		s_mdc = CreateCompatibleDC(NULL);
	}
	
	void Screen::Init() {
		static bool inited = false;
		
		if(!inited) {
			inited = true;
		}
	}

	int sizeX=800, sizeY=800;
	void Screen::SetResolution( int w, int h ) {
		sizeX = w;
		sizeY = h;
	}
	
	void Screen::GetResolution( int &w, int &h ) {
		w = sizeX;
		h = sizeY;
	}
	
	static float custom_color[4] = {1.0,1.0,1.0,0};
	static float custom_color_interp = 0.0;
	
	static float max_alpha = 1.0f;
	void Screen::SetMaxAlpha(float _max_alpha) {
		max_alpha = _max_alpha;
	}
	
	static float rotation = 0.0f;
	static float rot_sin=0.0f, rot_cos=1.0f;
	void Screen::SetRotation(float _rotation, int cx, int cy) {
		if(rotation != _rotation) {
			rotation = _rotation;
			if(rotation == 0.0f) return;
			// rot_cos = cos(rotation * M_PI/180.0f);
			// rot_sin = sin(rotation * M_PI/180.0f);
			SetRotationPoint(cx,cy);
		}
	}
	
	static float center_x=0.0f, center_y=0.0f;
	void Screen::SetRotationPoint(int x, int y) {
		center_x = (x * 2.0f) / sizeX - 1.0f;
		center_y = -((y * 2.0f) / sizeY - 1.0f);
	}
	
	static void rotate(float* p) {
		// [ cos -sin ]   [ x ]
		// [ sin cos  ]   [ y ]
		float x = p[0] - center_x;
		float y = p[1] - center_y;
		p[0] = (x * rot_cos - y * rot_sin) + center_x;
		p[1] = (x * rot_sin + y * rot_cos) + center_y;
	}

	
	void Screen::set_pen(uint32_t color) {
		HDC hdc = win32Backend::g_hdc;
		SelectObject(hdc, GetStockObject(DC_PEN));
		auto col = Color(color);
		SetDCPenColor(hdc, RGB(col.r,col.g,col.b));
	}
	void Screen::set_brush(uint32_t color) {
		HDC hdc = win32Backend::g_hdc;
		auto col = Color(color);
		if (col.a == 0) {
			SelectObject(hdc, GetStockObject(NULL_BRUSH));
		} else {
			SelectObject(hdc, GetStockObject(DC_BRUSH));
		}
		SetDCBrushColor(hdc, RGB(col.r,col.g,col.b));
	}
		
	void Screen::Rect(int x, int y, int w, int h, uint32_t color) {
		/*
COLORREF SetDCPenColor(
  HDC      hdc,
  COLORREF color
);
COLORREF SetDCBrushColor(
  HDC      hdc,
  COLORREF color
);
		*/
		// color = Color(color).GetBGR();
		// HDC hdc = win32Backend::g_hdc;
		// SelectObject(hdc, GetStockObject(BLACK_PEN));
		// SelectObject(hdc, GetStockObject(WHITE_PEN));
		// ::SetDCPenColor(win32Backend::g_hdc, color);
		// ::SetDCBrushColor(win32Backend::g_hdc, 0x00abcdef);
		// ::Rectangle(win32Backend::g_hdc, x, y, x+w, y+h);
		
		// SetDCPenColor(hdc, RGB(0,0,255));
		// SetDCBrushColor(hdc, RGB(0,255,0));
		Line(x,y, x+w, y, color);
		Line(x+w,y, x+w, y+h, color);
		Line(x+w,y+h, x, y+h, color);
		Line(x,y+h, x, y, color);
	}

	void Screen::FillRect(int x, int y, int w, int h, uint32_t color) {
		Color c(color);
		HBRUSH hbr = CreateSolidBrush(RGB(c.r, c.g, c.b));
		RECT r{x,y,x+w,y+h};
		::FillRect(win32Backend::g_hdc, &r, hbr);
		DeleteObject(hbr);
	}

	void Screen::FillCircle(int x, int y, float radius, uint32_t color) {
		// FillRect(x-radius/2, y-radius/2, radius, radius, color);
		HDC hdc = win32Backend::g_hdc;
		set_brush(color);
		Ellipse(hdc, x-radius/2, y-radius/2, x+radius/2, y+radius/2);
	}

	void Screen::Circle(int x, int y, float radius, uint32_t color) {
		// Rect(x-radius/2, y-radius/2, radius, radius, color);
		HDC hdc = win32Backend::g_hdc;
		// SelectObject(hdc, GetStockObject(NULL_BRUSH));
		set_brush(0);
		set_pen(color);
		Ellipse(hdc, x-radius/2, y-radius/2, x+radius/2, y+radius/2);
	}
	
	HBITMAP LoadBitmapFromBuffer(char *buffer, int width, int height) {
		return CreateBitmap(width, height, 1, 32, buffer);
	}
	
	void Screen::CacheImage(Image* img) {
		img->Update();
		Size s = img->GetImageSize();
		Point a, b;
		unsigned int texid = img->GetTextureId();
		
		if(img->GetAffectedRegion(a,b)) {
			if(texid < bitmaps.size()) {
				HBITMAP tex = (HBITMAP)bitmaps[texid];
				DeleteObject(tex);
			} else {
				bitmaps.push_back(0);
				texid = bitmaps.size()-1;
			}
			
			ng::Rect r = {a.x, a.y, b.x-a.x, b.y-a.y};
			const uint32_t* pixels = img->GetImage();
			HBITMAP bitmap = LoadBitmapFromBuffer((char*)pixels, r.w, r.h);
			bitmaps[texid] = bitmap;
			
			SetCache(img, texid);
			img->ResetAffectedRegion();
		}
	}
	
	HRGN cur_hrgn = 0;
	
	void Screen::SetClipRegion(int x, int y, int w, int h, bool enable) {
		HDC hdc = win32Backend::g_hdc;
		if(enable) {
			if(x != -1) {
				clip_region = ng::Rect(x,y,w,h);
				auto hrgn = CreateRectRgn(x, y, x+w, y+h); 
				HRGN prev = cur_hrgn;
				cur_hrgn=hrgn;
				SelectClipRgn(hdc, hrgn); 
				if(prev) {
					DeleteObject(prev);
				}
			}
			using_scissor = true;
		} else {
			SelectClipRgn(hdc, 0);
			HRGN prev = cur_hrgn;
			if(prev) {
				DeleteObject(prev);
			}
			cur_hrgn = 0;
			using_scissor = false;
		}
	}
	
	bool Screen::GetClipRegion(int &x, int &y, int &w, int &h) {
		x = clip_region.x;
		y = clip_region.y;
		w = clip_region.w;
		h = clip_region.h;
		return using_scissor;
	}
	
	void Screen::TexRect(int x, int y, Image* tex, bool repeat) {
		ng::Rect s = tex->GetImageCropRegion();
		if(s.w == 0 || s.h == 0 || !tex->GetImage() ) return;
		TexRect(x, y, s.w, s.h, tex, repeat, s.w, s.h);
	}
	
	//void TexRect(int x, int y, int w, int h, uint32_t texture) {
	void Screen::TexRect(int x, int y, int w, int h, Image* texture, bool repeat, int texWidth, int texHeight) {
		CacheImage(texture);
		ng::Rect cr = texture->GetImageCropRegion();
		// ng::Rect src_rect = {cr.x, cr.y, cr.x+cr.w, cr.y+cr.h};
		ng::Rect src_rect = {cr.x, cr.y, cr.w, cr.h};
		// ng::Rect dst_rect = {x,y, x+w, y+h};
		ng::Rect dst_rect = {x,y, w, h};
		int texid = texture->GetTextureId();
		if(texid < bitmaps.size()) {
			SelectObject(s_mdc, bitmaps[texid]);
			SetBkMode(s_mdc, TRANSPARENT);
			
			BLENDFUNCTION bf;
			bf.BlendOp = AC_SRC_OVER;
			bf.BlendFlags = 0;
			bf.AlphaFormat = AC_SRC_ALPHA;   // use source alpha  
			bf.SourceConstantAlpha = 0xbf;
			AlphaBlend(win32Backend::g_hdc, dst_rect.x, dst_rect.y, src_rect.w, src_rect.h, s_mdc, src_rect.x, src_rect.y, src_rect.w, src_rect.h, bf);

			// int ret = BitBlt(win32Backend::g_hdc, dst_rect.x, dst_rect.y, src_rect.w, src_rect.h, s_mdc, src_rect.x, src_rect.y, SRCCOPY);
			// if(ret == 0) {
				// std::cout << GetLastError() << "\n";
			// }
			// SelectObject(win32Backend::g_mdc, old_bitmap);
		}
	}

	void Screen::DeleteTexture(uint32_t textureID) {
	}

	void Screen::Line(int x1, int y1, int x2, int y2, uint32_t color) {
		HDC hdc = win32Backend::g_hdc;
		SelectObject(hdc, GetStockObject(DC_PEN));
		// SelectObject(hdc, GetStockObject(WHITE_PEN));
		auto col = Color(color);
		SetDCPenColor(hdc, RGB(col.r,col.g,col.b));
		MoveToEx(hdc, x1, y1, NULL);
		LineTo(hdc, x2, y2);
	}

	void Screen::VLine(int x1, int y1, int y2, uint32_t color) {
		Line(x1, y1, x1, y2, color);
	}
	
	void Screen::RemoveFromCache(unsigned int cache_id) {
		DeleteTexture(cache_id);
	}
	
	bool Screen::IsShaderSupported() {
		return false;
	}
	
	int	 Screen::SetNewOffScreenRender() {
		return 0;
	}
	
	bool Screen::FreeOffScreenRender(int id) {
		// glDestro
		return false;
	}
	
	bool Screen::SetOffScreenRender(int id) {
		return false;
	}
	
	Image* Screen::GetOffScreenTexture(int id) {
		return 0;
	}
	
}}
