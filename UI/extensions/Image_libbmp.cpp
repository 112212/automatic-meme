#include "Image_libbmp.hpp"
#include "../BasicImage.hpp"
#include "bmpread/bmpread.h"
#include <iostream>
#include "../Color.hpp"
namespace ng {
namespace Image_libbmp {

Resource* LoadBMP(File* file) {
	bmpread_t img;
	bmpread(file->path.c_str(), BMPREAD_ANY_SIZE | BMPREAD_BYTE_ALIGN | BMPREAD_TOP_DOWN, &img);
	
	Size s = {img.width, img.height};
	std::cout << "loaded img : " << s << "\n";
	uint32_t *pixels = new uint32_t[s.w*s.h];
	for(int i=0; i < s.h; i++) {
		for(int j=0; j < s.w; j++) {
			int a = (i*img.width + j)*3;
			pixels[i*s.w + j] = Color(img.data[a + 0], img.data[a+1], img.data[a+2]).GetUint32();
		}
	}
	
	BasicImage* bi = new BasicImage(pixels, s);
	// bi->Blit(pixels, s);
	delete[] pixels;
	return bi;
}

}

}
