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
		void OnKeyDown( Keycode &sym, Keymod mod ) {
			if(sym == KEY_RETURN || sym == KEY_KP_ENTER) {
				emitEvent("submit");
				intercept();
			}
		}
};
}
#endif
