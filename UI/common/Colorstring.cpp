#include "Colorstring.hpp"

#include "Colors.hpp"
#include "Fonts.hpp"

#include <SDL2/SDL_opengl.h>
#include <SDL2/SDL_ttf.h>

const char* colormap[10] = { 
	"\x1b[0m", // white
	"\x1b[31m", // red
	"\x1b[34m", // blue
	"\x1b[32m", // green
	"\x1b[33m", // yellow
	"\x1b[36m", // cyan
	"\x1b[35m", // purple
};

const uint i_colormap[] = {
	0xffffffff,
	0xffff0000,
	0xff0000ff,
	0xff00ff00,
	0xffffff00,
	0xff00ffff,
	0xffff00ff
};

const char *c_colormap = "wrbgycp";
	
Colorstring& Colorstring::insert (size_t pos, const std::string& str) {
	Colorstring s(str.c_str());
	auto it_ins = std::find_if(m_attr.begin(), m_attr.end(), [pos](const Attribute& a) {
		return a.pos >= pos;
	});
	for(auto it = it_ins; it != m_attr.end(); it++) {
		if(it->pos >= pos) {
			it->pos += s.size();
		}
	}
	for(auto &a : s.m_attr) a.pos += pos;
	m_attr.insert(it_ins, s.m_attr.begin(), s.m_attr.end());
	m_str.insert(pos, s.m_str);
}


std::string Colorstring::GetRawString() {
		std::string s;
		s.resize( m_attr.size()*2 + m_str.size() );
		int c = 0;
		int i = 0;
		for(auto& a : m_attr) {
			for(; i < a.pos; i++) {
				s[c++] = m_str[i];
			}
			s[c] = '^'; 
			s[c+1] = c_colormap[a.color];
			c+=2;
		}
		for(; i < m_str.size(); i++) {
			s[c++] = m_str[i];
		}
		return s;
	}
	

void Colorstring::process_string(const char* str) {
	const char* s = str;
	int len;
	for(len=0; *s; s++,len++);
	m_str.resize(len);
	int c=0;
	for(int i=0; i < len; i++) {
		if(str[i] == '^' && i+1 != len) {
			if(str[i+1] == '^') {
				m_str[c++] = str[i++];
			} else {
				
				Attribute a;
				a.pos = c;
				switch(str[++i]) {
					case 'w': a.color = Attribute::Color::white; break;
					case 'r': a.color = Attribute::Color::red; break;
					case 'g': a.color = Attribute::Color::green; break;
					case 'b': a.color = Attribute::Color::blue; break;
					case 'y': a.color = Attribute::Color::yellow; break;
					case 'c': a.color = Attribute::Color::cyan; break;
					case 'p': a.color = Attribute::Color::purple; break;
				}
				
				if(m_attr.size() > 0 && m_attr.back().pos == c)
					m_attr.back() = a;
				else
					m_attr.push_back(a);
			}
		} else {
			m_str[c++] = str[i];
		}
	}
	m_str.resize(c);
}

Colorstring& Colorstring::erase (size_t pos, size_t len) {
	std::vector<Attribute>::iterator begin = m_attr.begin();
	std::vector<Attribute>::iterator end = m_attr.end();
	for(auto it = begin; it != m_attr.end(); it++) {
		if(it->pos >= pos && it->pos < pos+len) {
			begin = it;
			break;
		}
	}
	for(auto it = end; it != begin; it--) {
		if((it->pos >= pos && it->pos < pos+len)) {
			end = it;
			break;
		}
	}
	auto it = m_attr.erase(begin,end+1);
	if(len != std::string::npos) {
		for(; it != m_attr.end(); it++) {
			it->pos -= len;
		}
	}
	m_str.erase(pos,len);
}


std::ostream& operator<<(std::ostream& o, const Colorstring& s) {
	int last_pos = 0;
	for(auto &i : s.m_attr) {
		o << s.m_str.substr(last_pos,i.pos-last_pos);
		o << colormap[i.color];
		last_pos = i.pos;
	}
	o << s.m_str.substr(last_pos) << colormap[0];
	return o;
}

GLuint Colorstring::get_texture(TTF_Font* font) {
	/*
	int last_pos = 0;
	for(auto &i : s.m_attr) {
		o << s.m_str.substr(last_pos,i.pos-last_pos);
		o << s.colormap[i.color];
		last_pos = i.pos;
	}
	o << s.m_str.substr(last_pos) << s.colormap[0];
	return o;
	*/
	int w = ng::Fonts::getTextSize(font, m_str);
	int h = TTF_FontHeight(font);
	SDL_Surface* surf = SDL_CreateRGBSurface(0, w, h, 8, 0xff0000, 0x00ff00, 0x0000ff, 0xff000000);
	int last_pos = 0;
	SDL_Rect dst;
	dst.y = dst.x = 0;
	dst.h = h;
	for(auto& i : m_attr) {
		std::string s = m_str.substr(last_pos,i.pos-last_pos);
		if(s.size() > 0) {
			SDL_Surface* mini_surf = TTF_RenderUTF8_Blended( font, s.c_str(), Colors::toSDL_Color(i_colormap[i.color]) );
			SDL_BlitSurface(mini_surf, nullptr, surf, &dst);
			dst.x += mini_surf->w;
			/*
			int SDL_BlitSurface(SDL_Surface*    src,
				const SDL_Rect* srcrect,
				SDL_Surface*    dst,
				SDL_Rect*       dstrect)
			*/

		}
	}
	
}
