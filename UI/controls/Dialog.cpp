#include "Dialog.hpp"

namespace ng {

void Dialog::Render( sf::RenderTarget &ren, sf::RenderStates state, bool isSelected) {
	// sf::Text t;
	// t.setFont( Fonts::GetFont( "default" ) );
	// t.setCharacterSize( characterSize );
	// t.setPosition(100,50);
	// t.setStri("nikola");
	// ren.draw(t,state);
	
	RenderWidget(ren,state,isSelected);
}


Dialog::Dialog() : Widget() {
	setType(TYPE_DIALOG);
}

}
