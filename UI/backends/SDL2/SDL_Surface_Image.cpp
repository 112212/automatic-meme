#include "config.h"

#ifdef USE_SDL2_image
#include <SDL2/SDL_image.h>
#endif

#include <iostream>
#include <chrono>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef USE_LIBPNG
#include "../../extensions/Image_libpng.hpp"
#endif

#include "SDL_Surface_Image.hpp"
#include "../../backend/Screen.hpp"
#include "RWOpsFromFile.hpp"


namespace ng {


static SDL_Surface* get_sdl_surface_from_image(Image* img) {
	Size s = img->GetImageSize();
	const unsigned int* pixels = img->GetImage();
	SDL_Surface* tmp_surf = SDL_CreateRGBSurfaceFrom((void*)pixels,
        s.w,s.h,32,4*s.w,0x00ff0000,0x0000ff00,0x000000ff,0xff000000);
	return tmp_surf;
}

SDL_Surface_Image::SDL_Surface_Image( SDL_Surface* surf ) : Image() {
	cur_frame = 0;
	this->surf = surf;
	s.w = surf->w;
	s.h = surf->h;
	if(!this->surf) {
		// std::cout << "FAIL creating font text\n";
	}
	SetFullAffectedRegion();
}


SDL_Surface_Image::SDL_Surface_Image( const std::vector<SDL_Surface*>& surfs ) {
	surf = 0;
	s.w = 0;
	s.h = 0;
	frames = surfs;
	cur_frame = 0;
	if(!frames.empty()) {
		s.w = frames[0]->w;
		s.h = frames[0]->h;
		caches.resize(frames.size());
		for(int& i : caches) {
			i=NO_TEXTURE;
		}
	}
	SetFullAffectedRegion();
	m_tp = std::chrono::high_resolution_clock::now();
}

SDL_Surface_Image::SDL_Surface_Image( const std::vector<Image*>& images ) {
	surf = 0;
	s.w = 0;
	s.h = 0;
	std::vector<SDL_Surface*> surfs;
	surfs.resize(images.size());
	for(int i=0; i < images.size(); i++) {
		
		surfs[i] = get_sdl_surface_from_image(images[i]);
	}
	frames = surfs;
	cur_frame = 0;
	if(!frames.empty()) {
		s.w = frames[0]->w;
		s.h = frames[0]->h;
		caches.resize(frames.size());
		for(int& i : caches) {
			i = NO_TEXTURE;
		}
	}
	SetFullAffectedRegion();
	m_tp = std::chrono::high_resolution_clock::now();
}

Resource* SDL_Surface_Image::LoadBMP(File* file) {
	SDL_Surface* bmp = SDL_LoadBMP_RW(GetRWOps(file), 1);
	SDL_PixelFormat pfm = getPixelFormat();
	SDL_Surface* bmp1 = SDL_ConvertSurface(bmp,&pfm,0);
	SDL_FreeSurface(bmp);
	return new SDL_Surface_Image(bmp1);
}

SDL_PixelFormat SDL_Surface_Image::getPixelFormat() {
	SDL_PixelFormat pfm;
	pfm.format = SDL_PIXELFORMAT_BGRA8888;
	pfm.palette = 0;
	pfm.BitsPerPixel = 32;
	pfm.BytesPerPixel = 4;
	pfm.Rmask = 0x00ff0000;
	pfm.Gmask = 0x0000ff00;
	pfm.Bmask = 0x000000ff;
	pfm.Amask = 0xff000000;
	return pfm;
}

Resource* SDL_Surface_Image::LoadIMG(File* file) {
#ifdef USE_SDL2_image
	SDL_Surface* surf2 = IMG_Load_RW(GetRWOps(file), 1);
	SDL_PixelFormat pfm = getPixelFormat();
	return new SDL_Surface_Image( SDL_ConvertSurface(surf2, &pfm, 0) );
#else
	return 0;
#endif
}

Resource* SDL_Surface_Image::LoadPNG(File* file) {
#ifdef USE_LIBPNG
	// if supports Image_libpng addon
	std::vector<Image*> vect;
	Image_libpng::load_png(file, vect);
	if(vect.empty()) {
		return 0;
	}
	return new SDL_Surface_Image(vect);
#else
	return LoadIMG(file);
#endif
}

// std::chrono::high_resolution_clock::time_point tp = std::chrono::high_resolution_clock::now();
void SDL_Surface_Image::Update(int ms) {
	if(frames.empty()) return;
	int speed = 20;
	if(ms == -1) {
		std::chrono::duration<float, std::milli> d = std::chrono::high_resolution_clock::now() - m_tp;
			
		if(d > std::chrono::milliseconds(speed)) {
			// int ms = std::chrono::duration_cast<std::chrono::milliseconds>(d).count();
			ms = (int)d.count();
			m_tp = std::chrono::high_resolution_clock::now();
		}
	}
	if(ms > 0) {
		int old_frame = cur_frame;
		cur_frame = (cur_frame + (ms / speed)) % frames.size();
		// std::cout << "update " << cur_frame << "\n";
		if(old_frame != cur_frame) {
			if(caches[cur_frame] == NO_TEXTURE) {
				// std::cout << "update\n";
				SetFullAffectedRegion();
			}
		}
	}
}

const unsigned int* SDL_Surface_Image::GetImage() {
	if(!surf) {
		// std::cout << "getting frame\n";
		if(!frames.empty()) {
			return (unsigned int*)frames[cur_frame]->pixels;
		} else {
			return 0;
		}
	} else {
		return (unsigned int*)surf->pixels;
	}
}

SDL_Surface_Image::~SDL_Surface_Image() {
	if(surf) {
		Free();
		SDL_FreeSurface(surf);
	}
}

Size SDL_Surface_Image::GetImageSize() {
	return s;
}

uint32_t SDL_Surface_Image::GetTextureId() {
	if(surf) {
		return Image::GetTextureId();
	} else {
		return caches[cur_frame];
	}
}

void SDL_Surface_Image::SetCache(uint32_t cache_id) {
	if(!caches.empty()) {
		caches[cur_frame] = cache_id;
	} else {
		Image::SetCache(cache_id);
	}
}

void SDL_Surface_Image::FreeCache() {
	if(surf) {
		Image::FreeCache();
	} else {
		getScreen()->RemoveFromCache(caches[cur_frame]);
	}
}


void SDL_Surface_Image::PutImage(Image* img, Rect dstRegion, Rect srcRegion) {
	Size s = img->GetImageSize();
	const unsigned int* pixels = img->GetImage();
	SDL_Surface* tmp_surf = SDL_CreateRGBSurfaceFrom((void*)pixels,
        s.w,s.h,32,4*s.w,0x00ff0000,0x0000ff00,0x000000ff,0xff000000);
	
	SDL_Rect src_rect;
	src_rect.x = srcRegion.x;
	src_rect.y = srcRegion.y;
	src_rect.w = srcRegion.w;
	src_rect.h = srcRegion.h;
	
	SDL_Rect dst_rect;
	dst_rect.x = dstRegion.x;
	dst_rect.y = dstRegion.y;
	dst_rect.w = dstRegion.w;
	dst_rect.h = dstRegion.h;
	
	SDL_BlitSurface(tmp_surf, &src_rect, this->surf, &dst_rect);
}

}

