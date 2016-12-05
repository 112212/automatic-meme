#include "Dialog.hpp"

namespace ng {

void Dialog::Render( sf::RenderTarget &ren, sf::RenderStates state, bool isSelected) {
	RenderWidget(ren,state,isSelected);
}


Dialog::Dialog() : Widget() {
	setType(TYPE_DIALOG);
}

}
