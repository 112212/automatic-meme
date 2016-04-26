#ifndef _CSURFACE_H_
#define _CSURFACE_H_
 
#include <SDL2/SDL.h>
#include <SDL2/SDL2_gfxPrimitives.h>
 
namespace ng {
class CSurface {
    public:
        CSurface();
 
    public:
        static SDL_Surface* OnLoad(const char* File);
        static bool OnDraw(SDL_Surface* Surf_Dest, SDL_Surface* Surf_Src, int X, int Y);
        static bool OnDraw(SDL_Surface* Surf_Dest, SDL_Surface* Surf_Src, int X, int Y, int X2, int Y2, int W, int H);
        static SDL_Surface* CreateSurface(Uint32 flags,int width,int height,const SDL_Surface* display);
        static bool OnDraw(SDL_Renderer* ren, SDL_Surface* Surf_Src, int x, int y);
        static bool OnDraw(SDL_Renderer* ren, SDL_Surface* Surf_Src, int x, int y, int x2, int y2, int w, int h);
        static bool OnDraw(SDL_Renderer* ren, SDL_Texture* tex, SDL_Surface* Surf_Src, int x, int y);
};
 
// inline void Draw_FillRect( SDL_Renderer* ren, int x, int y, int w, int h, Uint32 color );
// inline void Draw_Rect( SDL_Renderer* ren, int x, int y, int w, int h, Uint32 color );
// inline void Draw_Circle(SDL_Renderer* ren, int x, int y, float radius, Uint32 color);
// inline void Draw_FillCircle(SDL_Renderer* ren, int x, int y, float radius, Uint32 color);
// inline void Draw_Line(SDL_Renderer* ren, int x1, int y1, int x2, int y2, Uint32 color);
// inline void Draw_FillEllipse(SDL_Renderer* ren, int x, int y, float rx, float ry, Uint32 color);
// inline void Draw_VLine(SDL_Renderer* ren, int x1, int y1, int x2, Uint32 color );


inline void Draw_FillRect( SDL_Renderer* ren, int x, int y, int w, int h, Uint32 color ) {
	boxColor(ren, x, y, x+w, y+h, color);
}
inline void Draw_Rect( SDL_Renderer* ren, int x, int y, int w, int h, Uint32 color ) {
	rectangleColor(ren, x, y, x+w, y+h, color);
}
inline void Draw_FillCircle(SDL_Renderer* ren, int x, int y, float radius, Uint32 color) {
	filledCircleColor(ren, x, y, radius, color);
}
inline void Draw_Circle(SDL_Renderer* ren, int x, int y, float radius, Uint32 color) {
	circleColor(ren, x, y, radius, color);
}
inline void Draw_Line(SDL_Renderer* ren, int x1, int y1, int x2, int y2, Uint32 color) {
	lineColor(ren, x1,y1,x2,y2,color);
}
inline void Draw_FillEllipse(SDL_Renderer* ren, int x, int y, float rx, float ry, Uint32 color) {
	filledEllipseColor(ren, x,y,rx,ry,color);
}
inline void Draw_VLine(SDL_Renderer* ren, int x1, int y1, int x2, Uint32 color ) {
	vlineColor(ren, x1, y1, x2, color);
}
}
#endif
