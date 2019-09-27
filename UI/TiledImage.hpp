#ifndef NG_TILED_IMAGE_HPP
#define NG_TILED_IMAGE_HPP
#include "Image.hpp"
#include <chrono>
namespace ng {

class TiledImage : public Image {
	private:
		int n_tiles;
		int tile_selected;
		Rect tile_rect;
		int tiles_per_row;
		Size tile_size;
		Image* img;
		void process_image();
		void process_tile();
		float animation_speed;
		bool animated;
		std::chrono::high_resolution_clock::time_point m_tp;
	protected:
		virtual void SetCache(uint32_t cache_id);
	public:
		TiledImage(std::string filename, Size tile_size);
		TiledImage(const TiledImage& t, int tile);
		TiledImage(Image* t, Size tile_size);
		virtual ~TiledImage();
		void SetTile(int n_tile);
		void SetTile(int row, int row_tile);
		virtual void SetAnimated(bool animated);
		virtual void SetAnimationSpeed(float delay_ms);
		
		virtual Size GetImageSize();
		virtual const unsigned int* GetImage();
		virtual const Rect GetImageCropRegion();
		
		virtual void Update(int ms=-1);
		
		virtual uint32_t GetTextureId();
		virtual void ResetAffectedRegion();
		
		virtual void FreeCache();
		virtual void Free();
		
		
};

}
#endif
