#ifndef NG_TEXTBOX_HPP
#define NG_TEXTBOX_HPP

#include "../../Control.hpp"

enum {
	EVENT_TEXTBOX_CHANGE,
	EVENT_TEXTBOX_ENTER,
};

class TextBox : public Control {
	private:
	
		sf::Text m_text;
		sf::RectangleShape m_cursor;
		sf::RectangleShape m_rectShape;
		sf::RectangleShape m_selection;
		sf::Font m_font;
		
		Point m_text_loc;
		Point m_text_selection;
		bool m_is_mouseDown;
		int m_last_sel;
		int m_cursor_sel;
		int m_cursor_pt;
		int m_first_index;
		int m_maxtext;
		unsigned int characterSize;
		
		std::string m_str;
		void onPositionChange();
		
		static int m_font_index;
		void updateSelection();
		void fixSelection();
		int getSelectionPoint( int mX );
		int getCharPos( int num );
		int getCharDistance( int startChar, int endChar );
		void updateCursor();
		int getMaxText( );
		int getMaxTextBw( int indx );
		void updateMaxText();
		void setFirstIndex( int index );
	public:
		TextBox();
		~TextBox();
		void Render( sf::RenderTarget& ren, sf::RenderStates state, bool isSelected );
		void OnMouseDown( int mX, int mY );
		void OnMouseUp( int mX, int mY );
		void OnMouseMove( int mX, int mY, bool mouseState );
		void OnLostFocus();
		void OnGetFocus();
		void OnKeyDown( sf::Event::KeyEvent &sym );
		void OnLostControl();

		void SetText( const char* text );
		void SetText( std::string text );
		const char* GetText( );
		const char* GetSelectedText();
		
		void SetSelection( int start, int end );
		void SetCursor( SDL_Cursor* curs );
};

#endif
