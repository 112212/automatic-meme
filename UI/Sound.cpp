#include "Sound.hpp"
namespace ng {

Sound::~Sound() {}

void Sound::FreeCache() {}

void Sound::Free() {}

int	Sound::GetSoundSize() {return 0;}

const void* Sound::GetSound(uint32_t offset, uint32_t requested_size) {return 0;}

uint32_t Sound::GetSoundId() {return 0;}


int Sound::GetSoundChannels() {return 0;}

int Sound::GetSoundFrames() {return 0;}

}
