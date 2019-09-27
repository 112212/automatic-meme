#ifndef NG_IMAGE_HPP
#define NG_IMAGE_HPP
#include "common.hpp"
#include "Resource.hpp"
#include <string>

#ifndef NO_TEXTURE
	#define NO_TEXTURE 0xffffffffU
#endif

namespace ng {
class Font;
class Screen;
class Image : public Resource {
	private:
		friend class Screen;
		
		Point c1,c2;
		Point d1,d2;
		Screen* screen;
		unsigned int screen_cache_id;
		
	protected:
		// affected regions
		void UpdateAffectedRegion(const Point& p);
		void UpdateDirtyRegion(const Point& p);
		void SetFullAffectedRegion();
		Screen* getScreen();
	public:
		Image();
		Image(int w, int h);
		Image(unsigned int* buffer, int w, int h);
		Image(Image&& tex);
		virtual ~Image();
		
		// image manipulation (probably unnecessary)
		// virtual void Resize(int w, int h);
		// virtual void Pixel(Point a, unsigned int color = 0);
		// virtual void Line(Point a, Point b, unsigned int color = 0);
		// virtual void DrawRect(Point a, Point b, unsigned int color = 0, bool fill = false);
		// virtual void Circle(Point a, int radius);
		// virtual void Text(Font* font, std::string text_line, int x, int y);
		// virtual void Clear(unsigned int color);
		//
		
		virtual void PutImage(Image* img, Rect dstRegion, Rect srcRegion, uint32_t background_key=0, uint32_t bg_mask_check=0x00ffffff, uint32_t fg_color=0);
		
		virtual void Update(int ms=-1);
		virtual void SetTile(int tile_num);
		virtual void SetAnimated(bool animated);
		virtual void SetAnimationSpeed(float delay_ms);
		
		virtual void SetCache(uint32_t cache_id);
		virtual void FreeCache();
		virtual void Free();
		
		virtual Size GetImageSize();
		virtual const unsigned int* GetImage();
		virtual const Rect GetImageCropRegion();
		
		bool GetAffectedRegion(Point& a, Point &b);
		bool GetDirtyRegion(Point& a, Point &b);
		virtual void ResetAffectedRegion();
		virtual void ResetDirtyRegion();
		
		virtual uint32_t GetTextureId();
};
}
#endif
