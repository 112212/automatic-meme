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
#ifndef SDL_NG_LABEL_HPP
#define SDL_NG_LABEL_HPP

#include "../../Control.hpp"
namespace ng {
class Label : public Control {
	private:
		std::string m_text;
		
		struct TextLine{
			Uint32 tex;
			int w,h;
		};
		
		std::vector<TextLine> text_lines;
		
		TTF_Font* m_font;
		void STYLE_FUNC(value);
		
		void Render( Point pos, bool isSelected );
	public:
		Label();
		~Label();

		void SetText( std::string text );
		void SetFont( std::string text, int size );
};
}
#endif
