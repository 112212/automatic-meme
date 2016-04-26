#include "CSurface.h"
 
namespace ng {
	
CSurface::CSurface() {
}
 
SDL_Surface* CSurface::OnLoad(const char* File) {
	
    SDL_Surface* Surf_Temp = NULL;
    SDL_Surface* Surf_Return = NULL;
 
	/*
    if((Surf_Temp = SDL_LoadBMP((char*)File)) == NULL) {
        return NULL;
    }
 
    Surf_Return = SDL_DisplayFormat(Surf_Temp);
    SDL_FreeSurface(Surf_Temp);
	*/
    return Surf_Return;
}
bool CSurface::OnDraw(SDL_Surface* Surf_Dest, SDL_Surface* Surf_Src, int X, int Y) {
    if(Surf_Dest == NULL || Surf_Src == NULL) {
        return false;
    }
 
    SDL_Rect DestR;
 
    DestR.x = X;
    DestR.y = Y;
 
    SDL_BlitSurface(Surf_Src, NULL, Surf_Dest, &DestR);
 
    return true;
}
bool CSurface::OnDraw(SDL_Surface* Surf_Dest, SDL_Surface* Surf_Src, int X, int Y, int X2, int Y2, int W, int H) {
    if(Surf_Dest == NULL || Surf_Src == NULL) {
        return false;
    }
 
    SDL_Rect DestR;
 
    DestR.x = X;
    DestR.y = Y;
 
    SDL_Rect SrcR;
 
    SrcR.x = X2;
    SrcR.y = Y2;
    SrcR.w = W;
    SrcR.h = H;
 
    SDL_BlitSurface(Surf_Src, &SrcR, Surf_Dest, &DestR);
 
    return true;
}

SDL_Surface* CSurface::CreateSurface(Uint32 flags,int width,int height,const SDL_Surface* display)
{
  // 'display' is the surface whose format you want to match
  //  if this is really the display format, then use the surface returned from SDL_SetVideoMode

  const SDL_PixelFormat& fmt = *(display->format);
  return SDL_CreateRGBSurface(flags,width,height,
                  fmt.BitsPerPixel,
                  fmt.Rmask,fmt.Gmask,fmt.Bmask,fmt.Amask );
}

bool CSurface::OnDraw(SDL_Renderer* ren, SDL_Surface* Surf_Src, int x, int y) {
	if(ren == NULL || Surf_Src == NULL) {
        return false;
    }
 
	SDL_Texture *tex = SDL_CreateTextureFromSurface( ren, Surf_Src );
	
    SDL_Rect dst;
    dst.x = x;
    dst.y = y;
	SDL_QueryTexture(tex, NULL, NULL, &dst.w, &dst.h);
    SDL_RenderCopy(ren, tex, NULL, &dst);
	SDL_DestroyTexture(tex);
    return true;
}

bool CSurface::OnDraw(SDL_Renderer* ren, SDL_Texture* tex, SDL_Surface* Surf_Src, int x, int y) {
	SDL_Rect dst;
    dst.x = x;
    dst.y = y;
	SDL_QueryTexture(tex, NULL, NULL, &dst.w, &dst.h);
	SDL_UpdateTexture(tex, NULL, Surf_Src->pixels, Surf_Src->pitch);
    SDL_RenderCopy(ren, tex, NULL, &dst);
    return true;
}

bool CSurface::OnDraw(SDL_Renderer* ren, SDL_Surface* Surf_Src, int x, int y, int x2, int y2, int w, int h) {
	if(ren == NULL || Surf_Src == NULL) {
        return false;
    }
 
	SDL_Texture *tex = SDL_CreateTextureFromSurface( ren, Surf_Src );
	SDL_Rect src;
	src.x = x2;
	src.y = y2;
	src.w = w;
	src.h = h;
    SDL_Rect dst;
    dst.x = x;
    dst.y = y;
	SDL_QueryTexture(tex, NULL, NULL, &dst.w, &dst.h);
    SDL_RenderCopy(ren, tex, &src, &dst);
	SDL_DestroyTexture(tex);
    return true;
}


}
