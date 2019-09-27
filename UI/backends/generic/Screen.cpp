// #include "Drawing.hpp"
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>
#include <cmath>

#include "Screen.hpp"
#include "../../common.hpp"
#include "../../Color.hpp"



#include "priv.hpp"

namespace ng {
namespace xcbBackend {
	struct vec2 {
		float x,y;
	};
	struct vec4 {
		float x,y,z,w;
	};
	
	
	
	Screen::Screen() {
		using_scissor = false;
		// priv = new Priv;
		
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
			rot_cos = cos(rotation * M_PI/180.0f);
			rot_sin = sin(rotation * M_PI/180.0f);
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

	void Screen::Rect(int x, int y, int w, int h, uint32_t color) {
		// Priv* priv = (Priv*)this->priv;
		// XSetForeground(priv->display, DefaultGC(priv->display, 0), color);
		// XDrawRectangle(priv->display, priv->window, DefaultGC(priv->display, 0), x, y, w, h);
		xcb_rectangle_t rectangle = {x, y, w, h};
		xcb_change_gc(g_connection, g_foreground, XCB_GC_FOREGROUND, &color);
		xcb_poly_rectangle (g_connection, g_window, g_foreground, 1, &rectangle);
	}

	void Screen::FillRect(int x, int y, int w, int h, uint32_t color) {
		xcb_rectangle_t rectangle = {x, y, w, h};
		xcb_change_gc(g_connection, g_foreground, XCB_GC_FOREGROUND, &color);
		xcb_poly_fill_rectangle (g_connection, g_window, g_foreground, 1, &rectangle);
	}


	void Screen::FillCircle(int x, int y, float radius, uint32_t color) {
		FillRect(x-radius/2, y-radius/2, radius, radius, color);
	}

	void Screen::Circle(int x, int y, float radius, uint32_t color) {
		Rect(x-radius/2, y-radius/2, radius, radius, color);
	}
	
	void Screen::CacheImage(Image* img) {
		/*
		img->Update();
		Size s = img->GetImageSize();
		Point a, b;
		unsigned int texid = img->GetTextureId();
		
		
		if(img->GetAffectedRegion(a,b)) {
			
			if(texid < ximages.size()) {
				XImage* tex = (XImage*)ximages[texid];
				// XFree
			} else {
				ximages.push_back(0);
				texid = ximages.size()-1;
			}
			
			// img->FreeCache();
			ng::Rect r = {a.x, a.y, b.x-a.x, b.y-a.y};
			// std::cout << "rect: " << r.x << ", " << r.y << ", " << r.w << ", " << r.h << "\n";
			const uint32_t* pixels = img->GetImage();
			// uint8_t* out_pitch;
			// uint32_t* out_pixels;
			Priv* priv = (Priv*)this->priv;
			ximages[texid] = XCreateImage(priv->display, priv->visual, 24, ZPixmap, 0, (char*)pixels, s.w, s.h, 32, 0);
			
			// X11_LockTexture(tex, &r, (void**)&out_pixels, (int*)&out_pitch);
			// if(out_pixels) {
				// memcpy(out_pixels, pixels, 4*s.w*s.h);
			// }
			// X11_UnlockTexture(tex);
			
			SetCache(img, texid);
			img->ResetAffectedRegion();
		}
		*/
		
		
		
	}
	
	void Screen::SetClipRegion(int x, int y, int w, int h, bool enable) {
		if(enable) {
			if(x != -1) {
				// X11_Rect r = {x,y,w,h};
				// X11_RenderSetClipRect(ren, &r);
				clip_region.x = x;
				clip_region.y = y;
				clip_region.w = w;
				clip_region.h = h;
			}
			using_scissor = true;
		} else {
			using_scissor = false;
			// X11_RenderSetClipRect(ren, 0);
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
		
		// CacheImage(tex);
		// std::cout << "rendering tex2\n";
		TexRect(x, y, s.w, s.h, tex, repeat, s.w, s.h);
	}

	//void TexRect(int x, int y, int w, int h, uint32_t texture) {
	void Screen::TexRect(int x, int y, int w, int h, Image* texture, bool repeat, int texWidth, int texHeight) {
		/*
		XPutImage(display, d, gc, image, src_x, src_y, dest_x, dest_y, width, height)
			Display *display;
			Drawable d;
			GC gc;
			XImage *image;
			int src_x, src_y;
			int dest_x, dest_y;
			unsigned int width, height;
        */
		CacheImage(texture);
		ng::Rect cr = texture->GetImageCropRegion();
		ng::Rect src_rect = {cr.x, cr.y, cr.w, cr.h};
		ng::Rect dst_rect = {x,y,w,h};
		/*
		
		Priv* priv = (Priv*)this->priv;
		
		if(texture->GetTextureId() < ximages.size()) {
			XPutImage(priv->display, priv->window, DefaultGC(priv->display, 0), (XImage*)ximages[texture->GetTextureId()], 
				src_rect.x, src_rect.y, dst_rect.x, dst_rect.y, dst_rect.w, dst_rect.h);
		}
		*/
	}


	void Screen::DeleteTexture(uint32_t textureID) {
		// if(glIsTexture(textureID)) {
			// glDeleteTextures(1, &textureID);
		// }
		// X11_FreeTexture(textures[textureID]);
	}

	void Screen::Line(int x1, int y1, int x2, int y2, uint32_t color) {
		xcb_segment_t segment = {x1, y1, x2,y2};
		xcb_change_gc(g_connection, g_foreground, XCB_GC_FOREGROUND, &color);
		xcb_poly_segment (g_connection, g_window, g_foreground, 1, &segment);
	}

	void Screen::VLine(int x1, int y1, int y2, uint32_t color) {
		Line(x1, y1, x1, y2, color);
	}

	void Screen::RemoveFromCache(unsigned int cache_id) {
		DeleteTexture(cache_id);
	}
	
	uint32_t Screen::CompileShader(std::string vertexShader, std::string fragmentShader ) {
		// return loadShader(vertexShader.c_str(), fragmentShader.c_str());
		return 0;
	}
	
	void Screen::SetShader(uint32_t shader_id) {
		// glUseProgram(shader_id);
	}
	
	bool Screen::IsShadersSupported() {
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
