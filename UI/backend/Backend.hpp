#ifndef NG_BACKEND_HPP
#define NG_BACKEND_HPP
#include "Screen.hpp"
#include "Input.hpp"
#include "Speaker.hpp"
#include "System.hpp"

namespace ng {
class Backend {
	public:
		Backend() : screen(0), speaker(0), system(0) {}
		Backend(Screen* screen, Speaker* speaker, System* system = 0) {
			this->screen = screen;
			this->speaker = speaker;
			this->system = system;
		}
		Screen* screen;
		Speaker* speaker;
		System* system;
};

}

#endif
