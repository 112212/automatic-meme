#include "ComboBox.hpp"
#include "../../common/SDL/Drawing.hpp"

namespace ng {
ComboBox::ComboBox() {
	setType( "combobox" );
	
	m_font_height = 13;
	characterSize = 13;
	
	
	m_is_onarrow = false;
	m_is_opened = false;
	m_scrollbar = 0;
	m_textbox = 0;
	m_virtual_selected_index = -1;
	m_selected_index = -1;
	m_drawscrollbar = false;
	m_scrollbar_focus = false;
	m_textbox_focus = false;
	m_max_dropdown_items = 5;
	
	m_max_width = 0;
	m_dropdown_size = GetRect().h;
	tex_sel = 0;
	m_last_scroll = 0;
	m_is_textbox_mode = false;
	m_style.background_color = 0x00000000;
	m_selection_color = 0xff3E398E;
	
	m_font = Fonts::GetFont( "default", characterSize );
}

const int lineMargin = 2;

ComboBox::~ComboBox() {
	if(m_scrollbar)
		delete m_scrollbar;
	if(m_textbox)
		delete m_textbox;
}



void ComboBox::OnMouseDown( int mX, int mY ) {
	const Rect& r = GetRect();
	if(m_is_opened) {
		if(m_drawscrollbar) {
			if( isOnScrollbar( mX, mY ) ) {
				m_scrollbar->OnMouseDown( mX, mY );
				m_virtual_selected_index = -1;
				m_scrollbar_focus = true;
				m_last_scroll = m_scrollbar->GetValue();
				return;
			} else if(m_scrollbar_focus) {
				m_scrollbar->OnLostFocus();
				m_scrollbar_focus = false;
			}
		}
		
		
		if(mX > r.x && mX < r.x + m_max_width) {
			if(mY > r.y+r.h && mY < r.y+r.h + m_dropdown_size ) {

				int relative_selection = (mY - (r.y+r.h))/m_font_height;
				int absolute_selection = relative_selection + getListOffset();
				if(absolute_selection != m_selected_index) {
					m_selected_index = absolute_selection;
					updateSelection();
					emitEvent( event::change );
				}
				
				m_is_opened = false;
				sendGuiCommand( GUI_UNLOCK );
				sendGuiCommand( GUI_LOCK_ONCE );
				return;
			}
		}
		m_is_opened = false;
		sendGuiCommand( GUI_UNLOCK );
		sendGuiCommand( GUI_LOCK_ONCE );
		return;
	}
	
	if(m_is_textbox_mode) {
		if( isOnArrow( mX, mY ) ) {
			m_is_opened = !m_is_opened;
		} else if( isOnText( mX, mY ) ) {
			if( mX > r.x && mX < r.x + r.w - KVADRAT_SIZE ) {
				if( mY > r.y && mY < r.y + r.h ) {
					sendGuiCommand( GUI_KEYBOARD_LOCK );
					m_textbox_focus = true;
					m_textbox->OnMouseDown( mX, mY );
				}
			}
		}
	} else if( check_collision( mX, mY ) ){
		m_is_opened = !m_is_opened;
	}
	
	if(m_is_opened) {
		sendGuiCommand( GUI_FOCUS_LOCK );
		openBox();
	} else if( !m_textbox_focus ) {
		sendGuiCommand( GUI_UNLOCK );
		sendGuiCommand( GUI_LOCK_ONCE );
		m_drawscrollbar = false;
	}
	m_is_mouseDown = true;
}

void ComboBox::OnMouseUp( int x, int y ) {
	m_is_mouseDown = false;
	if(m_is_textbox_mode) {
		m_textbox->OnMouseUp( x, y );
	}
}

void ComboBox::OnMouseMove( int mX, int mY, bool mouseState ) {
	const Rect& r = GetRect();
	if(m_is_opened) {
		if(m_drawscrollbar) {
			if( m_scrollbar_focus ) {
				m_scrollbar->OnMouseMove( mX, mY, mouseState );
				m_last_scroll = m_scrollbar->GetValue();
				// just moved scrollbar, no need to do anything else
				return;
			}
		}
		if(mX > r.x && mX < r.x + m_max_width) {
			if(mY > r.y+r.h && mY < r.y+r.h + m_dropdown_size ) {
				int relative_selection = (mY - (r.y+r.h))/m_font_height;
				m_virtual_selected_index = relative_selection;
			}
		}
	} else {
		m_is_onarrow = isOnArrow( mX, mY );
		if(m_is_textbox_mode) {
			// TODO: check if this is correct solution
			if(!m_textbox_focus) {
				m_textbox_focus = true;
				m_textbox->OnGetFocus();
			}
			m_textbox->OnMouseMove( mX, mY, mouseState );
		}
	}
}


bool ComboBox::isOnArrow( int mX, int mY ) {
	const Rect& r = GetRect();
	if( mX > r.x+r.w - KVADRAT_SIZE && mX < r.x+r.w ) {
		if( mY > r.y && mY < r.y + r.h ) {
			return true;
		}
	}
	return false;
}


bool ComboBox::isOnText( int mX, int mY ) {
	const Rect& r = GetRect();
	if( mX > r.x && mX < r.x + r.w - KVADRAT_SIZE ) {
		if( mY > r.y && mY < r.y + r.h ) {
			return true;
		}
	}
	return false;
}


bool ComboBox::isOnScrollbar( int mX, int mY ) {
	if( mX > m_scrollrect.x && mX < m_scrollrect.x+m_scrollrect.w ) {
		if( mY > m_scrollrect.y && mY < m_scrollrect.y + m_scrollrect.h ) {
			return true;
		}
	}
	return false;
}

void ComboBox::OnLostFocus() {
	m_is_mouseDown = false;
	m_is_onarrow = false;
	if(m_scrollbar_focus) {
		m_scrollbar->OnLostFocus();
		m_scrollbar_focus = false;
	}
	if(m_is_textbox_mode) {
		m_textbox_focus = false;
		m_textbox->OnLostFocus();
	}
}

void ComboBox::OnLostControl() {
	m_is_opened = false;
	if(m_is_textbox_mode) {
		m_textbox_focus = false;
		m_textbox->OnLostControl();
	}
}

void ComboBox::OnGetFocus() {
	if(m_is_textbox_mode) {
		m_textbox_focus = true;
		m_textbox->OnGetFocus();
	}

}

void ComboBox::OnMWheel( int updown ) {
	if(m_drawscrollbar) {
		m_scrollbar->OnMWheel( updown );
		m_last_scroll = m_scrollbar->GetValue();
	}
}

int ComboBox::getListOffset() {
	if(m_drawscrollbar) {
		//int sz = text_lines.size();
		return m_last_scroll;//min<int>( ((sz-m_max_dropdown_items)*m_last_scroll) / 100, sz - m_max_dropdown_items );
	} else return 0;
}

std::string ComboBox::clipText( std::string s, int w ) {
	int maxtext = Fonts::getMaxText( m_font, s, w-15 );
	if( maxtext < s.size() )
		return s.substr( 0, maxtext-2 ) + "...";
	else
		return s;
}

/*
	-------------[ PUBLIC METHODS ]-----------
*/

int ComboBox::GetSelectedIndex() {
	return m_selected_index;
}


std::string ComboBox::GetText() {
	if(m_is_textbox_mode) {
		return m_textbox->GetText();
	} else {
		if(m_selected_index == -1)
			return "";
		else
			return m_items[ m_selected_index ];
	}
}

void ComboBox::SetSelectedIndex( int index ) {
	m_selected_index = index;
	updateSelection();
}

void ComboBox::SetTextEditableMode( bool editablemode ) {
	if(editablemode) {
		if(!m_textbox)
			m_textbox = new TextBox;
		const Rect& r = GetRect();
		m_textbox->SetRect( r.x, r.y, r.w-KVADRAT_SIZE, r.h );
		if(m_selected_index != -1)
			m_textbox->SetText( m_items[ m_selected_index ] );
	} else {
		if(m_textbox) {
			delete m_textbox;
			m_textbox = 0;
		}
	}
	m_is_textbox_mode = editablemode;
}


void ComboBox::SetMaxDropdown( int drp ) {
	m_max_dropdown_items = drp;
}


void ComboBox::SetMaxWidth( int w ) {
	m_max_width = std::max<int>( GetRect().w, w );
	updateItemsSize();
}


void ComboBox::AddItem( const char* item ) {
	AddItem( std::string(item) );
}

void ComboBox::OnKeyDown( SDL_Keycode &sym, SDL_Keymod &mod ) {
	if(m_is_textbox_mode)
		m_textbox->OnKeyDown( sym, mod );
}

void ComboBox::AddItem( std::string item ) {
	if(!m_font) return;
	SDL_Surface* txt = TTF_RenderText_Blended( m_font, clipText( item, m_max_width ).c_str(), {255,255,255} );
	if(txt) {
		m_items.push_back( item );
		text_lines.push_back( { Drawing::GetTextureFromSurface(txt, 0), txt->w, txt->h} );
		m_font_height = txt->h;
		
		SDL_FreeSurface(txt);
		
		if(m_selected_index == -1) {
			m_selected_index = 0;
		}
		updateSelection();
	}
}

void ComboBox::Render( Point pos, bool isSelected ) {
	const Rect& rect = GetRect();
	
	Drawing::VLine( rect.x+pos.x+rect.w - KVADRAT_SIZE, rect.y+pos.y, rect.y+pos.y+rect.h, Colors::Gray );
	
	if(m_is_textbox_mode ?  m_is_onarrow : isSelected) {
		Drawing::FillRect(rect.x+pos.x+rect.w-KVADRAT_SIZE+1, rect.y+pos.y+1, KVADRAT_SIZE-2, rect.h-2, 0x336633 );
	}
	
	
	Drawing::Line(rect.x+pos.x+rect.w - KVADRAT_SIZE + INNER_X, rect.y+pos.y+INNER_Y, rect.x+pos.x+rect.w - KVADRAT_SIZE/2, rect.y+pos.y+rect.h-INNER_Y, Colors::Gray );
	Drawing::Line(rect.x+pos.x+rect.w - INNER_X, rect.y+pos.y+INNER_Y, rect.x+pos.x+rect.w - KVADRAT_SIZE/2, rect.y+pos.y+rect.h-INNER_Y, Colors::Gray );
	
	if(m_is_opened) {
		// draw items
		int h=0,i=0;
		int offs = getListOffset();
		
		for(auto it = text_lines.cbegin()+offs; it != text_lines.cend(); it++,i++) {
			if(i >= m_max_dropdown_items)
				break;
			h += it->h;
		}
		Drawing::FillRect( rect.x + pos.x, rect.y + pos.y + rect.h, rect.w, rect.h + h, m_style.background_color);
		h=i=0;
		for(auto it = text_lines.cbegin()+offs; it != text_lines.cend(); it++,i++) {
			if(i >= m_max_dropdown_items)
				break;
			if(m_virtual_selected_index == i) {
				Drawing::FillRect( rect.x+pos.x, rect.y+pos.y + rect.h + h, m_max_width - (m_drawscrollbar ? m_scrollrect.w : 0), it->h, m_selection_color );
			} else {
				Drawing::Rect(rect.x+pos.x, rect.y+pos.y + rect.h + h, m_max_width, it->h, 0);
			}
			
			Drawing::TexRect( rect.x+pos.x+2, rect.y+pos.y + rect.h + h, it->w, it->h, it->tex );
			h += it->h;
		}
		
		if(m_drawscrollbar) {
			if(m_scrollrect.h != h) {
				m_scrollrect.h = h;
				m_scrollbar->SetRect( m_scrollrect );
			}
			m_scrollbar->Render( pos, false );
		}
	}
	
	
	if(m_is_textbox_mode) {
		m_textbox->Render( pos, m_textbox_focus );
	} else if(tex_sel != 0) {
		Drawing::TexRect( m_text_loc.x+pos.x, m_text_loc.y+pos.y, m_text_loc.w, m_text_loc.h, tex_sel );
	}
	
	Control::Render(pos, isSelected);
	
}

int ComboBox::getAverageHeight() {
	if(m_items.size() == 0) return 1;
	int h=0,i=0;
	for(auto it = text_lines.cbegin(); it != text_lines.cend(); it++,i++) {
		if(i >= m_max_dropdown_items)
			break;
		h += it->h;	
	}
	return h / i + 1;
}

void ComboBox::openBox() {
	const Rect& rect = GetRect();
	m_font_height = getAverageHeight();

	m_dropdown_size = m_items.size() * m_font_height;
	if(m_items.size() > m_max_dropdown_items) {

		if(!m_scrollbar) {
			m_scrollbar = new ScrollBar;
			m_scrollbar->SetVertical( true );
		}

		int scrollbar_width = 10;
		m_scrollrect = Rect( rect.x + m_max_width - scrollbar_width, rect.y+rect.h, scrollbar_width, m_dropdown_size );
		m_scrollbar->SetRect( m_scrollrect );
		
		m_scrollbar->SetSliderSize( std::max<int>(10, std::min<int>( ( (m_max_dropdown_items*100)/m_items.size()), m_scrollrect.h - 10) ) );
		m_scrollbar->SetRange( 0, m_items.size() - m_max_dropdown_items );
		m_scrollbar->SetMouseWheelConstant( std::max<int>( m_max_dropdown_items/3, 1 ) );
		m_drawscrollbar = true;
	}
}

void ComboBox::updateItemsSize() {
	std::string tmp;
	for(int i=0; i < m_items.size(); i++) {
		tmp = clipText( m_items[i], m_max_width );
		if( tmp != m_items[i] ) {
			SDL_Surface* surf = TTF_RenderText_Blended( m_font, tmp.c_str(), {255,255,255});
			text_lines[i] = { Drawing::GetTextureFromSurface(surf, text_lines[i].tex), surf->w, surf->h };
			SDL_FreeSurface(surf);
		}
	}
}

void ComboBox::onPositionChange() {
	const Rect& rect = GetRect();
	if(!m_max_width)
		m_max_width = rect.w;
		
	m_text_loc.x = rect.x + 5;
	m_text_loc.y = rect.y + 2;
	
	if(m_is_textbox_mode)
		m_textbox->SetRect( rect.x, rect.y, rect.w-KVADRAT_SIZE, rect.h );
}

void ComboBox::updateSelection() {
	if(m_selected_index >= m_items.size()) return;
	if(m_is_textbox_mode) {
		m_textbox->SetText( m_items[ m_selected_index ] );
	} else {
		SDL_Surface* surf = TTF_RenderText_Blended(  m_font, clipText( m_items[ m_selected_index ], GetRect().w-KVADRAT_SIZE ).c_str(), {255,255,255} );
		tex_sel = Drawing::GetTextureFromSurface( surf, tex_sel );
		m_text_loc.w = surf->w;
		m_text_loc.h = surf->h;
		SDL_FreeSurface(surf);
	}
	
}

Control* ComboBox::Clone() {
	ComboBox* cb = new ComboBox;
	copyStyle(cb);
	return cb;
}

void ComboBox::STYLE_FUNC(value) {
	STYLE_SWITCH {
		_case("value"):
			SetSelectedIndex(std::stoi(value));
		_case("selection_color"):
			m_selection_color = Colors::ParseColor(value);
		_case("dropdown_size"):
			m_max_width = std::stoi(value);
	}
}


}
