#ifndef NG_FORM_HPP
#define NG_FORM_HPP
#include "../Widget.hpp"
namespace ng {
class Form : public Widget {
	public:
		Form() {
			initEventVector(1);
			setInterceptMask(imask::key_down);
		}
		enum event {
			submit
		};
	
	private:
		void OnKeyDown( SDL_Keycode &sym, SDL_Keymod mod ) {
			if(sym == SDLK_RETURN || sym == SDLK_KP_ENTER) {
				emitEvent(event::submit);
				intercept();
			}
		}
};
}
#endif
