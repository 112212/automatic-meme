#ifndef SOUND_HPP
#define SOUND_HPP

#include <string>
#include "common.hpp"

namespace ng {
	
class Speaker;
class Sound {
	private:
		friend class Speaker;
		
		Speaker* speaker;
		unsigned int speaker_cache_id;
		
	public:
	
		virtual ~Sound();

		void FreeCache();
		virtual void Free();
		
		virtual int GetSoundSize();
		virtual int GetSoundChannels();
		virtual int GetSoundFrames();
		virtual const void* GetSound(uint32_t offset, uint32_t requested_size);
		
		uint32_t GetSoundId();
};

}
#endif
