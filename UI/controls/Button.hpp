#ifndef NG_BUTTON_HPP
#define NG_BUTTON_HPP

#include "../Control.hpp"
namespace ng {

class Button : public Control {
	private:
		Image* tex_text;
		Rect m_text_rect;
		bool need_update;
		std::string text;
		Point text_loc;
		bool m_is_mouseDown;
		bool m_disabled;
		int m_down_color;
		int m_up_color;
		
		void onRectChange();
		void update_text();
		void OnSetStyle(std::string& style, std::string& value);
		void OnMouseDown( int mX, int mY, MouseButton which_button );
		void OnMouseUp( int mX, int mY, MouseButton which_button );
		void OnLostFocus();
		void OnGetFocus();
		void onFontChange();
		void OnKeyDown( Keycode sym, Keymod mod );
		void OnKeyUp( Keycode sym, Keymod mod );
		
		void Render( Point pos, bool isSelected );
	public:
		Button(std::string id="");
		~Button();
		
		enum event {
			click,
			max_events
		};
		
		std::string GetText() { return text; }
		void SetText( std::string text );
		
		Button* Clone();
		
		Rect GetContentRect();
};
}
#endif
