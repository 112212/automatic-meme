#ifndef SDL_SURFACE_IMAGE_HPP
#define SDL_SURFACE_IMAGE_HPP

#include <SDL2/SDL.h>
#include <vector>
#include <chrono>

#include "../../Image.hpp"

namespace ng {
	
class SDL_Surface_Image : public Image {
	private:
		SDL_Surface* surf;
		Size s;
		int cur_frame;
		std::vector<int> caches;
		std::chrono::high_resolution_clock::time_point m_tp;
		std::vector<SDL_Surface*> frames;
		static void load_png(std::string filename, std::vector<SDL_Surface*> &out_vector);
		void SetCache(uint32_t cache_id);
	public:
		SDL_Surface_Image( SDL_Surface* surf );
		SDL_Surface_Image( const std::vector<SDL_Surface*>& surfs );
		SDL_Surface_Image( std::string image_filename );
		virtual const unsigned int* GetImage();
		virtual Size GetImageSize();
		virtual ~SDL_Surface_Image();
		virtual void PutImage(Image* img, Rect_t dstRegion, Rect_t srcRegion);
		virtual void Update(int ms=-1);
		virtual uint32_t GetTextureId();
		virtual void FreeCache();
		
		static Image* LoadImage(File file);
		static Image* LoadPNG(File file);
		
};

}
#endif
