#ifndef NG_SDL_SCREEN_HPP
#define NG_SDL_SCREEN_HPP
#include <SDL.h>
#include "../../backend/Screen.hpp"
#include "../../Image.hpp"
namespace ng {

class SDLScreen : public Screen {
	private:
		bool using_scissor;
		ng::Rect clip_region;
		SDL_Renderer* ren;
		std::vector<SDL_Texture*> textures;
		std::vector<uint32_t> textures_holes;
	public:
	
		SDLScreen();
		void SetResolution( int w, int h );
		void GetResolution( int &w, int &h );
		void Init();
		void Rect(int x, int y, int w, int h, unsigned int color);
		void FillRect(int x, int y, int w, int h, unsigned int color);
		void FillCircle(int x, int y, float radius, unsigned int color);
		void Circle(int x, int y, float radius, unsigned int color);
		void Line(int xA, int yA, int xB, int yB, unsigned int color);
		void VLine(int x1, int y1, int y2, unsigned int color);
		void SetMaxAlpha(float _max_alpha);
		void SetRotation(float rotation, int cx=0, int cy=0);
		void SetRotationPoint(int x, int y);
		void SetClipRegion(int x, int y, int w, int h, bool enable=true);
		bool GetClipRegion(int &x, int &y, int &w, int &h);
		void DeleteTexture(unsigned int textureID);
		void CacheImage(ng::Image* img);
		void TexRect(int x, int y, int w, int h, Image* texture, bool repeat = false, int texWidth = 1, int texHeight = 1);
		void TexRect(int x, int y, Image* tex, bool repeat = false);
		
		
		
		uint32_t CompileShader(std::string vertexShader, std::string fragmentShader);
		void SetShader(uint32_t shader_id);
		bool IsShadersSupported();
		
		int	 SetNewOffScreenRender();
		bool FreeOffScreenRender(int id);
		bool SetOffScreenRender(int id);
		Image* GetOffScreenTexture(int id);
		void SetSDLRenderer(SDL_Renderer* ren);
};

}
#endif
