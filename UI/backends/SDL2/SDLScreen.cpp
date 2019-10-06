#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>
#include <cmath>

#include "SDLScreen.hpp"
#include "../../common.hpp"
#include "../../Color.hpp"

namespace ng {
	
	struct vec2 {
		float x,y;
	};
	struct vec4 {
		float x,y,z,w;
	};
	
	SDLScreen::SDLScreen() {
		using_scissor = false;
	}
	
	void SDLScreen::Init() {
		static bool inited = false;
		
		if(!inited) {			
			inited = true;
		}
	}
	
	void SDLScreen::SetSDLRenderer(SDL_Renderer* ren) {
		this->ren = ren;
		
		SDL_SetRenderDrawBlendMode(ren, SDL_BLENDMODE_BLEND);
	}

	int sizeX=800, sizeY=800;
	void SDLScreen::SetResolution( int w, int h ) {
		sizeX = w;
		sizeY = h;
	}
	
	void SDLScreen::GetResolution( int &w, int &h ) {
		w = sizeX;
		h = sizeY;
	}
	
	
	static float custom_color[4] = {1.0,1.0,1.0,0};
	static float custom_color_interp = 0.0;
	
	static float max_alpha = 1.0f;
	void SDLScreen::SetMaxAlpha(float _max_alpha) {
		max_alpha = _max_alpha;
	}
	
	static float rotation = 0.0f;
	static float rot_sin=0.0f, rot_cos=1.0f;
	void SDLScreen::SetRotation(float _rotation, int cx, int cy) {
		if(rotation != _rotation) {
			rotation = _rotation;
			if(rotation == 0.0f) return;
			rot_cos = cos(rotation * M_PI/180.0f);
			rot_sin = sin(rotation * M_PI/180.0f);
			SetRotationPoint(cx,cy);
		}
	}
	
