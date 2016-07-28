#ifndef NG_TEXTBOX_HPP
#define NG_TEXTBOX_HPP

#include "../../Control.hpp"
#include "../../common/SDL/Drawing.hpp"
#include <vector>
namespace ng {

class TextBox : public Control {
	private:
	
		// first.middle.last
		
		//########## TODO: REMOVE #####
		SDL_Surface* m_surf_first;
		SDL_Surface* m_surf_middle;
		SDL_Surface* m_surf_last;
		Uint32 tex_first;
		Uint32 tex_middle;
		Uint32 tex_last;
		std::string m_text;
		
		Point m_text_loc;
		Point m_text_selection;
		
		int m_last_sel;
		int m_cursor_sel;
		int m_cursor_pt;
		int m_first_index;
		int m_maxtext;
		
		void updateSelection();
		void fixSelection();
		int getSelectionPoint( int &mX );
		int getCharPos( int num );
		void updateCursor();
		int getMaxText( );
		int getMaxTextBw( int indx );
		void updateMaxText();
		void setFirstIndex( int index );
		// static SDL_Cursor* m_cursor;
		//#######################3
		
		//~~~~ TODO: implement
		struct TextLine {
			unsigned int tex;
			SDL_Surface* surf;
			std::string text;
		};
		std::vector<TextLine> m_lines;
		
		// position of window
		Point m_position;
		// anchor for selection (no selection if -1,-1)
		Point m_anchor;
		// blinking cursor
		Point m_cursor;
		
		// config
		bool isMultiline;
		bool parseTags;
		bool copyPasteRemoveTags; // remove color tags ( ^r red color ^w white again ^5 some color )
		bool terminalMode;
		//~~~~
		
		TTF_Font* m_font;
		bool m_mousedown;

		void onPositionChange();
		void STYLE_FUNC(value);
		
	public:
	
		enum event {
			change,
			enter,
			terminal_command
		};
	
		TextBox();
		~TextBox();
		
		//
		virtual void Render( SDL_Rect position, bool isSelected );
		virtual void OnMouseDown( int mX, int mY );
		virtual void OnMouseUp( int mX, int mY );
		virtual void OnMouseMove( int mX, int mY, bool mouseState );
		virtual void OnLostFocus();
		virtual void OnGetFocus();
		virtual void OnKeyDown( SDL_Keycode &sym, SDL_Keymod mod );
		virtual void OnLostControl();
		//
				
		void SetText(std::string text);
		std::string GetText();
		std::string GetSelectedText();
		
		void SetSelection( Point start, Point end );
		void SetCursor( SDL_Cursor* curs );
};
}
#endif
