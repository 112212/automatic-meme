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

namespace ng {

Label::Label() {
	setType( TYPE_LABEL );
	setInteractible(false);
	fnt = Fonts::GetFont( "default" );
}

Label::~Label() {
}


void Label::Render( sf::RenderTarget& ren, sf::RenderStates state, bool isSelected ) {
	for( auto &i : m_texts ) {
		ren.draw( i,state );
	}
}

void Label::SetText( const char* text ) {

	m_text = text;
	m_texts.clear();

	// do the line wrappi
	line_height = fnt.getLineSpaci( characterSize );
	int sum = 0, save, last_ind = 0, line = 0;
	int len = m_text.size();
	sf::Text txt;
	for(int i = 0; i < len; i++) {
		const sf::Glyph& g = fnt.getGlyph( text[i], characterSize, false );
		if(m_text[i] == '\n' or sum > m_rect.w - 25) {
			txt.setStri( m_text.substr( last_ind, i - last_ind ) );
			txt.setFont( fnt );
			txt.setCharacterSize( characterSize );
			txt.setPosition( m_rect.x, m_rect.y + line*line_height );
			m_texts.push_back( txt );
			last_ind = i;
			line++;
			sum = 0;
		}
		sum += g.advance;
	}
	txt.setString( m_text.substr( last_ind, m_texts.size() - last_ind ) );
	txt.setFont( fnt );
	txt.setCharacterSize( characterSize );
	txt.setPosition( m_rect.x, m_rect.y + line*line_height );
	m_texts.push_back( txt );
}

void Label::onPositionChange() {
	for( int i = 0; i < m_texts.size(); i++ ) {
		sf::Text &t = m_texts[i];
		t.setPosition( m_rect.x, m_rect.y + line_height*i);
	}
}

void Label::SetFont( const char* text, int size ) {
	fnt = Fonts::GetFont( text );
	characterSize = size;
	for(auto &i : m_texts) {
		i.setCharacterSize( characterSize );
	}
}

void Label::OnSetStyle(std::string& style, std::string& value) {
	if(style == "text")
		SetText(value.c_str());
}

}
