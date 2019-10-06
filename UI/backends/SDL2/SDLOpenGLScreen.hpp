#ifndef NG_SDL_SCREEN_HPP
#define NG_SDL_SCREEN_HPP
#include <SDL2/SDL.h>
#include "../../backend/Screen.hpp"
#include "../../Image.hpp"
namespace ng {

class SDLOpenGLScreen : public Screen {
	private:
		bool using_scissor;
		ng::Rect clip_region;
	public:
	
		SDLOpenGLScreen();
		virtual void SetResolution( int w, int h );
		virtual void GetResolution( int &w, int &h );
		virtual void Init();
		virtual void Rect(int x, int y, int w, int h, unsigned int color);
		virtual void FillRect(int x, int y, int w, int h, unsigned int color);
		virtual void FillCircle(int x, int y, float radius, unsigned int color);
		virtual void Circle(int x, int y, float radius, unsigned int color);
		virtual void Line(int xA, int yA, int xB, int yB, unsigned int color);
		virtual void VLine(int x1, int y1, int y2, unsigned int color);
		virtual void SetMaxAlpha(float _max_alpha);
		virtual void SetRotation(float rotation, int cx=0, int cy=0);
		virtual void SetRotationPoint(int x, int y);
		virtual void SetClipRegion(int x, int y, int w, int h, bool enable=true);
		virtual bool GetClipRegion(int &x, int &y, int &w, int &h);
		virtual void DeleteTexture(unsigned int textureID);
		virtual void CacheImage(Image* img);
		virtual void TexRect(int x, int y, int w, int h, Image* texture, bool repeat = false, int texWidth = 1, int texHeight = 1);
		virtual void TexRect(int x, int y, Image* tex, bool repeat = false);
		
		virtual uint32_t CompileShader(std::string vertexShader, std::string fragmentShader);
		virtual void SetShader(uint32_t shader_id);
		virtual bool IsShadersSupported();
		
		virtual int	 SetNewOffScreenRender();
		virtual bool FreeOffScreenRender(int id);
		virtual bool SetOffScreenRender(int id);
		virtual Image* GetOffScreenTexture(int id);
};

}
#endif
