#ifndef NG_SOUNDLIBSND_HPP
#define NG_SOUNDLIBSND_HPP
#include <sndfile.h>
#include "../../Sound.hpp"
#include "../../File.hpp"

namespace ng {
class SoundLibSnd : public Sound {
	
	private:
		void * m_buffer;
		unsigned int length;
		SNDFILE* file;
		SF_INFO sfinfo;
		uint32_t read_offset;
		uint32_t read_length;
	public:
		SoundLibSnd(std::string filename);
		static Resource* LoadSound(File* file);
		virtual ~SoundLibSnd();

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
