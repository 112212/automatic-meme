#ifndef NG_RESOURCE_MANAGER_HPP
#define NG_RESOURCE_MANAGER_HPP

#include "../Resource.hpp"
#include "../File.hpp"
#include <vector>
#include <utility>
#include <functional>
#include <map>

namespace rapidxml {
	template<typename ch> class xml_node;
	template<typename ch> class file;
}

namespace ng {

typedef std::pair<std::string, std::string> KeyValue;
typedef std::map<std::string, std::string> Kvp;
// typedef std::vector<std::string, std::string> Kvp;
typedef std::function<Resource*(File*, Kvp)> ResourceLoader;
typedef std::function<Resource*(File*)> ResourceLoader2;
std::string GetKv(const Kvp& kv, const std::string& str);
class ResourceManager {

	static std::function<File*(std::string)> get_file_function;
	static std::map<std::string, Resource*> resources;
	static std::map<std::string, ResourceLoader> resource_loaders;
	static std::map<std::string, std::string> registered_extensions;
	public:
		static void SetFSOpen(std::function<File*(std::string)> file_opener);
		static void RegisterResourceLoader(std::string name, ResourceLoader loader);
		static void RegisterResourceLoader(std::string name, ResourceLoader2 loader);
		static void RegisterExtension(std::string ext, std::string resource_loader);
		static Resource* GetResource(std::string name);
		static void PutResource(std::string name, Resource* res);
		static Resource* LoadResource(std::string filename, std::string name="", Kvp kvp={});
		static void ProcessResource(rapidxml::xml_node<char>* node);
};


}

#endif
