#ifndef SDLSOUND_HPP
#define SDLSOUND_HPP

#include "../../Sound.hpp"

namespace ng {
class SDLSound : public Sound {
	
	private:
		uint8_t * buffer;
		unsigned int length;
	protected:
		
	public:
		SDLSound(std::string filename);
		static Sound* LoadSound(File file);
		virtual ~SDLSound();

		void FreeCache();
		virtual void Free();
		
		virtual int GetSoundChannels();
		virtual int GetSoundFrames();
		virtual int GetSoundSize();
		virtual const void* GetSound(uint32_t offset, uint32_t requested_size);
		
		uint32_t GetSoundId();
	
};
}
#endif
