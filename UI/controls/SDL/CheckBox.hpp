#ifndef NG_CHECKBOX_HPP
#define NG_CHECKBOX_HPP

#include "../../Control.hpp"
namespace ng {


#define CHECKBOX_SIZE 15
#define CHECKBOX_OKVIR 5
#define CHECKBOX_SHIFT 10

class CheckBox : public Control {
	private:
		SDL_Surface* m_surf_text;
		Point m_text_loc;
		bool m_isChecked;
		TTF_Font* m_font;
		Uint32 tex_text;
		char* m_text;
		void onPositionChange();
		static int m_font_index;
		void updateText();
		void STYLE_FUNC(value);
		
	public:
		enum event {
			change
		};

		CheckBox();
		~CheckBox();
		
		CheckBox* Clone();
		void Render( Point pos, bool isSelected );
		void OnMouseDown( int mX, int mY );
		void OnMouseUp( int mX, int mY );
		void OnLostFocus();
		
		bool IsSelected();
		void SetText( const char* text );
		void Uncheck();
		void Check();
		void SetValue( bool check );
		
		bool IsChecked() { return m_isChecked; }
};
}
#endif
