#ifndef NG_FORM_HPP
#define NG_FORM_HPP
#include "../Widget.hpp"
namespace ng {
class Form : public Widget {
	public:
		Form() {
			setType( "form" );
			setInterceptMask(imask::key_down);
		}

	private:
		void OnKeyDown( SDL_Keycode &sym, SDL_Keymod mod ) {
			if(sym == SDLK_RETURN || sym == SDLK_KP_ENTER) {
				emitEvent("submit");
				intercept();
			}
		}
};
}
#endif
