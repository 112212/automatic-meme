#ifndef NG_IMAGES_HPP
#define NG_IMAGES_HPP
// #include "Image.hpp"
#include <functional>
#include <map>
#include "../common.hpp"
namespace ng {
class Image;
// typedef std::function<Image*(File)> ImageConstructor;
class Images {
	private:
		// static std::map< std::string, Image* > loaded_images;
		// static std::map< std::string, ImageConstructor > registered_image_engines;
	public:
		// static bool ImageExists( std::string filename );
		static Image* GetImage(std::string filename);
		static Image* LoadImage( std::string filename );
		// static void RegisterImageExtension( std::string ext, ResourceLoader loader);
};

}

#endif

