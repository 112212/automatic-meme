#ifndef NG_X11_SPEAKER_HPP
#define NG_X11_SPEAKER_HPP
#include <X11/Xlib.h>

#include "../Speaker.hpp"

namespace ng {
namespace xcbBackend {
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
