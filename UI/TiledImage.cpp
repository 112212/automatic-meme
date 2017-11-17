#include "TiledImage.hpp"
#include <chrono>
#include <iostream>
namespace ng {
	
TiledImage::TiledImage(std::string filename, Size tile_size) {
	
}

TiledImage::TiledImage(const TiledImage& t, int tile) {
	this->n_tiles = t.n_tiles;
	this->tile_selected = t.tile_selected;
	this->tile_rect = t.tile_rect;
	this->tiles_per_row = t.tiles_per_row;
	this->tile_size = t.tile_size;
	this->img = t.img;
	animation_speed = 20;
	process_image();
}

TiledImage::TiledImage(Image* t, Size tile_size) {
	this->tile_size = tile_size;
	this->img = t;
	tile_rect.w = tile_size.w;
	tile_rect.h = tile_size.h;
	tile_selected = 0;
	animation_speed = 20;
	process_image();
}

void TiledImage::process_tile() {
	if(tile_selected < n_tiles) {
		int n = tile_selected / tiles_per_row;
		tile_rect.x = (tile_selected - n * tiles_per_row) * tile_size.w;
		tile_rect.y = n * tile_size.h;
	}
}

void TiledImage::process_image() {
	Size s = img->GetImageSize();
	this->tiles_per_row = s.w / tile_size.w;
	this->n_tiles = (s.h / tile_size.h) * tiles_per_row;
	
	if(tile_selected < 0 || tile_selected > n_tiles) {
		tile_selected = 0;
	}
	
	process_tile();
	
	tile_rect.w = tile_size.w;
	tile_rect.h = tile_size.h;
	
	Point a,b;
	if(img->GetAffectedRegion(a,b)) {
		SetFullAffectedRegion();
	}
}

void TiledImage::SetTile(int n_tile) {
	if(n_tile < n_tiles) {
		tile_selected = n_tile;
		process_tile();
	}
}

void TiledImage::SetTile(int row, int row_tile) {
	int n_tile = (row * tiles_per_row + row_tile);
	if(n_tile < n_tiles) {
		tile_selected = n_tile;
		process_tile();
	}
}

void TiledImage::SetAnimated(bool animated) {
	this->animated = animated;
}
void TiledImage::SetAnimationSpeed(float delay_ms) {
	this->animation_speed = delay_ms;
}

void TiledImage::Update(int ms) {
	if(!animated) {
		return;
	}
	float speed = animation_speed;
	if(ms == -1) {
		std::chrono::duration<float, std::milli> d = std::chrono::high_resolution_clock::now() - m_tp;
		if(d > std::chrono::duration<float, std::milli>(speed)) {
			ms = (int)d.count();
			m_tp = std::chrono::high_resolution_clock::now();
		}
	}
	if(ms > 0) {
		tile_selected = (int)(tile_selected + (ms / speed)) % n_tiles;
		process_tile();
	}
}

Size TiledImage::GetImageSize() {
	Size s = img->GetImageSize();
	return s;
}

const unsigned int* TiledImage::GetImage() {
	return img->GetImage();
}

const Rect TiledImage::GetImageCropRegion() {
	return tile_rect;
}

void TiledImage::ResetAffectedRegion() {
	img->ResetAffectedRegion();
}

void TiledImage::SetCache(uint32_t cache_id) {
	img->SetCache(cache_id);
	
}

uint32_t TiledImage::GetTextureId() {
	return img->GetTextureId();
}

void TiledImage::FreeCache() {
	img->FreeCache();
}

void TiledImage::Free() {
	img->Free();
}

TiledImage::~TiledImage() {
	
}

}
