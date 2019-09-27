#include <RapidXML/rapidxml.hpp>
#include "ComboBox.hpp"
#include "../Gui.hpp"

namespace ng {
ComboBox::ComboBox() {
	setType( "combobox" );
	
	m_font_height = 13;
	
	m_is_onarrow = false;
	m_is_opened = false;
	m_textbox = 0;
	m_selected_index = -1;
	m_textbox_focus = false;
	m_max_dropdown_items = 5;
	
	m_max_width = GetRect().w;
	m_dropdown_size = GetRect().h;
	tex_sel = 0;
	m_last_scroll = 0;
	m_is_textbox_mode = false;
	m_style.background_color = 0x00000000;
	m_selection_color = 0xff3E398E;
	m_lb = 0;
	
}

const int lineMargin = 2;

ComboBox::~ComboBox() {
	if(m_textbox)
		delete m_textbox;
}

void ComboBox::parseXml(rapidxml::xml_node<char>* node) {
	for(;node;node=node->next_sibling()) {
		if(std::string(node->name()) == "item") {
			AddItem(node->value());
		}
	}
}

void ComboBox::OnMouseDown( int mX, int mY, MouseButton which_button )  {
	const Rect& r = GetRect();

	Point p(mX,mY);
	if(m_is_textbox_mode) {
		if( isOnArrow( mX, mY ) ) {
			m_is_opened = !m_is_opened;
			
		} else if( isOnText( mX, mY ) ) {
			if( mX > r.x && mX < r.x + r.w - RECTANGLE_SIZE ) {
				if( mY > r.y && mY < r.y + r.h ) {
					sendGuiCommand( GUI_KEYBOARD_LOCK );
					m_textbox_focus = true;
					m_textbox->OnMouseDown( mX, mY, which_button );
				}
			}
		}
	} else if( CheckCollision( p+r ) ){
		m_is_opened = !m_is_opened;
	}
	
	if(m_is_opened) {
		if(!m_lb && getEngine()) {
			m_lb = createControl<ListBox>("listbox", "listbox");
			getEngine()->AddControl(m_lb,false);
			m_lb->SetFont(m_style.font);
			m_lb->SetStyle("hoverselectionmode", "true");
			m_lb->SetStyle("always_change", "true");
			m_lb->OnEvent("click", [&](Args& args) {
				sendGuiCommand( GUI_UNLOCK );
				args.control->SetVisible(false);
				// std::cout << "clicked\n";
				m_selected_index = m_lb->GetSelectedIndex();
				updateSelection();
				m_is_opened = false;
				
				emitEvent("change", {m_selected_index});
			});
			m_lb->OnEvent("lostfocus", [&](Args& args) {
				m_is_opened = false;
				args.control->SetVisible(false);
				// sendGuiCommand( GUI_UNLOCK );
			});
			
		}
		
		if(m_lb) {
			m_lb->SendToFront();
			m_lb->SetVisible(true);
			m_lb->Focus();
			sendGuiCommand( GUI_LOCK_ONCE, m_lb );
			Point abs = getAbsoluteOffset();
			m_lb->Clear();
			for(auto i : m_items) {
				m_lb->AddItem(i);
			}
			Rect cr = m_lb->GetContentRect();
			m_lb->SetRect(abs.x, abs.y + r.h, r.w, cr.h);
			m_lb->ProcessLayout(true);
		}
		
	} else {
		if(m_lb) {
			m_lb->SetVisible(false);
			sendGuiCommand( GUI_UNLOCK );
		}
	}
	
	m_is_mouseDown = true;
}

void ComboBox::OnMouseUp( int x, int y, MouseButton which_button ) {
	m_is_mouseDown = false;
	if(m_is_textbox_mode) {
		m_textbox->OnMouseUp( x, y, which_button );
	}
}

void ComboBox::OnMouseMove( int mX, int mY, bool mouseState ) {
	const Rect& r = GetRect();
	if(!m_is_opened) {

		m_is_onarrow = isOnArrow( mX+r.x, mY+r.y );
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
	if( mX > r.w - RECTANGLE_SIZE && mX < r.w ) {
		if( mY > 0 && mY < r.h ) {
			return true;
		}
	}
	return false;
}


bool ComboBox::isOnText( int mX, int mY ) {
	const Rect& r = GetRect();
	if( mX > 0 && mX < r.w - RECTANGLE_SIZE ) {
		if( mY > 0 && mY < r.h ) {
			return true;
		}
	}
	return false;
}



void ComboBox::OnLostFocus() {
	m_is_mouseDown = false;
	m_is_onarrow = false;

	if(m_is_textbox_mode) {
		m_textbox_focus = false;
		m_textbox->OnLostFocus();
	}
}


void ComboBox::OnGetFocus() {
	if(m_is_textbox_mode) {
		m_textbox_focus = true;
		m_textbox->OnGetFocus();
	}

}

void ComboBox::OnMWheel( int updown ) {
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
		if(!m_textbox) {
			m_textbox = new TextBox;
		}
		const Rect& r = GetRect();
		m_textbox->SetRect( r.x, r.y, r.w-RECTANGLE_SIZE, r.h );
		if(m_selected_index != -1) {
			m_textbox->SetText( m_items[ m_selected_index ] );
		}
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

void ComboBox::onFontChange() {
	m_font_height = 0;
	if(m_lb) {
		m_lb->SetFont(m_style.font);
	}
	updateSelection();
}

void ComboBox::SetMaxWidth( int w ) {
	m_max_width = std::max<int>( GetRect().w, w );
}


void ComboBox::AddItem( const char* item ) {
	AddItem( std::string(item) );
}

void ComboBox::OnKeyDown( Keycode sym, Keymod mod ) {
	if(m_is_textbox_mode)
		m_textbox->OnKeyDown( sym, mod );
}

void ComboBox::AddItem( std::string item ) {
	if(!m_style.font) return;
	m_items.push_back(item);
	std::string clipped = m_style.font->ClipText( item, m_max_width );
	text_lines.push_back ( {clipped, m_style.font->GetTextImage( clipped, 0xffffff ) } );
	Size s = text_lines.back().img->GetImageSize();
	m_font_height = std::max(m_font_height, s.h);
	
	if(m_selected_index == -1) {
		m_selected_index = 0;
	}
	updateSelection();
}

void ComboBox::Render( Point pos, bool isSelected ) {
	const Rect& rect = GetRect();
	
	Control::RenderBase(pos, isSelected);
	Drawing().VLine( rect.x+pos.x+rect.w - RECTANGLE_SIZE, rect.y+pos.y, rect.y+pos.y+rect.h, Color::Gray );
	
	if(m_is_textbox_mode ? m_is_onarrow : isSelected) {
		Drawing().FillRect(rect.x+pos.x+rect.w-RECTANGLE_SIZE+1, rect.y+pos.y+1, RECTANGLE_SIZE-2, rect.h-2, 0x336633 );
	}
	
	Drawing().Line(rect.x+pos.x+rect.w - RECTANGLE_SIZE + INNER_X, rect.y+pos.y+INNER_Y, rect.x+pos.x+rect.w - RECTANGLE_SIZE/2, rect.y+pos.y+rect.h-INNER_Y, Color::Gray );
	Drawing().Line(rect.x+pos.x+rect.w - INNER_X, rect.y+pos.y+INNER_Y, rect.x+pos.x+rect.w - RECTANGLE_SIZE/2, rect.y+pos.y+rect.h-INNER_Y, Color::Gray );

	if(m_is_textbox_mode) {
		m_textbox->Render( pos, m_textbox_focus );
	} else if(tex_sel != 0) {
		Drawing().TexRect( m_text_loc.x+pos.x, m_text_loc.y+pos.y, m_text_loc.w, m_text_loc.h, tex_sel );
	}
	
	Control::RenderWidget(pos, isSelected);
	
}

int ComboBox::getAverageHeight() {
	if(m_items.size() == 0) return 1;
	int h=0,i=0;
	for(auto it = text_lines.cbegin(); it != text_lines.cend(); it++,i++) {
		if(i >= m_max_dropdown_items) {
			break;
		}
		Size s = it->img->GetImageSize();
		h += s.h;	
	}
	return h / i + 1;
}


void ComboBox::onRectChange() {
	const Rect& rect = GetRect();
	m_max_width = rect.w;
	m_text_loc.x = rect.x + 5;
	m_text_loc.y = rect.y + 2;
	updateSelection();
	if(m_is_textbox_mode) {
		m_textbox->SetRect( rect.x, rect.y, rect.w-RECTANGLE_SIZE, rect.h );
	}
}

void ComboBox::updateSelection() {
	if(m_selected_index >= m_items.size()) return;
	if(m_is_textbox_mode) {
		// m_textbox->SetText( m_items[ m_selected_index ] );
	} else {
		tex_sel = m_style.font->GetTextImage( m_style.font->ClipText( m_items[ m_selected_index ], GetRect().w-RECTANGLE_SIZE ), 0xffffff );
		Size s = tex_sel->GetImageSize();
		m_text_loc.w = s.w;
		m_text_loc.h = s.h;
	}
	
}

Control* ComboBox::Clone() {
	ComboBox* cb = new ComboBox();
	copyStyle(cb);
	return cb;
}

void ComboBox::OnSetStyle(std::string& style, std::string& value) {
	STYLE_SWITCH {
		_case("value"):
			SetSelectedIndex(std::stoi(value));
		_case("selection_color"):
			m_selection_color = Color::ParseColor(value);
		_case("dropdown_size"):
			m_max_width = std::stoi(value);
	}
}


}
