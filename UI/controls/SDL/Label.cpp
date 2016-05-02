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
	setType( TYPE_LABEL );
	setInteractible(false);
	m_font = Fonts::GetFont( "default", 13 );
	
	//m_text = stri("");
	//~ m_surf_text = 0;
}

Label::~Label() {
	//~ if(m_surf_text)
		//~ SDL_FreeSurface( m_surf_text );
}


void Label::Render( SDL_Rect pos, bool isSelected ) {
	//~ if(m_surf_text)
		//~ CSurface::OnDraw( surf, m_surf_text, m_rect.x, m_rect.y+5 );
	// vector<SDL_Surface*>::iterator
	int j=0;
	for( auto i = text_lines.begin(); i != text_lines.end(); i++,j++) {
		// TODO: fix this
		// CSurface::OnDraw( ren, *i, m_rect.x, m_rect.y+5+j*line_height );
		Drawing::TexRect( m_rect.x, m_rect.y+5+j*line_height, i->w, i->h, i->tex);
	}
}

void Label::SetText( const char* text ) {

	m_text = text;
	//~ if(m_surf_text)
		//~ SDL_FreeSurface( m_surf_text );
	
	// multiline check
	int dummy, advance,last_cut=0,sum=0,i,len = m_text.size();
	char save;
	int j = 0;
	
	TTF_Font* fnt = m_font;
	for(i=0; i < len; i++) {
		TTF_GlyphMetrics( fnt, m_text[i], &dummy, &dummy, &dummy, &dummy, &advance);
		if( m_text[i] == '\n' or sum > m_rect.w-25) {
			
			// TODO: odseci
			save = m_text[i];
			m_text[i] = 0;
			
			SDL_Surface* surf = TTF_RenderText_Blended( m_font, &m_text[last_cut], {255,255,255} );
			if(j < text_lines.size()) {
				text_lines[j++] = { Drawing::GetTextureFromSurface(surf, text_lines[j].tex),
					surf->w, surf->h };
			} else {
				text_lines.push_back( { Drawing::GetTextureFromSurface(surf, 0),
					surf->w, surf->h } );
			}
			SDL_FreeSurface(surf);
			
			last_cut = i+1;
			m_text[i] = save;
			
			sum = 0;
		}
		sum += advance;
	}
	save = m_text[i];
	m_text[i] = 0;
	
	SDL_Surface* surf = TTF_RenderText_Blended( m_font, &m_text[last_cut], {255,255,255} );
	if(j < text_lines.size()) {
		text_lines[j++] = { Drawing::GetTextureFromSurface(surf, text_lines[j].tex),
			surf->w, surf->h };
	} else {
		text_lines.push_back( { Drawing::GetTextureFromSurface(surf, 0),
			surf->w, surf->h } );
	}
	SDL_FreeSurface(surf);
	last_cut = i;
	m_text[i] = save;
	
	//~ m_surf_text = TTF_RenderText_Solid( m_font, m_text, CColors::s_white );
}

//~ int TextBox::getMaxText( ) {
	//~ TTF_Font* fnt = m_font;
	//~ 
	//~ int dummy, advance;
	//~ int len = m_text.length() - m_first_index;
	//~ int sum=0;
	//~ 
	//~ for(int i=0; i < len; i++) {
		//~ TTF_GlyphMetrics( fnt, m_text[m_first_index+i], &dummy, &dummy, &dummy, &dummy, &advance);
		//~ if( sum > m_rect.w-25 ) {
			//~ return i+1;
		//~ }
		//~ sum += advance;
	//~ }
	//~ return len;
//~ }

const int Label::line_height = 20;

void Label::SetFont( const char* text, int size ) {
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
