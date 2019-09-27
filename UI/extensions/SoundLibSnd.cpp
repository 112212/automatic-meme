#include <iostream>
#include <cstring>
#include "SoundLibSnd.hpp"
namespace ng {
SoundLibSnd::SoundLibSnd(std::string filename) {
	memset(&sfinfo, 0, sizeof(SF_INFO));
	sfinfo.samplerate = 44100;
	file = sf_open(filename.c_str(), SFM_READ, &sfinfo);
	if(!file) {
		std::cout << "Error: cannot open file " << filename << "\n";
		exit(-1);
	}
	
	std::cout << "opened file " << sfinfo.samplerate << " chnl: " << sfinfo.channels << "\n";
	/*
	
	  typedef struct
      {    sf_count_t  frames ;     // Used to be called samples.
           int         samplerate ;
           int         channels ;
           int         format ;
           int         sections ;
           int         seekable ;
       } SF_INFO ;
       
     */
	read_offset = 0;
	read_length = 0;
	m_buffer = 0;
	
	if(file) {
		m_buffer = malloc(sizeof(int16_t) * sfinfo.channels * sfinfo.frames);
		sf_readf_short(file, (int16_t*)m_buffer, sfinfo.frames);
	}
}

Resource* SoundLibSnd::LoadSound(File* file) {
	// TODO: implement
	return 0;
	// std::cout << "loading filename: " << file.name << "\n";
	return new SoundLibSnd("");
}

SoundLibSnd::~SoundLibSnd() {
	sf_close(file);
}

void SoundLibSnd::FreeCache() {
	
}

void SoundLibSnd::Free() {
	
}

/*
*/
int SoundLibSnd::GetSoundSize() {
	std::cout << "sound size: " << (sfinfo.channels * sfinfo.frames) << "\n";
	return sfinfo.channels * sfinfo.frames;
}

int SoundLibSnd::GetSoundChannels() {
	return sfinfo.channels;
}

int SoundLibSnd::GetSoundFrames() {
	return sfinfo.frames;
}

/*
	req format
		short
*/
const void* SoundLibSnd::GetSound(uint32_t offset_frame, uint32_t requested_frames) {
	// std::cout << "loading m_buffer " << offset_frame << " , " << requested_frames << "\n";
	/*
	if(offset_frame < read_offset || (offset_frame+requested_frames) > (read_offset+read_length)) {
		sf_seek(file, offset_frame, SEEK_SET);
		requested_frames += 4096;
		if(requested_frames > read_length) {
			if(m_buffer) {
				free(m_buffer);
			}
			m_buffer = malloc(sizeof(int16_t) * sfinfo.channels * requested_frames);
		}
		
		sf_count_t read_count = sf_readf_short(file, (int16_t*)m_buffer, requested_frames);
		// std::cout << "read count: " << read_count << "\n";
		read_offset = offset_frame;
		read_length = requested_frames;
		// std::cout << "loading m_buffer\n";
	}
	// std::cout << "loading m_buffer2\n";
	return ((int16_t*)m_buffer + (offset_frame-read_offset));
	*/
	return ((int16_t*)m_buffer + offset_frame*2);
}

uint32_t SoundLibSnd::GetSoundId() {
	
}
}
