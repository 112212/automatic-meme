#include <RapidXML/rapidxml.hpp>
#include "../managers/Images.hpp"
#include "ListBox.hpp"
namespace ng {
ListBox::ListBox() {
	setType( "listbox" );
	
	m_font_height = 13;
	
	m_scrollbar = 0;
	m_selected_index = -1;
	m_drawscrollbar = false;
	m_scrollbar_focus = false;
	
	m_spacing = 0;
	m_last_scroll = 0;
	m_selection_color = 0xff0414CA;
	m_has_pics = false;
	m_has_caption = false;
	m_details = false;
	m_readonly = false;
	m_always_changed = false;
	m_hover_mode = false;
	m_max_items = 0;
}

ListBox::~ListBox() {
	if(m_scrollbar) {
		delete m_scrollbar;
	}
}



void ListBox::Clear() {
	for(Item &t : m_items) {
		t.img->Free();
	}
	m_items.clear();
}


void ListBox::Render( Point pos, bool isSelected ) {
	const Rect& rect = GetRect();
	int x = rect.x + pos.x;
	int y = rect.y + pos.y;
	
	Control::Render(pos,isSelected);
	
	// draw visible scrolled (offset) items
	int h=0,i=0;
	int offs = getListOffset();
	
	// draw caption
	if(m_has_caption) {
		
	}
	
	for(auto it = m_items.cbegin()+offs; it != m_items.cend(); it++,i++) {
		if(i >= m_max_items) {
			break;
		}
		
		Size s = it->img->GetImageSize();
		
		if(m_hover_mode) {
			// draw selection in hover mode
			if(isSelected) {
				Point c = getCursor();
				if(c.y > 0 && c.x > 0 && c.x < rect.w) {
					int tmp_sel = c.y/(m_font_height+m_spacing);
					if(tmp_sel == i) {
						Drawing().FillRect( x, y + h, rect.w - (m_drawscrollbar ? m_scrollrect.w : 0),  m_font_height + m_spacing, m_selection_color );
					}
				}
			}
		} else {
			// draw selection in normal mode
			if(!m_readonly) {
				if(m_selected_index == i+offs) {
					Drawing().FillRect( x, y + h, 
						rect.w - (m_drawscrollbar ? m_scrollrect.w : 0), 
						m_font_height + m_spacing, m_selection_color );
				}
			}
		}
		
		int pic_width = 0;
		if(m_has_pics) {
			pic_width = m_font_height;
			
			// draw small pic
			if(it->pic) {
				Drawing().TexRect( x, y + h + m_spacing/2, pic_width, m_font_height, it->pic);
			}
		}
		
		// draw text
		Drawing().TexRect( x+2 + pic_width, y+h+m_spacing/2, s.w, s.h, it->img );
		h += m_font_height + m_spacing;
	}
	
	RenderWidget(pos, isSelected);
}

Rect ListBox::GetContentRect() {
	if(!m_items.empty()) {
		Size s = m_items.front().img->GetImageSize();
		int h = m_items.size()*s.h;
		int w = 0;
		for(auto i : m_items) {
			s = i.img->GetImageSize();
			w=std::max(s.w, w);
		}
		return Rect(0, 0, w, h);
	}
	return Rect(0, 0, 0, 0);
}

void ListBox::AddItem( std::string str, std::string value, Image* smallimg ) {
	std::string item_text = m_style.font->ClipText( str, GetRect().w );
	// std::cout << item_text << " => " << GetRect().w << "\n";
	Image* img = m_style.font->GetTextImage( item_text, 0xffffff );
	Item item;
	item.str = str;
	item.val = value;
	item.img = img;
	item.pic = smallimg;
	m_items.push_back( item );
	if(m_selected_index == -1) {
		m_selected_index = 0;
		emitEvent( "change", {m_selected_index} );
	}
	updateBox();
}


void ListBox::SetSmallImg(int idx, Image* smallimg) {
	m_items[idx].pic = smallimg;
}

void ListBox::AddItem( std::string item, Image* smallimg ) {
	AddItem(item);
	m_items.back().pic = smallimg;
}

void ListBox::OnMouseDown( int mX, int mY, MouseButton which_button ) {
	int tmp = (mY)/(m_font_height+m_spacing) + getListOffset();
	if((m_always_changed || tmp != m_selected_index) && tmp < m_items.size()) {
		// m_selected_index = tmp;
		// emitEvent( "change", {m_selected_index} );
	}
}

void ListBox::OnMouseUp( int mX, int mY, MouseButton which_button ) {
	int tmp = (mY)/(m_font_height+m_spacing) + getListOffset();
	if((m_always_changed || tmp != m_selected_index) && tmp < m_items.size()) {
		int old_idx = m_selected_index;
		m_selected_index = tmp;
		// std::cout << "idx: " << tmp << "\n";
		emitEvent( "change", {m_selected_index} );
	}
}

void ListBox::OnMouseMove( int mX, int mY, bool mouseState ) {
}

void ListBox::SetValue(int idx, std::string val) {
	if(idx < m_items.size()) {
		m_items[idx].val = val;
		updateBox();
	}
}


void ListBox::OnLostFocus() {
}

void ListBox::onRectChange() {
	updateItemSizes();
	updateBox();
}

void ListBox::onFontChange() {
	updateItemSizes();
}


void ListBox::OnGetFocus() {
}

void ListBox::RemoveItem(int idx) {
	m_items.erase(m_items.begin()+idx);
	if(m_selected_index >= m_items.size()) {
		m_selected_index = m_items.size() - 1;
	}
	emitEvent( "change", {m_selected_index} );
}

int ListBox::getAverageHeight() {
	int h=0,num=0;
	for(auto &it : m_items) {
		Size s = (it.img)->GetImageSize();
		if(h >= GetRect().h) {
			break;
		}
		h += s.h;
		num++;
	}
	if(h == 0) {
		return 1;
	} else {
		return h / num;
	}
}
#define dbg(x) x
void ListBox::updateBox() {
	const Rect& rect = GetRect();
	if(rect.h == 0 || m_font_height == 0) {
		return;
	}
	
	m_font_height = getAverageHeight();
	m_max_items = rect.h / m_font_height;
	// dbg(std::cout << "upd box: " << rect.h << " " << m_max_items << " : "<< GetRect() << "\n";)
	
	if(m_items.size() > m_max_items) {
		int scrollbar_width = 10;
		if(!m_scrollbar) {
			m_scrollbar = createControl<ScrollBar>("scrollbar", "vscroll");
			m_scrollbar->SetVertical( true );
			m_scrollbar->SetLayout(getString("AR,0,%d,H", scrollbar_width));
			m_scrollbar->OnEvent("change", [&](Args a) {
				m_last_scroll = m_scrollbar->GetValue();
			});
			AddControl(m_scrollbar,false);
			ProcessLayout();
		}
		m_scrollbar->SetVisible(true);
		
		m_scrollrect = m_scrollbar->GetRect();
		m_scrollbar->SetSliderSize( clip<int>((m_max_items*100)/m_items.size(), 10, m_scrollrect.h - 10) );
		m_scrollbar->SetRange( 0, m_items.size() - m_max_items );
		m_scrollbar->SetMouseWheelConstant( std::max<int>( m_max_items/3, 1 ) );
		m_drawscrollbar = true;
	} else if(m_scrollbar) {
		m_scrollbar->SetVisible(false);
		m_drawscrollbar = false;
	}
}

void ListBox::OnMWheel( int updown ) {
	if(m_drawscrollbar) {
		m_scrollbar->OnMWheel( updown );
		m_last_scroll = m_scrollbar->GetValue();
	}
}

int ListBox::getListOffset() {
	if(m_drawscrollbar) {
		return m_last_scroll;
	} else {
		return 0;
	}
}

void ListBox::updateItemSizes() {
	std::string tmp;
	for(int i=0; i < m_items.size(); i++) {
		tmp = m_style.font->ClipText( m_items[i].str, GetRect().w );
		delete m_items[i].img;
		m_items[i].img = m_style.font->GetTextImage( tmp, 0xffffff );
	}
	// updateBox();
}


int ListBox::GetSelectedIndex() {
	return m_selected_index;
}

std::string ListBox::GetText() {
	if(m_selected_index == -1) {
		return "";
	} else {
		return m_items[ m_selected_index ].str;
	}
}


void ListBox::OnKeyDown( Keycode sym, Keymod mod ) {
	int tmp=-1;
	if(sym == KEY_DOWN) {
		tmp = std::min<int>(m_selected_index+1,m_items.size()-1);
	} else if(sym == KEY_UP) {
		tmp = std::max(m_selected_index-1,0);
	}
	
	
	if((m_always_changed || tmp != m_selected_index) && (tmp != -1) && tmp < m_items.size()) {
		m_selected_index = tmp;
		emitEvent( "change", {m_selected_index} );
	}
}



std::string ListBox::GetValue() {
	if(m_selected_index == -1) {
		return "";
	} else {
		return m_items[ m_selected_index ].val;
	}
}

void ListBox::SetSelectedIndex( int index ) {
	m_selected_index = index;
}

void ListBox::OnSetStyle(std::string& style, std::string& value) {
	STYLE_SWITCH {
		_case("value"):
			SetSelectedIndex(std::stoi(value));
		_case("showpic"):
			m_has_pics = toBool(value);
		_case("spacing"):
			m_spacing = std::stoi(value);
		_case("details"):
			m_details = toBool(value);
		_case("caption"):
			m_caption = value;
			m_has_caption = true;
		_case("readonly"):
			m_readonly = toBool(value);
		_case("selection_color"):
			m_selection_color = Color(value).GetUint32();
		_case("always_change"):
			m_always_changed = toBool(value);
		_case("hover_selection_mode"):
			m_hover_mode = toBool(value);
	}
}

ListBox* ListBox::Clone() {
	ListBox* lb = new ListBox();
	copyStyle(lb);
	return lb;
}


void ListBox::parseXml(rapidxml::xml_node<char>* node) {
	for(;node;node=node->next_sibling()) {
		if(std::string(node->name()) == "item") {
			Image* img = 0;
			std::string val="";
			for(auto attr = node->first_attribute(); attr; attr = attr->next_attribute()) {
				auto a = std::string(attr->name());
				if(a == "img") {
					img = Images::GetImage( attr->value() );
				}
				if(a == "value") {
					val = attr->value();
				}
			}
			AddItem(node->value(), val, img);
		}
	}
}

}



