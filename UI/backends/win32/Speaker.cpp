#include <iostream>

#include "Speaker.hpp"

namespace ng {
namespace win32Backend {
	

// audio callback function
// here you have to copy the data of your audio buffer into the
// requesting audio buffer (stream)
// you should only copy as much as the requested length (len)
void Speaker::my_audio_callback(void *userdata, uint8_t *stream, int len) {
	
	
	
}



void Speaker::startPlaying() {
	
}
void Speaker::stopPlaying() {

}


void Speaker::Init() {
	if(inited) return;
	inited = true;
	
}

Speaker::Speaker() {
	inited = false;
}

}}
