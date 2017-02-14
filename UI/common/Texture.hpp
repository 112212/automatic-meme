#ifndef TEXTURE_HPP
#define TEXTURE_HPP
#include "common.hpp"

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
		Texture(){}
		Texture(int w, int h);
		Texture(unsigned int* buffer, int w, int h);
		
		void DrawLine(Point a, Point b, unsigned int color = 0);
		void DrawRect(Point a, Point b, unsigned int color = 0, bool fill = false);
		void DrawCircle(Point a, int radius);
		void DrawImage(Point dstPoint, int* buffer, int w, int h, Point srcPoint, Size srcRegion);
		void Clear(unsigned int color);
		
		Size GetTextureSize();
		unsigned int* GetTexture();
		void GetUpdateRegion(Point& a, Point &b);
		unsigned int GetTextureId();
		void SetTextureId(unsigned int texid);
};
}
#endif
