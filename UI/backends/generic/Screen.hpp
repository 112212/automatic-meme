#ifndef NG_XLIB_SCREEN_HPP
#define NG_XLIB_SCREEN_HPP
#include "../Screen.hpp"
#include "../../Image.hpp"
namespace ng {
namespace xcbBackend {
class Screen : public ng::Screen {
	private:
		bool using_scissor;
		ng::Rect clip_region;
		std::vector<void*> ximages;
	public:
		void* priv;
	
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
		
		uint32_t CompileShader(std::string vertexShader, std::string fragmentShader);
		void SetShader(uint32_t shader_id);
		bool IsShadersSupported();
		
		int	 SetNewOffScreenRender();
		bool FreeOffScreenRender(int id);
		bool SetOffScreenRender(int id);
		Image* GetOffScreenTexture(int id);
};

}}
#endif
