#include "ResourceManager.hpp"
#include "FileIFStream.hpp"
#include <RapidXML/rapidxml.hpp>
// #include <RapidXML/rapidxml_utils.hpp>
#include <iostream>
#include <cstring>
namespace ng {
	
	static File* default_fs(std::string filename) {
		return new FileIFStream(new std::ifstream(filename));
	}
	
	std::function<File*(std::string)> ResourceManager::get_file_function = default_fs;
	std::map<std::string, Resource*> ResourceManager::resources;
	std::map<std::string, ResourceLoader> ResourceManager::resource_loaders __attribute__ ((init_priority (200))) ;
	std::map<std::string, std::string> ResourceManager::registered_extensions;
	
	
	std::string GetKv(const Kvp& kv, const std::string& str) {
		auto it = kv.find(str);
		if(it == kv.end()) {
			return "";
		} else {
			return it->second;
		}
	}
	
	void ResourceManager::SetFSOpen(std::function<File*(std::string)> file_opener ) {
		get_file_function = file_opener;
	}
	
	void ResourceManager::RegisterResourceLoader(std::string name, ResourceLoader loader) {
		// std::cout << "registering loader " << name << "\n";
		resource_loaders[name] = loader;
	}
	
	void ResourceManager::RegisterResourceLoader(std::string name, ResourceLoader2 loader) {
		resource_loaders[name] = [=](File* f, Kvp kv) {
			return loader(f);
		};
	}
	
	void ResourceManager::RegisterExtension(std::string ext, std::string resource_loader) {
		registered_extensions[ext] = resource_loader;
	}
	
	
	// template<typename A, typename B>
	// void* findPtr(A somemap, B somekey) {
		// auto it = somemap.find(somekey);
		// if(it != somemap.end()) {
			// return *it;
		// } else {
			// return 0;
		// }
	// }
	
	void ResourceManager::ProcessResource(rapidxml::xml_node<char>* node) {
		for(auto *n = node->first_node(); n; n = n->next_sibling()) {
			/*
			if(strcmp(n->name(), "image") == 0) {
				
			} else if(strcmp(node_name(), "sound") == 0) {
				
			} else if(strcmp(node_name,"font") == 0)) {
				
			}
			*/
			
			std::string file;
			std::string name;
			std::string engine;
			Kvp kvp;
			for(auto *a = n->first_attribute(); a; a=a->next_attribute()) {
				if(!strcmp(a->name(), "name")) {
					name = a->value();
				} else if(!strcmp(a->name(), "file")) {
					file = a->value();
				} else if(!strcmp(a->name(), "engine")) {
					engine = a->value();
					// std::cout << "engine = " << a->value() << "\n";
				} else {
					kvp[a->name()] = a->value();
				}
			}
			
			if(name.empty()) {
				name = file;
			}
			// if(!file.empty()) {
				
				if(engine.empty()) {
					LoadResource(file, name, kvp);
				} else {
					if(GetResource(name)) continue;
					auto it = resource_loaders.find(engine);
					if(it != resource_loaders.end()) {
						File* f = get_file_function(file);
						if(f) {
							f->path = file;
						}
						Resource* res = it->second(f, kvp);
						if(res) {
							resources[name] = res;
						}
					}
				}
			// }
		}
	}
	
	Resource* ResourceManager::GetResource(std::string name) {
		
		auto it = resources.find(name);
		if(it != resources.end()) {
			return it->second;
		} else {
			return 0;
		}
		
	}
	
	void ResourceManager::PutResource(std::string name, Resource* res) {
		resources[name] = res;
	}
	
	Resource* ResourceManager::LoadResource(std::string filename, std::string name, Kvp kvp) {
		if(name.empty()) {
			name = filename;
		}
		
		Resource* res = GetResource(name);
		if(res) {
			return res;
		}
		
		std::string::size_type ext_pos = filename.rfind(".");
		if(ext_pos == std::string::npos) {
			return 0;
		}
		std::string extension = filename.substr(ext_pos);
		std::cout << "loading resource: " << filename << " : " << extension << "\n";
			
		auto it = registered_extensions.find(extension);
		if(it != registered_extensions.end()) {
			File* f = get_file_function(filename);
			if(!f) {
				return 0;
			} else {
				f->path = filename;
			}
			
			// std::cout << "found res loader\n";
			auto it2 = resource_loaders.find(it->second);
			if(it2 != resource_loaders.end()) {
				res = it2->second(f, kvp);
				
				if(res) {
					resources[name] = res;
				} else {
					std::cout << "cannot load resource " << name << " (" << filename   << ")\n";
				}
				// std::cout << "LOADING OK\n";
				return res;
			}
		} else {
			std::cout << "cannot load resource " << name << " (" << filename   << ") - no registered extension\n";
		}
		return 0;
	}
		
}

