#include <iostream>

#include "SDLSpeaker.hpp"

namespace ng {
	
	
static Uint8 *audio_pos; // global pointer to the audio buffer to be played
static Uint32 audio_len; // remaining length of the sample we have to play


// audio callback function
// here you have to copy the data of your audio buffer into the
// requesting audio buffer (stream)
// you should only copy as much as the requested length (len)
void SDLSpeaker::my_audio_callback(void *userdata, Uint8 *stream, int len) {
	
	SDLSpeaker* spkr = static_cast<SDLSpeaker*>(userdata);
	if(len > 0) {
		uint32_t wrote = spkr->writeData(len, stream);
	}
	
	
	// if (audio_len <= 2) {
		// SDL_PauseAudio(1);
		// return;
	// }
	
	// len = std::min(audio_len, len);
	
	// SDL_memcpy (stream, audio_pos, len); 					// simply copy from one buffer into the other
	
	
	// SDL_MixAudio(stream, audio_pos, len, SDL_MIX_MAXVOLUME);// mix from one buffer into another
	
	// audio_pos += len;
	// audio_len -= len;
	
	// if (audio_len <= 2) {
	
}

/*
void SDLSpeaker::PlaySound(Sound* sound) {
	audio_len = sound->GetSoundSize();
	// audio_len = sound->GetSoundFrames()*2;
	audio_pos = (unsigned char*)sound->GetSound(0,audio_len);
	SDL_PauseAudio(0);
}
*/


void SDLSpeaker::startPlaying() {
	// std::cout << "start playing\n";
	SDL_PauseAudio(0);
}
void SDLSpeaker::stopPlaying() {
	SDL_PauseAudio(1);
}


void SDLSpeaker::Init() {
	if(inited) return;
	inited = true;
	
	audio_pos = 0;
	audio_len = 0;
	SDL_memset(&audio_spec, 0, sizeof(audio_spec));
	audio_spec.userdata = this;
	// audio_spec.freq = 44100;
	audio_spec.freq = 44100;
	audio_spec.format = AUDIO_S16;
	audio_spec.channels = 2;
	audio_spec.samples = 4096;
	audio_spec.callback = my_audio_callback;

	SDL_OpenAudio(&audio_spec, NULL);
}

SDLSpeaker::SDLSpeaker() {
	inited = false;
}

}
