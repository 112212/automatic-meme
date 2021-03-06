#ifndef NG_RADIOBUTTON_HPP
#define NG_RADIOBUTTON_HPP

#include "../Control.hpp"
namespace ng {
	
class RadioButton : public Control {
	private:
		Image* m_surf_text;
		Image* tex_text;
		Point m_text_loc;
		bool m_is_mouseDown;
		bool m_isSelected;
		int m_group;
		int m_checksize;
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
