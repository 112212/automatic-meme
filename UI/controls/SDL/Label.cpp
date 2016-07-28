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
namespace ng {
Label::Label() {
	setType(TYPE_LABEL);
	setInteractible(false);
	m_font = Fonts::GetFont("default", 13);
	text_lines.clear();
}

Label::~Label() {
}


void Label::Render( SDL_Rect pos, bool isSelected ) {
	int j=0;
	int line_height = TTF_FontHeight(m_font);
	for( auto i = text_lines.begin(); i != text_lines.end(); i++,j++) {
		if(5+j*line_height+i->h > m_rect.h) break;
		Drawing::TexRect( m_rect.x, m_rect.y+5+j*line_height, i->w, i->h, i->tex);
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
	
	int dummy, advance, last_cut=0, sum=0, i, len = m_text.size();
	char save;
	int j = 0;
	
	int max_text_width = m_rect.w-25;

	TTF_Font* fnt = m_font;
	for(i=0; i <= len; i++) {
		TTF_GlyphMetrics( fnt, m_text[i], &dummy, &dummy, &dummy, &dummy, &advance);
		if( m_text[i] == '\n' or sum > max_text_width or i == len) {
			
			save = m_text[i];
			if(m_text[last_cut] == '\n') {
				last_cut = i+1;
				continue;
			}
			
			m_text[i] = 0;
			
			SDL_Surface* surf = TTF_RenderText_Blended( m_font, &m_text[last_cut], {255,255,255} );
			if(j < text_lines.size()) {
				text_lines[j++] = { Drawing::GetTextureFromSurface(surf, text_lines[j].tex), surf->w, surf->h };
			} else {
				text_lines.push_back( { Drawing::GetTextureFromSurface(surf, 0), surf->w, surf->h } );
				j++;
			}
			SDL_FreeSurface(surf);
			
			m_text[i] = save;
			
			last_cut = i+1;
			sum = 0;
		}
		sum += advance;
	}

}

void Label::SetFont( std::string text, int size ) {
	TTF_Font* fnt = Fonts::GetFont( text, size );
	if(fnt) {
		m_font = fnt;
	}
}

void Label::STYLE_FUNC(value) {
	STYLE_SWITCH {
		_case("value"):
			SetText(value.c_str());
	}
}
}
