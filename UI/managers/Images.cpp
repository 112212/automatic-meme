#include "Images.hpp"
#include "ResourceManager.hpp"
namespace ng {


Image* Images::LoadImage( std::string filename ) {
	return (Image*)ResourceManager::LoadResource(filename, filename);
}

Image* Images::GetImage(std::string name) {
	return (Image*)ResourceManager::LoadResource(name);
}

}
