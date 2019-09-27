#include <vector>
#include <string>

#include "../File.hpp"
#include "../Image.hpp"

namespace ng {
	
	namespace Image_libbmp {
		// void load_png(File* file, std::vector<SDL_Surface*>& out_vector);
		// void load_bmp(File* file, Image* img);
		Resource* LoadBMP(File* file);
	}
	
}
