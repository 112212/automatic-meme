#ifndef NG_TEXTBOX_HPP
#define NG_TEXTBOX_HPP

#include "../../Control.hpp"
#include "../../common/SDL/Drawing.hpp"
#include "../../common/Colorstring.hpp"
#include "../ScrollBar.hpp"

#include <vector>
namespace ng {

class TextBox : public Control {
	private:
		struct TextLine {
			int tex;
			int w,h;
			// std::string text;
			Colorstring text;
			bool wrap;
			int last_color;
			TextLine() : tex(0xffffffff),wrap(false),last_color(0) {}
		};
		std::vector<TextLine> m_lines;
		
		TextLine m_placeholder;
		
		// position of window
		Point m_position;
		// anchor for selection (no selection if x,y = -1,-1)
		Point m_anchor;
		
		ScrollBar *m_scrollbar;
		bool m_scrollbar_selected;
		
		Point m_cursor;
		int m_line_max;
		int m_cursor_max_x;
		int m_cursor_blink_counter;
		int m_cursor_blinking_rate;
		int m_readonly;
		bool m_colors;
		
		bool m_color_input;
		bool m_locked;
		bool m_password;
		int m_max_length;
		int m_text_max;
		int m_lines_max;
		int m_line_height;
		int m_line_max_width;
		
		static int m_selection_color;
		static int m_cursor_color;
		
		void updatePosition();
		void onFontChange();
		void backspace();
		void spreadColor(std::vector<TextLine>::iterator it, bool fully = false);
		void updateTexture(TextLine&,bool = false);
		std::vector<TextLine> wrap_lines(const std::vector<TextLine>& lines);
		void compact_lines(std::vector<TextLine>& v, std::vector<TextLine>::iterator it);
		void sortPoints(Point &p1, Point &p2);
		void deleteSelection();
		void OnMWheel( int updown );
		
		// config
		bool m_multiline;
		bool m_textwrap;
		bool m_wordwrap;
		
		bool m_mousedown;

		void onPositionChange();
		void OnSetStyle(std::string& style, std::string& value);
		
	public:
	
		TextBox();
		~TextBox();
		
		//
		virtual void Render( Point position, bool isSelected );
		virtual void OnMouseDown( int mX, int mY, MouseButton button );
		virtual void OnMouseUp( int mX, int mY, MouseButton button );
		virtual void OnMouseMove( int mX, int mY, bool mouseState );
		virtual void OnLostFocus();
		virtual void OnGetFocus();
		virtual void OnKeyDown( SDL_Keycode &sym, SDL_Keymod mod );
		virtual void OnLostControl();
		//
				
		TextBox* Clone();
		void SetText(std::string text);
		std::string GetText();
		std::string GetRawText();
		std::string GetSelectedText();
		std::string GetRawSelectedText();
		
		void PutTextAtCursor(std::string text);
		void PutCursorAt( Point cursor );
		
		void SetSelection( Point start, Point end );
		void SetCursor( SDL_Cursor* curs );
		void SetMultilineMode( bool tf );
		void SetCursorBlinkingRate( int rate );
		void SetTextWrap(bool wrap);
		void SetWordWrap(bool word);
		void SetReadOnly( bool );
};
}
#endif
