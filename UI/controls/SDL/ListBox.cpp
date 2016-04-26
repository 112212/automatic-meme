#include "ListBox.hpp"
namespace ng {
ListBox::ListBox() {
	setType( TYPE_LISTBOX );
	
	m_font_height = 13;
	m_font = Fonts::GetFont( "default", m_font_height );
	
	m_scrollbar = 0;
	m_selected_index = -1;
	m_drawscrollbar = false;
	m_scrollbar_focus = false;
	
	m_last_scroll = 0;
}

ListBox::~ListBox() {
	if(m_scrollbar)
		delete m_scrollbar;
}

void ListBox::Render( SDL_Renderer* ren, SDL_Rect pos, bool isSelected ) {
	int x = m_rect.x + pos.x;
	int y = m_rect.y + pos.y;
	// draw items
	int h=0,i=0;
	int offs = getListOffset();
	for(auto it = m_vec_surf_text.cbegin()+offs; it != m_vec_surf_text.cend(); it++,i++) {
		if(i >= m_max_items)
			break;
		if(m_selected_index == i+offs) {
			Draw_FillRect( ren, x, y + h, m_rect.w - (m_drawscrollbar ? m_scrollrect.w : 0), (*it)->h, 0xffff0000 );
			
		} else
			Draw_FillRect( ren, x, y + h, m_rect.w, (*it)->h, 0 );
		CSurface::OnDraw(ren, *it, x+2, y + h);
		h += (*it)->h;
	}
	//Draw_Rect(surf, x, y, m_rect.w, h, Colors::c_white );
	if(m_drawscrollbar) {
		m_scrollbar->Render( ren, pos, false );
	}
	Draw_Rect(ren, x, y, m_rect.w, m_rect.h, Colors::Gray );
	
	#ifdef SELECTION_MARK
		Draw_Rect(ren, x, y, m_rect.w, m_rect.h, isSelected ? Colors::Yellow : Colors::White );
	#else
		Draw_Rect(ren, x, y, m_rect.w, m_rect.h, Colors::White );
	#endif
}

void ListBox::AddItem( std::string item ) {
	
	SDL_Surface* txt = TTF_RenderText_Solid( m_font, clipText( item, m_rect.w ).c_str(), {255,255,255} );
	if(txt) {
		m_items.push_back( item );
		m_vec_surf_text.push_back( txt );
		
		if(m_selected_index == -1) {
			m_selected_index = 0;
		}
	} else {
		cout << "Error u ubacivanju itema :( ..." << endl;
	}
	updateBox();
}
void ListBox::AddItem( const char* item ) {
	
	SDL_Surface* txt = TTF_RenderText_Solid( m_font, clipText( item, m_rect.w ).c_str(), {255,255,255} );
	if(txt) {
		m_items.push_back( std::string(item) );
		m_vec_surf_text.push_back( txt );
		
		if(m_selected_index == -1) {
			m_selected_index = 0;
		}
	} else {
		cout << "Error u ubacivanju itema :( ..." << endl;
	}
	updateBox();
}


void ListBox::OnMouseDown( int mX, int mY ) {
	m_is_mouseDown = true;
	if(m_drawscrollbar) {
		if( isOnScrollbar( mX, mY ) ) {
			m_scrollbar->OnMouseDown( mX, mY );
			m_scrollbar_focus = true;
			if(m_scrollbar->GetDifference()) {
				m_last_scroll = m_scrollbar->GetValue();
			}
			return;
		} else {
			if(m_scrollbar_focus) {
				m_scrollbar->OnLostFocus();
				m_scrollbar_focus = false;
			}
		}
	}
	
	// treba pronaci na cemu drzimo mis u postaviti "selekciju"
	int tmp = (mY - m_rect.y)/m_font_height + getListOffset();
	if(tmp != m_selected_index) {
		m_selected_index = tmp;
		emitEvent( EVENT_LISTBOX_CHANGE );
	}
}

void ListBox::OnMouseUp( int mX, int mY ) {
	m_is_mouseDown = false;
}

void ListBox::OnMouseMove( int mX, int mY, bool mouseState ) {

	if(m_drawscrollbar) {
		if( isOnScrollbar( mX, mY ) ) {
			m_scrollbar->OnMouseMove( mX, mY, mouseState );
			m_scrollbar_focus = true;
			if(m_scrollbar->GetDifference()) {
				m_last_scroll = m_scrollbar->GetValue();
			}
			return;
		} else {
			if(m_scrollbar_focus) {
				m_scrollbar->OnLostFocus();
				m_scrollbar_focus = false;
			}
		}
	}
}

bool ListBox::isOnScrollbar( int &mX, int &mY ) {
	if( mX > m_scrollrect.x && mX < m_scrollrect.x+m_scrollrect.w ) {
		if( mY > m_scrollrect.y && mY < m_scrollrect.y + m_scrollrect.h ) {
			return true;
		}
	}
	return false;
}


void ListBox::OnLostFocus() {
	m_is_mouseDown = false;
	if(m_scrollbar_focus) {
		m_scrollbar->OnLostFocus();
		m_scrollbar_focus = false;
	}
}


void ListBox::onPositionChange() {
	updateItemsSize();
}


void ListBox::OnLostControl() {
}

int ListBox::getMaxText( std::string txt, int w ) {
	TTF_Font* fnt = m_font;
	
	int dummy, advance;
	int len = txt.length();
	int sum=0;
	for(int i=0; i < len; i++) {
		TTF_GlyphMetrics( fnt, txt[i], &dummy, &dummy, &dummy, &dummy, &advance);
		if( sum > w-15 ) {
			return i+1;
		}
		sum += advance;
	}
	return len;
}

void ListBox::OnGetFocus() {
}

int ListBox::getAverageHeight() {
	int h=0,i=0;
	for(auto it = m_vec_surf_text.begin(); it != m_vec_surf_text.end(); it++,i++) {
		if(h >= m_rect.h)
			break;
		h += (*it)->h;
	}
	return h / i;
}


void ListBox::updateBox() {
	m_font_height = getAverageHeight();
	m_max_items = m_rect.h / m_font_height;
	if(m_items.size() > m_max_items) {
		// treba implementirati scrollbar :)
		if(!m_scrollbar) {
			m_scrollbar = new ScrollBar;
			m_scrollbar->SetVertical( true );
		}
		int scrollbar_width = 10;
		m_scrollrect = { m_rect.x + m_rect.w - scrollbar_width, m_rect.y, scrollbar_width, m_rect.h };
		m_scrollbar->SetRect( m_scrollrect.x, m_scrollrect.y, m_scrollrect.w, m_scrollrect.h );
		
		m_scrollbar->SetSliderSize( std::max<int>(10, std::min<int>( ( (m_max_items*100)/m_items.size()), m_scrollrect.h - 10) ) );
		m_scrollbar->SetMaxRange( m_items.size() - m_max_items );
		m_scrollbar->SetMouseWheelConstant( std::max<int>( m_max_items/3, 1 ) );
		m_drawscrollbar = true;
	}
}

void ListBox::OnMWheel( int updown ) {
	if(m_drawscrollbar) {
		m_scrollbar->OnMWheel( updown );
		if(m_scrollbar->GetDifference()) {
			m_last_scroll = m_scrollbar->GetValue();
		}
	}
}

int ListBox::getListOffset() {
	if(m_drawscrollbar) {
		//int sz = m_vec_surf_text.size();
		return m_last_scroll;//std::min<int>( ((sz-m_max_items)*m_last_scroll) / 100, sz - m_max_items );
	} else return 0;
}


void ListBox::updateItemsSize() {
	std::string tmp;
	for(int i=0; i < m_items.size(); i++) {
		tmp = clipText( m_items[i], m_rect.w );
		if( tmp != m_items[i] ) {
			SDL_FreeSurface( m_vec_surf_text[i] );
			m_vec_surf_text[i] = TTF_RenderText_Solid( m_font, tmp.c_str(), {255,255,255} );
		}
	}
}

std::string ListBox::clipText( std::string s, int w ) {
	int maxtext = getMaxText( s, w );
	if( maxtext < s.size() )
		return s.substr( 0, maxtext-3 ) + "..";
	else
		return s;
}

int ListBox::GetSelectedIndex() {
	return m_selected_index;
}

std::string ListBox::GetText() {
	if(m_selected_index == -1)
		return "";
	else
		return m_items[ m_selected_index ];
}

void ListBox::SetSelectedIndex( int index ) {
	m_selected_index = index;
}


}
