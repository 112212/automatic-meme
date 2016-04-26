// Label.hpp
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
#ifndef NG_LABEL_HPP
#define NG_LABEL_HPP
#ifdef USE_SDL
	#include "SDL/Label.hpp"
#elif USE_SFML
#include "../Control.hpp"
namespace ng {
class Label : public Control {
	private:
		std::vector<sf::Text> m_texts;
		std::string m_text;
		int line_height;
		unsigned int characterSize;
		sf::Font fnt;
		void onPositionChange();
		
	public:
		Label();
		~Label();
		
		void Render( sf::RenderTarget& ren, sf::RenderStates state, bool isSelected );
		
		void SetText( const char* text );
		void SetFont( const char* text, int size );
};
}
#endif
#endif
