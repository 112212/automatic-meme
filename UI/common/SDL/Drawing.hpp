#ifndef _CSURFACE_H_
#define _CSURFACE_H_
#include <SDL2/SDL.h>
namespace ng {

namespace Drawing {

	
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

	unsigned int GetTextureFromSurface(SDL_Surface* surface, unsigned int textureID);
	unsigned int GetTextureFromSurface2(SDL_Surface* surface, unsigned int textureID);
	void DeleteTexture(unsigned int textureID);
	void TexRect(int x, int y, int w, int h, unsigned int texture, bool repeat = false, int texWidth = 1, int texHeight = 1);
}

}
#endif
