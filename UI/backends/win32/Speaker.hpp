#ifndef NG_WIN32_SPEAKER_HPP
#define NG_WIN32_SPEAKER_HPP

#include "../../backend/Speaker.hpp"

namespace ng {
namespace win32Backend {
class Speaker : public ng::Speaker {
	private:
		bool inited;
		
		virtual void startPlaying();
		virtual void stopPlaying();
		static void my_audio_callback(void *userdata, uint8_t *stream, int len);
	public:
		Speaker();
		virtual void Init();
};

}}
#endif
