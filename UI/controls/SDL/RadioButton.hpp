#ifndef SDL_NG_RADIOBUTTON_HPP
#define SDL_NG_RADIOBUTTON_HPP

#include "../../Control.hpp"
namespace ng {
enum {
	EVENT_RADIOBUTTON_CHANGE
};

#define RADIO_BUTTON_RADIUS 7

class RadioButton : public Control {
	private:
		SDL_Surface* m_surf_text;
		Point m_text_loc;
		bool m_is_mouseDown;
		bool m_isSelected;
		int m_group;
		TTF_Font* m_font;
		std::string m_text;
		void onPositionChange();
		
		void handleRadioButtonChange();
		
		void Render(  SDL_Rect pos, bool isSelected );
		void OnMouseDown( int mX, int mY );
		void OnMouseUp( int mX, int mY );
		void OnLostFocus();
		void OnSetStyle(std::string& style, std::string& value);
	public:
		RadioButton();
		~RadioButton();
		
		bool IsSelected() { return m_isSelected; }

		
		void SetText( std::string text );
		void SetGroup( int group ) { m_group = group; }
		void Unselect() { m_isSelected = false; }
		void Select() { m_isSelected = true; handleRadioButtonChange(); emitEvent( EVENT_RADIOBUTTON_CHANGE ); }
		int GetGroup() { return m_group; }
};
}
#endif
