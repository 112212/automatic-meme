#include "Sound.hpp"
namespace ng {

Sound::~Sound() {}

void Sound::FreeCache() {}

void Sound::Free() {}

int	Sound::GetSoundSize() {}

const void* Sound::GetSound(uint32_t offset, uint32_t requested_size) {}

uint32_t Sound::GetSoundId() {}


int Sound::GetSoundChannels() {}

int Sound::GetSoundFrames() {}

}
