#ifndef NG_CHECKBOX_HPP
#define NG_CHECKBOX_HPP

#include "../../Control.hpp"
namespace ng {
enum {
	EVENT_CHECKBOX_CHANGE
};

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
		
		
	public:
		CheckBox();
		~CheckBox();
		
		void Render(  SDL_Rect pos, bool isSelected );
		void OnMouseDown( int mX, int mY );
		void OnMouseUp( int mX, int mY );
		void OnLostFocus();
		
		bool IsSelected() { return m_isChecked; }
		void SetText( const char* text );
		void OnSetStyle(std::string& style, std::string& value);
		void Uncheck() { m_isChecked = false; }
		void Check() { m_isChecked = true; emitEvent( EVENT_CHECKBOX_CHANGE ); }
		void SetValue( bool check ) { m_isChecked = check; }
		
		bool IsChecked(){ return m_isChecked; }
};
}
#endif
