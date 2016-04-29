#ifndef NG_TEXTBOX_HPP
#define NG_TEXTBOX_HPP

#include "../../Control.hpp"
#include "../../common/SDL/Drawing.hpp"
namespace ng {
enum {
	EVENT_TEXTBOX_CHANGE,
	EVENT_TEXTBOX_ENTER,
	EVENT_TEXTBOX_LOCK
};

class TextBox : public Control {
	private:
	
		// first.middle.last
		SDL_Surface* m_surf_first;
		SDL_Surface* m_surf_middle;
		SDL_Surface* m_surf_last;
		static SDL_Cursor* m_cursor;
		
		TTF_Font* m_font;
		
		Point m_text_loc;
		Point m_text_selection;
		bool m_is_mouseDown;
		int m_last_sel;
		int m_cursor_sel;
		int m_cursor_pt;
		int m_first_index;
		int m_maxtext;
		
		std::string m_text;
		void onPositionChange();
		// static int m_font_index;
		void updateSelection();
		void fixSelection();
		int getSelectionPoint( int &mX );
		int getCharPos( int num );
		void updateCursor();
		int getMaxText( );
		int getMaxTextBw( int indx );
		void updateMaxText();
		void setFirstIndex( int index );
		void OnSetStyle(std::string& style, std::string& value);
		
		
	public:
		TextBox();
		~TextBox();
		
		
		virtual void Render( SDL_Rect position, bool isSelected );
		virtual void OnMouseDown( int mX, int mY );
		virtual void OnMouseUp( int mX, int mY );
		virtual void OnMouseMove( int mX, int mY, bool mouseState );
		virtual void OnLostFocus();
		virtual void OnGetFocus();
		virtual void OnKeyDown( SDL_Keycode &sym, SDL_Keymod &mod );
		virtual void OnLostControl();
		

		void SetText( const char* text );
		void SetText( std::string text );
		const char* GetText( );
		const char* GetSelectedText();
		
		void SetSelection( int start, int end );
		void SetCursor( SDL_Cursor* curs );
};
}
#endif
