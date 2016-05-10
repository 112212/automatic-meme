#ifndef NG_TEXTBOX_HPP
#define NG_TEXTBOX_HPP

#include "../../Control.hpp"
#include "../../common/SDL/Drawing.hpp"
namespace ng {

class TextBox : public Control {
	private:
	
		// first.middle.last
		
		//#### TODO: REMOVE
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
		//####
		
		//~~~~ TODO: implement
		struct TextLine {
			unsigned int tex;
			SDL_Surface* surf;
			std::string text;
		};
		std::vector<TextLine> lines;
		
		Point anchor;
		Point cursor;
		
		// config
		bool isMultiline;
		bool parseTags;
		bool copyPasteRemoveTags; // remove color tags ( ^r red color ^w white again ^5 some color )
		bool terminalMode;
		//~~~~
		
		TTF_Font* m_font;
		static SDL_Cursor* m_cursor;
		
		
		bool m_is_mouseDown;
		
		int m_maxtext;
		
		void onPositionChange();

		void updateSelection();
		void fixSelection();
		int getSelectionPoint( int &mX );
		int getCharPos( int num );
		void updateCursor();
		int getMaxText( );
		int getMaxTextBw( int indx );
		void updateMaxText();
		void setFirstIndex( int index );
		void STYLE_FUNC(value);
		
		
	public:
	
		enum event {
			textbox_change,
			textbox_enter,
			textbox_terminal_command
		};
	
		TextBox();
		~TextBox();
		
		
		virtual void Render( SDL_Rect position, bool isSelected );
		virtual void OnMouseDown( int mX, int mY );
		virtual void OnMouseUp( int mX, int mY );
		virtual void OnMouseMove( int mX, int mY, bool mouseState );
		virtual void OnLostFocus();
		virtual void OnGetFocus();
		virtual void OnKeyDown( SDL_Keycode &sym, SDL_Keymod mod );
		virtual void OnLostControl();
		
		void SetText( std::string text );
		const char* GetText( );
		const char* GetSelectedText();
		
		void SetSelection( int start, int end );
		void SetCursor( SDL_Cursor* curs );
};
}
#endif
