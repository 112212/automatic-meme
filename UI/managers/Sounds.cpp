#include "Sounds.hpp"

namespace ng {

std::map< std::string, Sound* > Sounds::loaded_sounds;
std::map< std::string, SoundConstructor > Sounds::registered_sound_engines;

Sound* Sounds::LoadSound(std::string filename) {
	std::string::size_type ext_pos = filename.rfind(".");
	if(ext_pos == std::string::npos) {
		return 0;
	}
	
	std::string extension = filename.substr(ext_pos);
		
	auto it = registered_sound_engines.find(extension);
	if(it != registered_sound_engines.end()) {
		File f;
		f.name = filename;
		f.stream = 0;
		Sound* s = (it->second)(f);
		if(s) {
			loaded_sounds[filename] = s;
			return s;
		} else {
			return 0;
		}
	} else {
		return 0;
	}
}

void Sounds::RegisterSoundExtension(std::string ext, SoundConstructor sound_constructor) {
	registered_sound_engines[ext] = sound_constructor;
}

Sound* Sounds::GetSound(std::string filename) {
	auto it = loaded_sounds.find(filename);
	if(it == loaded_sounds.end()) {
		return LoadSound(filename);
	} else {
		return it->second;
	}
}

}
