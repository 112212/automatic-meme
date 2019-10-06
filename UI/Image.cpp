#include "Image.hpp"
#include <cstring>
#include <algorithm>
#include <iostream>
#include "backend/Screen.hpp"
namespace ng {
	
Image::Image() {
	screen = 0;
	// std::cout << "NEW 1 IMG\n";
	screen_cache_id = NO_TEXTURE;
}

Image::Image(int w, int h) {
	screen = 0;
	screen_cache_id = NO_TEXTURE;
	// std::cout << "NEW 2 IMG\n";
}

Image::Image(unsigned int* buffer, int w, int h) {
	screen = 0;
	screen_cache_id = NO_TEXTURE;
}

Image::Image(Image&& tex) {
	this->screen = tex.screen;
	this->screen_cache_id = tex.screen_cache_id;
	tex.screen = 0;
	tex.screen_cache_id = 0;
}

Image::~Image() {
	Free();
}


void Image::PutImage(Image* img, Rect dstRegion, Rect srcRegion, uint32_t background_key, uint32_t bg_mask_check, uint32_t fg_color) {
	Size s = img->GetImageSize();
	const unsigned int* pixels = img->GetImage();
}


Size Image::GetImageSize() {
	return Size(0,0);
}

const unsigned int* Image::GetImage() {
	return 0;
}

uint32_t Image::GetTextureId() {
	return screen_cache_id;
}

void Image::Update(int ms) {
	
}

void Image::SetCache(uint32_t cache_id) {
	screen_cache_id = cache_id;
}

bool Image::GetAffectedRegion(Point& a, Point &b) {
	a = c1;
	b = c2;
	return c1.x != -1 && c1.x != c2.x && c1.y != c2.y;
}

void Image::Free() {
	FreeCache();
}

void Image::FreeCache() {
	if(screen_cache_id != NO_TEXTURE && screen) {
		screen->RemoveFromCache(screen_cache_id);
		screen_cache_id = NO_TEXTURE;
		SetFullAffectedRegion();
	}
}

const Rect Image::GetImageCropRegion() {
	Size s = GetImageSize();
	return Rect(0,0,s.w,s.h);
}

void Image::SetTile(int tile_num) {
	
}

Screen* Image::getScreen() {
	return screen;
}

void Image::SetAnimated(bool animated) {
	
}

void Image::SetAnimationSpeed(float delay_ms) {
	
}

void Image::UpdateAffectedRegion(const Point& p) {
	// p.x = clip(p.x, 0, size.w-1);
	// p.y = clip(p.y, 0, size.h-1);
	if(c1.x == -1 or c2.x == -1) {
		c1 = c2 = p;
		if(d1.x == -1) {
			d1 = d2 = p;
		}
	} else {
		c1.x = std::min(p.x, c1.x);
		d1.x = std::min(d1.x, c1.x);
		
		c1.y = std::min(p.y, c1.y);
		d1.y = std::min(d1.y, c1.y);
		
		c2.x = std::max(p.x, c2.x);
		d2.x = std::max(d2.x, c2.x);
		
		c2.y = std::max(p.y, c2.y);
		d2.y = std::max(c2.y, d2.y);
	}
	// std::cout << "upd: " << c1 << c2 <<"\n";
}

void Image::ResetAffectedRegion() {
	c1 = c2 = Point(-1,-1);
}

void Image::ResetDirtyRegion() {
	d1 = d2 = Point(-1,-1);
}

bool Image::GetDirtyRegion(Point& a, Point &b) {
	a = d1;
	b = d2;
	return a.x != -1 && !(a == b);
}

void Image::SetFullAffectedRegion() {
	Size s = GetImageSize();
	// std::cout << "set aff\n";
	ResetAffectedRegion();
	UpdateAffectedRegion(Point(0,0));
	UpdateAffectedRegion(Point(s.w,s.h));
}



}
