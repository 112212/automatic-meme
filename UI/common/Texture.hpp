#ifndef TEXTURE_HPP
#define TEXTURE_HPP
#include "common.hpp"

#ifndef NO_TEXTURE
#define NO_TEXTURE 0xffffffffU
#endif
namespace ng {
class Texture {
	private:
		Size size;
		
		unsigned int tex_id;
		
		// ARGB format
		unsigned int *buffer;
		
		// affected regions
		Point c1,c2;
	public:
		Texture();
		Texture(int w, int h);
		Texture(unsigned int* buffer, int w, int h);
		Texture(Texture&& tex);
		~Texture();
		
		void Resize(int w, int h);
		void Pixel(Point a, unsigned int color = 0);
		void Line(Point a, Point b, unsigned int color = 0);
		void Rect(Point a, Point b, unsigned int color = 0, bool fill = false);
		void Circle(Point a, int radius);
		void Image(Point dstPoint, int* buffer, int w, int h, Point srcPoint, Size srcRegion);
		void Clear(unsigned int color);
		
		
		Size& GetTextureSize();
		const unsigned int* GetTexture();
		bool GetUpdateRegion(Point& a, Point &b);
		unsigned int GetTextureId();
		void SetTextureId(unsigned int texid);
};
}
#endif
