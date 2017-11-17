#ifndef BASIC_IMAGE_HPP
#define BASIC_IMAGE_HPP
#include "Image.hpp"
namespace ng {
class BasicImage : public Image {
	private:
		unsigned int *buffer;
		Size size;
	public:
		BasicImage(int w, int h);
		BasicImage(BasicImage&& b);
		~BasicImage();
		
		virtual void Free();
		
		virtual Size GetImageSize();
		virtual const unsigned int* GetImage();
		
		virtual void PutImage(Image* img, Rect_t dstRegion, Rect_t srcRegion, unsigned int background_key=0, unsigned int fg_color=0);
		
		
		void Resize(int w, int h);
		void Pixel(Point a, unsigned int color);
		void Rect(Point a, Point b, unsigned int color, bool fill);
		void Line(Point a, Point b, unsigned int color);
		void Clear(unsigned int color);
};
}
#endif
