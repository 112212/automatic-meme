#ifndef NG_SDL_SPEAKER_HPP
#define NG_SDL_SPEAKER_HPP
#include <SDL.h>

#include "../../backend/Speaker.hpp"

namespace ng {

class SDLSpeaker : public Speaker {
	private:
		SDL_AudioSpec audio_spec;
		bool inited;
		
		virtual void startPlaying();
		virtual void stopPlaying();
		static void my_audio_callback(void *userdata, uint8_t *stream, int len);
	public:
		SDLSpeaker();
		virtual void Init();
};

}
#endif
