#ifndef SDL_NG_RADIOBUTTON_HPP
#define SDL_NG_RADIOBUTTON_HPP

#include "../../Control.hpp"
namespace ng {


#define RADIO_BUTTON_RADIUS 14

class RadioButton : public Control {
	private:
		SDL_Surface* m_surf_text;
		Uint32 tex_text;
		Point m_text_loc;
		bool m_is_mouseDown;
		bool m_isSelected;
		int m_group;
		TTF_Font* m_font;
		std::string m_text;
		void onRectChange();
		
		void handleRadioButtonChange();
		
		void Render( Point pos, bool isSelected );
		void OnMouseDown( int mX, int mY, MouseButton button );
		void OnMouseUp( int mX, int mY, MouseButton button );
		void OnLostFocus();
		void OnSetStyle(std::string& style, std::string& value);
	public:
	
		RadioButton();
		~RadioButton();
		
		bool IsSelected() { return m_isSelected; }

		RadioButton* Clone();
		void SetText( std::string text );
		void SetGroup( int group ) { m_group = group; }
		void Unselect() { m_isSelected = false; }
		void Select();
		int GetGroup() { return m_group; }
};

}
#endif
