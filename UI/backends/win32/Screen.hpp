#ifndef NG_WIN32_SCREEN_HPP
#define NG_WIN32_SCREEN_HPP
#include "../../backend/Screen.hpp"
#include "../../Image.hpp"
namespace ng {
namespace win32Backend {
class Screen : public ng::Screen {
	private:
		bool using_scissor;
		ng::Rect clip_region;
		std::vector<void*> bitmaps;
		
		void set_pen(uint32_t col);
		void set_brush(uint32_t col);
	public:
		// void* priv;
	
		Screen();
		void SetResolution( int w, int h );
		void GetResolution( int &w, int &h );
		void Init();
		
		void Rect(int x, int y, int w, int h, unsigned int color);
		void FillRect(int x, int y, int w, int h, unsigned int color);
		void FillCircle(int x, int y, float radius, unsigned int color);
		void Circle(int x, int y, float radius, unsigned int color);
		
		void Line(int xA, int yA, int xB, int yB, unsigned int color);
		void VLine(int x1, int y1, int y2, unsigned int color);
		
		void SetMaxAlpha(float max_alpha);
		
		void SetRotation(float rotation, int cx=0, int cy=0);
		void SetRotationPoint(int x, int y);
		
		void SetClipRegion(int x, int y, int w, int h, bool enable=true);
		bool GetClipRegion(int &x, int &y, int &w, int &h);
		
		void DeleteTexture(unsigned int textureID);
		void CacheImage(Image* img);
		void RemoveFromCache(unsigned int cache_id);
		void TexRect(int x, int y, int w, int h, Image* texture, bool repeat = false, int texWidth = 1, int texHeight = 1);
		void TexRect(int x, int y, Image* tex, bool repeat = false);
		
		bool IsShaderSupported();
		
		int	 SetNewOffScreenRender();
		bool FreeOffScreenRender(int id);
		bool SetOffScreenRender(int id);
		Image* GetOffScreenTexture(int id);
		// void SetRenderer(_Renderer* ren);
};

}}
#endif
