#ifndef NG_BASIC_IMAGE_HPP
#define NG_BASIC_IMAGE_HPP
#include "Image.hpp"
#include <cstring>
namespace ng {
class BasicImage : public Image {
	private:
		uint32_t *buffer;
		Size size;
		bool clipLine(Point &a, Point &b);
	public:
		BasicImage(int w, int h);
		BasicImage(Size s) : BasicImage(s.w, s.h) {}
		BasicImage(BasicImage&& b);
		~BasicImage();
		
		virtual void Free();
		
		virtual Size GetImageSize();
		virtual const unsigned int* GetImage();
		
		virtual void PutImage(Image* img, Rect dstRegion, Rect srcRegion, uint32_t background_key=0, uint32_t bg_mask_check=0x0ffffff, uint32_t fg_color=0);
		
		// easy blit
		template<typename T>
		BasicImage(const T* data, Size datasize) : BasicImage(datasize.w, datasize.h) {
			int len = datasize.w * datasize.h;
			for(int i = 0; i < len; i++) {
				buffer[i] = data[i];
			}
		}
		
		template<typename T>
		void Blit(const T* data, Size datasize) {
			for(int y=0; y < datasize.h; y++) {
				for(int x=0; x < datasize.w; x++) {
					const T& d = data[y*datasize.w + x];
					buffer[y * size.w + x] = d;
				}
			}
		}
		
		void BlitRows(const uint32_t** data, Size datasize) {
			for(int y=0; y < datasize.h; y++) {
				const uint32_t* r = data[y];
				memcpy(&buffer[y*datasize.w], r, datasize.w*sizeof(uint32_t));
			}
		}
		
		void Refresh();
		void Resize(int w, int h);
		void Pixel(Point a, unsigned int color);
		void DrawRect(Point a, Point b, unsigned int color, bool fill=false);
		void Text(std::string str, Point dst, Font *font, uint32_t color=0xffffffff);
		void Line(Point a, Point b, uint32_t color);
		void LineAA(Point a, Point b, uint32_t color);
		void DrawPoly(std::vector<Point> points, uint32_t color);
		void Clear(uint32_t color=0);
};
}
#endif
