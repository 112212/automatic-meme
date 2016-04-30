#ifndef NG_DIALOG
#define NG_DIALOG

#include "../Widget.hpp"
namespace ng {
class Dialog : public Widget {
	
	private:
		void Render( sf::RenderTarget &ren, sf::RenderStates state, bool isSelected);
	public:
		Dialog();
};
}
#endif
