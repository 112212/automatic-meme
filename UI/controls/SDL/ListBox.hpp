#ifndef NG_LISTBOX_HPP
#define NG_LISTBOX_HPP

#include "../../Control.hpp"
#include "../ScrollBar.hpp"
#include "TextBox.hpp"
namespace ng {
enum {
	EVENT_LISTBOX_CLICK,
	EVENT_LISTBOX_LOCK,
	EVENT_LISTBOX_UNLOCK,
	EVENT_LISTBOX_CHANGE
};


class ListBox : public Control {
	private:
		bool m_is_mouseDown;
		bool m_drawscrollbar;
		bool m_scrollbar_focus;
		
		void onPositionChange();
		
		TTF_Font* m_font;
		
		// items
		std::vector<std::string> m_items;
		
		// surf text in std::vector
		std::vector<SDL_Surface*> m_vec_surf_text;
		
		SDL_Rect m_scrollrect;
		ScrollBar *m_scrollbar; // scrollbar :)
		
		// min, max dropdown
		int m_max_items;
		int m_last_scroll;
		
		// koliko moze da se prosiri
		
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
		
		void Render( SDL_Renderer* ren, SDL_Rect pos, bool isSelected );
		void OnMouseDown( int mX, int mY );
		void OnMouseUp( int mX, int mY );
		void OnLostFocus();
		virtual void OnGetFocus();
		virtual void OnMWheel( int updown );
		void OnLostControl();
		void OnMouseMove( int mX, int mY, bool mouseState );
		
	public:
		ListBox();
		~ListBox();
		
		

		void Clear() { m_items.clear(); }
		void AddItem( const char* item );
		void AddItem( std::string item );
		int GetSelectedIndex();
		void SetSelectedIndex( int index );
		std::string GetText();
};
}
#endif
