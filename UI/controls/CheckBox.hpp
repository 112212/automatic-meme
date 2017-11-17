#ifndef NG_CHECKBOX_HPP
#define NG_CHECKBOX_HPP

#include "../Control.hpp"
namespace ng {


#define CHECKBOX_SIZE 15
#define CHECKBOX_FRAME 5
#define CHECKBOX_SHIFT 2

class CheckBox : public Control {
	private:
		Point m_text_loc;
		bool m_isChecked;
		Font* m_font;
		Image* tex_text;
		std::string m_text;
		bool tmode;
		void onRectChange();
		static int m_font_index;
		void updateText();
		void OnSetStyle(std::string& style, std::string& value);
		
	public:

		CheckBox();
		~CheckBox();
		
		CheckBox* Clone();
		void Render( Point pos, bool isSelected );
		void OnMouseDown( int mX, int mY, MouseButton which_button);
		void OnMouseUp( int mX, int mY, MouseButton which_button );
		void OnLostFocus();
		
		bool IsSelected();
		bool IsChecked();
		void SetText( std::string text );
		void Uncheck();
		void Check();
		void SetValue( bool check );
		
};
}
#endif
