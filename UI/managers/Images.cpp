#include "Images.hpp"
namespace ng {

std::map< std::string, Image* > Images::loaded_images;
std::map< std::string, ImageConstructor > Images::registered_image_engines;

Image* Images::LoadImage( std::string filename ) {
	std::string::size_type ext_pos = filename.rfind(".");
	if(ext_pos == std::string::npos) {
		return 0;
	}
	std::string extension = filename.substr(ext_pos);
		
	auto it = registered_image_engines.find(extension);
	if(it != registered_image_engines.end()) {
		File f;
		f.name = filename;
		f.stream = 0;
		return (it->second)(f);
	} else {
		return 0;
	}
}

void Images::RegisterImageExtension( std::string ext, ImageConstructor image_constructor) {
	registered_image_engines[ext] = image_constructor;
}

Image* Images::GetImage(std::string filename) {
	auto it = loaded_images.find(filename);
	if(it == loaded_images.end()) {
		return LoadImage(filename);
	} else {
		return it->second;
	}
}

}
