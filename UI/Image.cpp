#include "Image.hpp"
#include <cstring>
#include <algorithm>
#include <iostream>
#include "backend/Screen.hpp"
namespace ng {
	
Image::Image() {
	screen = 0;
	screen_cache_id = NO_TEXTURE;
}

Image::Image(int w, int h) {
	screen = 0;
	screen_cache_id = NO_TEXTURE;
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


void Image::PutImage(Image* img, Rect dstRegion, Rect srcRegion, unsigned int background_key, unsigned int fg_color) {
	Size s = img->GetImageSize();
	const unsigned int* pixels = img->GetImage();
}


Size Image::GetImageSize() {
}

const unsigned int* Image::GetImage() {
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

void Image::FreeCache() {
	if(screen) {
		screen->RemoveFromCache(screen_cache_id);
		screen_cache_id = NO_TEXTURE;
	}
	SetFullAffectedRegion();
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
	if(c1.x == -1) {
		c1 = c2 = p;
	} else {
		c1.x = std::min(p.x, c1.x);
		c1.y = std::min(p.y, c1.y);
		c2.x = std::max(p.x, c2.x);
		c2.y = std::max(p.y, c2.y);
	}
}

void Image::ResetAffectedRegion() {
	c2.x = -1;
	c1 = c2;
	
}

void Image::SetFullAffectedRegion() {
	Size s = GetImageSize();
	ResetAffectedRegion();
	UpdateAffectedRegion(Point(0,0));
	UpdateAffectedRegion(Point(s.w,s.h));
}

void Image::Free() {
	FreeCache();
}
}