	static float center_x=0.0f, center_y=0.0f;
	void SDLScreen::SetRotationPoint(int x, int y) {
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

	void SDLScreen::Rect(int x, int y, int w, int h, uint32_t color) {
		Color c(color);
		uint8_t alpha = std::min((uint8_t)(max_alpha*255), c.a);
		SDL_SetRenderDrawColor(ren, c.r, c.g, c.b, alpha);
		SDL_Rect r = {x,y,w,h};
		SDL_RenderDrawRect(ren, &r);
	}

	void SDLScreen::FillRect(int x, int y, int w, int h, unsigned int color) {
		Color c(color);
		uint8_t alpha = std::min((uint8_t)(max_alpha*255), c.a);
		SDL_SetRenderDrawColor(ren, c.r, c.g, c.b, alpha);
		SDL_Rect r = {x,y,w,h};
		SDL_RenderFillRect(ren, &r);
	}

	void SDLScreen::FillCircle(int x0, int y0, float radius0, uint32_t color) {
		// FillRect(x0-radius0/2,y0-radius0/2,radius0,radius0,color);
		Color c(color);
		uint8_t alpha = std::min((uint8_t)(max_alpha*255), c.a);
		SDL_SetRenderDrawColor(ren, c.r, c.g, c.b, alpha);
		std::vector<SDL_Point> points;
		points.reserve(50);
		
		int radius = radius0/2;
		int x = radius-1;
		int y = 0;
		int dx = 1;
		int dy = 1;
		int err = dx - (radius << 1);

		while (x >= y)
		{
			
			points.push_back({x0, y0 + y});
			points.push_back({x0 + x, y0 + y});
			
			points.push_back({x0, y0 + x});
			points.push_back({x0 + y, y0 + x});
			
			points.push_back({x0, y0 + x});
			points.push_back({x0 - y, y0 + x});
			
			points.push_back({x0, y0 + y});
			points.push_back({x0 - x, y0 + y});
			
			points.push_back({x0, y0 - y});
			points.push_back({x0 - x, y0 - y});
			
			points.push_back({x0, y0 - x});
			points.push_back({x0 - y, y0 - x});
			
			points.push_back({x0, y0 - x});
			points.push_back({x0 + y, y0 - x});
			
			points.push_back({x0, y0 - y});
			points.push_back({x0 + x, y0 - y});

			if (err <= 0)
			{
				y++;
				err += dy;
				dy += 2;
			}
			
			if (err > 0)
			{
				x--;
				dx += 2;
				err += dx - (radius << 1);
			}
		}
		SDL_RenderDrawLines(ren, points.data(), points.size());
	}

	/*
	void SDLScreen::Circle(int x, int y, float radius, uint32_t color) {
		// Rect(x-radius/2,y-radius/2,radius,radius, color);
		// SDL_Surface* surface = SDL_CreateRGBSurface(0, width, height, 32, 0, 0, 0, 0);
		// SDL_Rect src_rect;
		// SDL_Rect dst_rect;
		
		std::vector<SDL_Point> points(50);
		// radius /= 2;
		int radius_half = radius/2;
		float radius2 = radius*radius;
		for(int y1=0; y1 <= radius_half; y1++) {
			int x1 = sqrt(radius2 - y1*y1);
			// for(int x=0; x1 < radius; x++) {
				// 1
				points.push_back({x+x1,y+y1});
				// 2
				points.push_back({x-x1,y+y1});
				// 3
				points.push_back({x-x1,y-y1});
				// 4
				points.push_back({x+x1,y-y1});
			// }
			// std::cout << x1 << ", " << y1 << "\n";
		}
		
		SDL_RenderDrawPoints(ren, points.data(), points.size());
	}
	*/
	
	void SDLScreen::Circle(int x0, int y0, float radius0, uint32_t color) {
		// Rect(x-radius/2,y-radius/2,radius,radius, color);
		// SDL_Surface* surface = SDL_CreateRGBSurface(0, width, height, 32, 0, 0, 0, 0);
		// SDL_Rect src_rect;
		// SDL_Rect dst_rect;
		Color c(color);
		uint8_t alpha = std::min((uint8_t)(max_alpha*255), c.a);
		SDL_SetRenderDrawColor(ren, c.r, c.g, c.b, alpha);
		std::vector<SDL_Point> points;
		points.reserve(50);
		
		int radius = radius0/2;
		int x = radius-1;
		int y = 0;
		int dx = 1;
		int dy = 1;
		int err = dx - (radius << 1);

		while (x >= y)
		{
			points.push_back({x0 + x, y0 + y});
			points.push_back({x0 + y, y0 + x});
			points.push_back({x0 - y, y0 + x});
			points.push_back({x0 - x, y0 + y});
			points.push_back({x0 - x, y0 - y});
			points.push_back({x0 - y, y0 - x});
			points.push_back({x0 + y, y0 - x});
			points.push_back({x0 + x, y0 - y});

			if (err <= 0)
			{
				y++;
				err += dy;
				dy += 2;
			}
			
			if (err > 0)
			{
				x--;
				dx += 2;
				err += dx - (radius << 1);
			}
		}
		
		SDL_RenderDrawPoints(ren, points.data(), points.size());
	}
	
	void SDLScreen::CacheImage(Image* img) {
		img->Update();
		Size s = img->GetImageSize();
		Point a, b;
		unsigned int texid = img->GetTextureId();
		SDL_Texture* tex;
		
		// SDL_SetHint( SDL_HINT_RENDER_SCALE_QUALITY, "1" );
		if(img->GetAffectedRegion(a,b)) {
			if(texid < textures.size()) {
				tex = textures[texid];
			} else {
				tex = SDL_CreateTexture(ren, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, s.w, s.h);
				SDL_SetTextureBlendMode(tex, SDL_BLENDMODE_BLEND);
				if(textures_holes.empty()) {
					// no holes, push directly to tex
					texid = textures.size();
					textures.push_back(tex);
				} else {
					// place tex to hole
					texid = textures_holes.back();
					textures_holes.pop_back();
					textures[texid] = tex;
				}
			}
			
			// img->FreeCache();
			SDL_Rect r = {a.x, a.y, b.x-a.x, b.y-a.y};
			// std::cout << "rect: " << r.x << ", " << r.y << ", " << r.w << ", " << r.h << "\n";
			const uint32_t* pixels = img->GetImage();
			uint8_t* out_pitch;
			uint32_t* out_pixels;
			
			SDL_LockTexture(tex, &r, (void**)&out_pixels, (int*)&out_pitch);
			if(out_pixels) {
				int len = s.w*s.h;
				for(int i=0; i < len; i++) {
					out_pixels[i] = pixels[i];
				}
				// memcpy(out_pixels, pixels, 4*s.w*s.h);
			}
			SDL_UnlockTexture(tex);
			
			SetCache(img, texid);
			img->ResetAffectedRegion();
		}
		// SDL_SetHint( SDL_HINT_RENDER_SCALE_QUALITY, "0" );
	}
	
	void SDLScreen::SetClipRegion(int x, int y, int w, int h, bool enable) {
				std::cout << "scissor\n"; 
		if(enable) {
			if(x != -1) {
				SDL_Rect r = {x,y,w,h};
				SDL_RenderSetClipRect(ren, &r);
				clip_region = Rect(x,y,w,h);
			}
			using_scissor = true;
		} else {
			using_scissor = false;
			SDL_RenderSetClipRect(ren, 0);
		}
	}
	
	bool SDLScreen::GetClipRegion(int &x, int &y, int &w, int &h) {
		x = clip_region.x;
		y = clip_region.y;
		w = clip_region.w;
		h = clip_region.h;
		return using_scissor;
	}
	
	void SDLScreen::TexRect(int x, int y, Image* tex, bool repeat) {
		ng::Rect s = tex->GetImageCropRegion();
		if(s.w == 0 || s.h == 0 || !tex->GetImage() ) return;
		TexRect(x, y, s.w, s.h, tex, repeat, s.w, s.h);
	}

	//void TexRect(int x, int y, int w, int h, uint32_t texture) {
	void SDLScreen::TexRect(int x, int y, int w, int h, Image* texture, bool repeat, int texWidth, int texHeight) {
		
		CacheImage(texture);
		
		ng::Rect cr = texture->GetImageCropRegion();
		SDL_Rect src_rect = {cr.x, cr.y, cr.w, cr.h};
		SDL_Rect dst_rect = {x,y,w,h};
		int idx = texture->GetTextureId();
		if(idx > textures.size()) return;
		auto* tex = textures[idx];
		SDL_SetTextureAlphaMod(tex, (uint8_t)(max_alpha*255));
		SDL_RenderCopy(ren, tex, &src_rect, &dst_rect);
	}


	void SDLScreen::DeleteTexture(uint32_t textureID) {
		if(textureID < textures.size()) {
			SDL_DestroyTexture(textures[textureID]);
			textures[textureID] = 0;
			textures_holes.push_back(textureID);
		}
	}

	void SDLScreen::Line(int xA, int yA, int xB, int yB, uint32_t color) {
		Color c(color);
		SDL_SetRenderDrawColor(ren, c.r, c.g, c.b, c.a);
		SDL_RenderDrawLine(ren, xA,yA,xB,yB);
	}

	void SDLScreen::VLine(int x1, int y1, int y2, uint32_t color) {
		Line(x1, y1, x1, y2, color);
	}
	
	uint32_t SDLScreen::CompileShader(std::string vertexShader, std::string fragmentShader ) {
		// return loadShader(vertexShader.c_str(), fragmentShader.c_str());
		return 0;
	}
	
	void SDLScreen::SetShader(uint32_t shader_id) {
		// glUseProgram(shader_id);
	}
	
	bool SDLScreen::IsShadersSupported() {
		return false;
	}
	
	int	 SDLScreen::SetNewOffScreenRender() {
		return 0;
	}
	
	bool SDLScreen::FreeOffScreenRender(int id) {
		// glDestro
		return false;
	}
	
	bool SDLScreen::SetOffScreenRender(int id) {
		return false;
	}
	
	Image* SDLScreen::GetOffScreenTexture(int id) {
		return 0;
	}
	
}
