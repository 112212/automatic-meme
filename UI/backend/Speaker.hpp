#ifndef _SOUND_HPP_
#define _SOUND_HPP_
#include <vector>
#include <string>
#include "../Sound.hpp"
namespace ng {
	
class Speaker {
	private:
		struct SoundData {
			Sound* sound;
			uint32_t channels;
			uint32_t sound_size;
			uint32_t frames;
			uint32_t cursor;
			double volume;
			bool looping;
		};
		std::vector<SoundData> sounds;
		std::vector<uint8_t> buffer;
	protected:
		
		virtual void startPlaying();
		virtual void stopPlaying();
		virtual uint32_t canAcceptData();
		uint32_t writeData(uint32_t size, uint8_t* buffer);
	public:
		virtual void Init();
		void PlaySound(Sound* sound, bool loop=false);
		void StopSound(Sound* sound);
		void StopAllSound();
		
		void Process();
};

}

#endif
