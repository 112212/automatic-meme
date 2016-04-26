#ifndef NG_LISTBOX_HPP
#define NG_LISTBOX_HPP

#include "../../Control.hpp"
#include "../ScrollBar.hpp"
#include "TextBox.hpp"

enum {
	EVENT_LISTBOX_CLICK,
	EVENT_LISTBOX_LOCK,
	EVENT_LISTBOX_UNLOCK,
	EVENT_LISTBOX_CHANGE
};


class ListBox : public Control {
	private:
		sf::Font m_font;
		sf::RectangleShape m_rectShape;
		sf::RectangleShape m_highlight;
		
		bool m_is_mouseDown;
		bool m_drawscrollbar;
		bool m_scrollbar_focus;
		
		void onPositionChange();
		
		unsigned int characterSize;
		
		// items
		std::vector<std::string> m_items;
		
		// surf text in vector
		std::vector<sf::Text> m_texts;
		
		Rect m_scrollrect;
		ScrollBar *m_scrollbar;
		
		// min, max dropdown
		int m_max_items;
		int m_last_scroll;
		
		int m_selected_index;
		
		int m_scroll_pos;
		int m_font_height;
		
		bool isOnScrollbar( int mX, int mY );
		int getMaxText( std::string txt, int w );
		int getListOffset();
		void updateItemsSize();
		int getAverageHeight();
		std::string clipText( std::string s, int w );
		void updateBox();
	public:
		ListBox();
		~ListBox();
		void Render( sf::RenderTarget& ren, sf::RenderStates states, bool isSelected );
		void OnMouseDown( int mX, int mY );
		void OnMouseUp( int mX, int mY );
		void OnLostFocus();
		void OnGetFocus();
		void OnMWheel( int &updown );
		
		void OnLostControl();
		void OnMouseMove( int mX, int mY, bool mouseState );

		void Clear() { m_items.clear(); }
		void AddItem( const char* item );
		void AddItem( std::string item );
		int GetSelectedIndex();
		void SetSelectedIndex( int index );
		std::string GetText();
};

#endif
