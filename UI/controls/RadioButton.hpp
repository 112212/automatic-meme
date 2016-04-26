#ifndef NG_RADIOBUTTON_HPP
#define NG_RADIOBUTTON_HPP

#include "../Control.hpp"
#ifdef USE_SDL
	#include "SDL/RadioButton.hpp"
#elif USE_SFML
namespace ng {
enum {
	EVENT_RADIOBUTTON_CHANGE
};

#define RADIO_BUTTON_RADIUS 7

class RadioButton : public Control {
	private:
		sf::Text text;
		sf::CircleShape radio;
		sf::CircleShape active;
		
		Point m_text_loc;
		bool m_is_mouseDown;
		bool m_isSelected;
		int m_group;
		
		const char* ctext;
		void onPositionChange();
		void handleRadioButtonChange();
		
		
	public:
		RadioButton();
		~RadioButton();
		
		void Render( sf::RenderTarget& ren, sf::RenderStates state, bool isSelected );
		void OnMouseDown( int mX, int mY );
		void OnMouseUp( int mX, int mY );
		void OnLostFocus();
		
		bool IsSelected() { return m_isSelected; }

		void OnSetStyle(std::string& style, std::string& value);

		void SetText( const char* text );
		void SetGroup( int group ) { m_group = group; }
		void Unselect() { m_isSelected = false; }
		void Select();
		int GetGroup() { return m_group; }
};
}
#endif
#endif
