#include <algorithm>
#include <cstring>
#include <iostream>
#include "Speaker.hpp"
namespace ng {

	
	void Speaker::Init() {}
	void Speaker::startPlaying() {}
	void Speaker::stopPlaying() {}
	uint32_t Speaker::canAcceptData() { return 0; }
	
	void Speaker::PlaySound(Sound* sound, bool loop) {
		if(!sound) return;
		
		// std::cout << "play sound " << sound << "\n";
		SoundData sd;
		sd.sound = sound;
		sd.looping = loop;
		// sd.looping = true;
		sd.sound_size = sound->GetSoundSize();
		sd.frames = sound->GetSoundFrames();
		sd.cursor = 0;
		sd.volume = 1.0;
		sounds.push_back(sd);
		
		Process();
	}
	
	void Speaker::StopSound(Sound* sound) {
		std::remove_if(sounds.begin(), sounds.end(), [&](SoundData& d) {
			return d.sound == sound;
		});
		
		Process();
	}
	
	void Speaker::StopAllSound() {
		sounds.clear();
		
		Process();
	}
	
	// size is in bytes
	uint32_t Speaker::writeData(uint32_t size, uint8_t* buffer) {
		// TODO: add support for more sounds
		memset(buffer, 0, (size/4)*4);
		bool should_play = false;
		double volume = 0;
		for(SoundData& s : sounds) {
			volume += s.volume;
		}
		
		for(SoundData& s : sounds) {
			// uint32_t sound_size = s.sound->GetSoundSize();
			uint32_t req_words = size/(sizeof(int16_t) * 2/*channels*/);
			uint32_t frames_request = std::min(s.frames - s.cursor, req_words);
			if(frames_request > 0) {
				uint8_t* data = (uint8_t*)s.sound->GetSound(s.cursor, frames_request);
				
				// std::cout << "requested: " << can_request << ", " << s.sound_size << ", " << sounds.size() << "\n";
				// memcpy(buffer, data, can_request);
				
				// mix audio
				int16_t* frames = (int16_t*)data;
				int16_t* out = (int16_t*)buffer;
				uint32_t frames_len = frames_request*2;
				for(uint32_t i=0; i < frames_len; i++) {
					out[i] = out[i] + frames[i] * s.volume / volume;
					
					// out[i] = clip<int16_t>(out[i] + frames[i] * s.volume, -32767, 32767);
					// out[i] += frames[i];
				}
				
				s.cursor += frames_request;
				if(s.cursor == s.frames) {
					if(s.looping) {
						s.cursor = 0;
					}
				} else {
					should_play = true;
				}
			}
			
			// return can_request;
		}
		
		// if(!should_play) {
			// stopPlaying();
		// }
		return 0;
	}
	
	void Speaker::Process() {
		
		for(auto it = sounds.begin(); it != sounds.end(); ) {
			if(it->cursor == it->frames) {
				if(it->looping) {
					it->cursor = 0;
				} else {
					it=sounds.erase(it);
					continue;
				}
			}
			it++;
		}
		
		// std::cout << "proc audio\n";
		
		// uint32_t frames = canAcceptData();
		// if(frames > 0) {
			// uint32_t written = writeData(frames, buffer.data());
		// }
		
		
		if(!sounds.empty()) {
			startPlaying();
		} else {
			stopPlaying();
		}
	}
	
	

}
