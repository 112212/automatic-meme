#include "Label.hpp"
#include "../ColorString.hpp"

namespace ng {
Label::Label(std::string id) {
	SetId(id);
	setType( "label" );
	setInteractible(false);
	text_lines.clear();
	m_alignment = Alignment::left;
	m_angle = 0;
}

Label::~Label() {
}

Rect Label::GetContentRect() {
	Rect r;
	r.x = 0;
	r.y = 0;
	
	r.w = 0; 
	r.h = 0;
	for(auto i : text_lines) {
		Size s = i->GetImageSize();
		r.w = std::max(r.w, s.w+5);
		r.h += s.h;
	}
	return r;
}

void Label::Render( Point pos, bool isSelected ) {
	if(!m_style.font) return;
	const Rect& rect = GetRect();
	Drawing().SetRotation(m_angle, rect.x+rect.w/2, rect.y+rect.h/2);
	Control::RenderBase(pos, isSelected);
	int j=0;

	GlyphMetrics g = m_style.font->GetGlyphMetrics('A');
	int ofs = 0;
	int line_height = g.height-4;
	int lh = rect.h / line_height;
	int y = pos.y + rect.y;
	lh = std::min<int>(text_lines.size(), lh);
	ofs = (rect.h - lh*line_height)/2-2;
	for( auto i = text_lines.begin(); i != text_lines.end(); i++,j++) {
		Size s = (*i)->GetImageSize();
		if(j*line_height+s.h > rect.h) break;
		if(m_alignment == Alignment::left) {
			Drawing().TexRect( pos.x + rect.x, y + ofs + j*line_height, s.w, s.h, *i);
		} else if(m_alignment == Alignment::right) {
			Drawing().TexRect( pos.x + rect.x + rect.w - s.w, y + ofs + j*line_height, s.w, s.h, *i);
		} else if(m_alignment == Alignment::center) {
			Drawing().TexRect( pos.x + rect.x + (rect.w - s.w)/2, y + ofs + j*line_height, s.w, s.h, *i);
		}
	}
	
	Control::RenderWidget(pos, isSelected);
	Drawing().SetRotation(0.0f);
}

void Label::AppendText( std::string text ) {
	m_text += text;
	// TODO: optimize
	SetText(m_text.GetRawString());
}

void Label::SetText( std::string new_text ) {
	doOnRender([=]() {
		std::string text = new_text;
		// replace tabs with 4 spaces
		find_and_replace(text, "\t", "    ");
		m_text = text;
		
		std::vector<Image*> new_text_lines;
		
		if(!m_style.font) {
			return;
		}
		
		int j = 0;
		
		int max_text_width = GetRect().w-5;
		int last_color = 0;
		for(std::string::size_type pos = 0; pos < m_text.size(); ) {
			
			ColorString cstr = m_text.utf8_csubstr(pos);
			
			int max_text = m_style.font->GetMaxText(cstr.str(), max_text_width);
			int p = cstr.utf8_find('\n').second;
			if(p != std::string::npos) {
				max_text = std::min(max_text, p);
			}
			
			if(max_text < cstr.utf8_size()) {
				cstr = cstr.utf8_csubstr(0, max_text);
			}
			
			Image* img = cstr.get_image( m_style.font, m_style.color != 0 ? m_style.color : last_color );
			if(!img) {
				return;
			}

			int ncol = cstr.GetLastColor();
			if(ncol > 0) {
				last_color = ncol;
			}
			if(j < new_text_lines.size()) {
				delete new_text_lines[j];
				new_text_lines[j] = img;
				j++;
			} else {
				new_text_lines.push_back( img );
				j++;
			}
			
			if(max_text < cstr.utf8_size() && max_text != 0) {
				pos += max_text;
			} else {
				pos = m_text.utf8_find('\n', pos).second;
				if(pos == m_text.npos) {
					break;
				}
				pos++;
			}
		}
		
		text_lines.swap(new_text_lines);
		for(auto i : new_text_lines) {
			delete i;
		}
	});
}

void Label::onFontChange() {
	SetText(m_text.GetRawString());
}

void Label::onRectChange() {
	SetText(m_text.GetRawString());
}

void Label::SetAlignment( Alignment alignment ) {
	m_alignment = alignment;
}

Control* Label::Clone() {
	Label* l = new Label();
	copyStyle(l);
	return l;
}

void Label::OnSetStyle(std::string& style, std::string& value) {
	STYLE_SWITCH {
		_case("value"):
			SetText(value.c_str());
		_case("align"):
			if(value == "left") {
				SetAlignment( Alignment::left );
			} else if(value == "right") {
				SetAlignment( Alignment::right );
			} else if(value == "center") {
				SetAlignment( Alignment::center );
			}
			break;
			
		_case("rotation"):
			SetRotation(std::stof(value));
	}
}

void Label::SetRotation(int angle) {
	m_angle = angle;
}

int Label::GetRotation() {
	return m_angle;
}

}
