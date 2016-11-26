// Label.cpp
// 
// Copyright 2013 nikola <sciliquant@gmail.com>
// 
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// alo with this program; if not, write to the Free Software
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
// MA 02110-1301, USA.
// 
// 
#include "Label.hpp"
#include "../../common/debug.hpp"
#include "../../common/SDL/Drawing.hpp"
#include "../../common/Colorstring.hpp"
namespace ng {
Label::Label() {
	setType(TYPE_LABEL);
	setInteractible(false);
	text_lines.clear();
}

Label::~Label() {
}


void Label::Render( Point pos, bool isSelected ) {
	Control::Render(pos, isSelected);
	const Rect& rect = GetRect();
	int j=0;
	int line_height = TTF_FontHeight(m_style.font)-5;
	for( auto i = text_lines.begin(); i != text_lines.end(); i++,j++) {
		if(5+j*line_height+i->h > rect.h) break;
		Drawing::TexRect( rect.x, rect.y+5+j*line_height, i->w, i->h, i->tex);
	}
}

static void find_and_replace(std::string& source, std::string const& find, std::string const& replace) {
    for(std::string::size_type i = 0; (i = source.find(find, i)) != std::string::npos; i += replace.length()) {
        source.replace(i, find.length(), replace);
    }
}



void Label::SetText( std::string text ) {
	m_text = text;
	
	// replace tabs with 4 spaces
	find_and_replace(m_text, "\t", "    ");
	
	int j = 0;
	
	int max_text_width = GetRect().w-25;
	int last_color = 0;
	for(std::string::size_type pos = 0; pos < m_text.size();) {
		
		std::string s = m_text.substr(pos);
		// if(s.size() == 1) {
			// cout <<s 
		// }
		int max_text = Fonts::getMaxText(m_style.font, s, max_text_width);
		// cout << "(" << pos << ") doing: " << s << endl;
		int p = s.find('\n');
		if(p != s.npos)
			max_text = p;
		
		Colorstring cstr;
		if(max_text < s.size())
			cstr = s.substr(0,max_text).c_str();
		else
			cstr = s.c_str();
		
		SDL_Surface* surf = cstr.get_surface(m_style.font, last_color);
		int ncol = cstr.GetLastColor();
		if(ncol > 0)
			last_color = ncol;
		if(j < text_lines.size()) {
			text_lines[j] = { Drawing::GetTextureFromSurface(surf, text_lines[j].tex), surf->w, surf->h };
			j++;
		} else {
			text_lines.push_back( { Drawing::GetTextureFromSurface(surf, 0), surf->w, surf->h } );
			j++;
		}
		SDL_FreeSurface(surf);
		
		// cout << j << " ... " << pos << " : " << m_text.size() << " : " << max_text << ", " << s.size() << "\n";
		if(max_text < s.size() && max_text != 0)
			pos += max_text;
		else {
			pos = m_text.find('\n', pos);
			if(pos == m_text.npos) 
				break;
			pos++;
		}
	}
	
}


Control* Label::Clone() {
	Label* l = new Label;
	*l = *this;
	return l;
}

void Label::STYLE_FUNC(value) {
	STYLE_SWITCH {
		_case("value"):
			SetText(value.c_str());
	}
}
}
