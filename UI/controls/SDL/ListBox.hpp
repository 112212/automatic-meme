#ifndef NG_LISTBOX_HPP
#define NG_LISTBOX_HPP

#include "../../Control.hpp"
#include "../ScrollBar.hpp"
#include "TextBox.hpp"
namespace ng {

class ListBox : public Control {
	private:
		bool m_is_mouseDown;
		bool m_drawscrollbar;
		bool m_scrollbar_focus;
		
		void onRectChange();
		
		TTF_Font* m_font;
		
		std::vector<std::string> m_items;
		
		int m_selection_color;
		struct TextLine{
			Uint32 tex;
			int w,h;
		};
		
		std::vector<TextLine> text_lines;
		
		SDL_Rect m_scrollrect;
		ScrollBar *m_scrollbar; // scrollbar :)
		
		// min, max dropdown
		int m_max_items;
		int m_last_scroll;
		
		int m_selected_index;
		
		int m_scroll_pos;
		int m_font_height;
		
		bool isOnScrollbar( int &mX, int &mY );
		int getMaxText( std::string txt, int w );
		int getListOffset();
		void updateItemsSize();
		int getAverageHeight();
		std::string clipText( std::string s, int w );
		void updateBox();
		
		void Render( Point pos, bool isSelected );
		void OnMouseDown( int mX, int mY, MouseButton which_button );
		void OnMouseUp( int mX, int mY, MouseButton which_button );
		void OnLostFocus();
		virtual void OnGetFocus();
		virtual void OnMWheel( int updown );
		void OnLostControl();
		void OnMouseMove( int mX, int mY, bool mouseState );
		void OnSetStyle(std::string& style, std::string& value);
		
	public:
		ListBox();
		~ListBox();
		
		

		void Clear();
		void AddItem( std::string item );
		int GetSelectedIndex();
		void SetSelectedIndex( int index );
		std::string GetText();
		ListBox* Clone();
};
}
#endif
