#ifndef NG_LISTBOX_HPP
#define NG_LISTBOX_HPP

#include "../Control.hpp"
#include "../Control.hpp"
#include "ScrollBar.hpp"
#include "TextBox.hpp"
namespace ng {

class ListBox : public Control {
	private:
		bool m_drawscrollbar;
		bool m_scrollbar_focus;
		bool m_details;
		bool m_has_pics;
		bool m_has_caption;
		bool m_readonly;
		bool m_always_changed;
		bool m_hover_mode;
		int m_spacing;
		std::string m_caption;
		
		struct Item {
			std::string str;
			std::string val;
			Image* img;
			Image* pic;
		};
		
		std::vector<Item> m_items;
		
		int m_selection_color;
		Rect m_scrollrect;
		ScrollBar *m_scrollbar;
		
		// min, max dropdown
		int m_max_items;
		int m_last_scroll;
		int m_selected_index;
		int m_scroll_pos;
		int m_font_height;
		
		int getListOffset();
		void updateItemSizes();
		int getAverageHeight();
		
		
		void onRectChange();
		void onFontChange();
		
		void updateBox();
		
		void Render( Point pos, bool isSelected );
		void OnMouseDown( int mX, int mY, MouseButton which_button );
		void OnMouseUp( int mX, int mY, MouseButton which_button );
		void OnLostFocus();
		virtual void OnGetFocus();
		virtual void OnMWheel( int updown );
		void OnMouseMove( int mX, int mY, bool mouseState );
		void OnSetStyle(std::string& style, std::string& value);
		void parseXml(rapidxml::xml_node<char>* node);
		virtual void OnKeyDown( Keycode sym, Keymod mod );
		
	public:
		ListBox();
		~ListBox();
		virtual Rect GetContentRect();
		void Clear();
		void AddItem( std::string item, std::string value="", Image* smallimg=0 );
		void AddItem( std::string item, Image* smallimg );
		void RemoveItem(int idx);
		int GetSelectedIndex();
		void SetSelectedIndex( int index );
		void SetSmallImg(int idx, Image* smallimg);
		std::string GetText();
		std::string GetValue();
		void SetValue(int idx, std::string val);
		ListBox* Clone();
};
}
#endif
