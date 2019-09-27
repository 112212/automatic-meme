#ifndef NG_SOUNDS_HPP
#define NG_SOUNDS_HPP
// #include "Sound.hpp"
#include <map>
#include <functional>
#include "../common.hpp"

namespace ng {
class Sound;
// typedef std::function<Sound*(File)> SoundConstructor;
class Sounds {
	private:
		// static std::map< std::string, Sound* > loaded_sounds;
		// static std::map< std::string, SoundConstructor > registered_sound_engines;
	public:
		static Sound* LoadSound(std::string filename);
		static Sound* GetSound( std::string filename );
		// static void RegisterSoundExtension( std::string ext, SoundConstructor image_constructor);
};

}
#endif
