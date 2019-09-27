#include <vector>
#include <string>

#include "png.h"
#include "../File.hpp"
// #include <libpng/png.h>
// #include <SDL2/SDL.h>
#include "../Image.hpp"

namespace ng {
	
	namespace Image_libpng {
		// void load_png(File* file, std::vector<SDL_Surface*>& out_vector);
		void load_png(File* file, std::vector<Image*>& out_vector);
		Resource* LoadPNG(File* file);
	}
	
}
