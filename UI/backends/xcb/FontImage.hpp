#include "../../Image.hpp"
namespace ng {
namespace xcbBackend {
class FontImage : public ng::Image {
	private:
		std::string str;
	public:
		FontImage(std::string s) {
			str=s;
		}
		std::string GetStr() { return str; }
};

}}
