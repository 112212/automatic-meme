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

const unsigned int i_colormap[] = {
	0xffffffff,
	0xffff0000,
	0xff0000ff,
	0xff00ff00,
	0xffffff00,
	0xff00ffff,
	0xffff00ff
};

const char *c_colormap = "wrbgycp";

Colorstring::Colorstring() {}

Colorstring::Colorstring(const std::string& a) {
	process_string(a.c_str());
}

Colorstring& Colorstring::insert (size_t pos, const std::string& str) {
	*this = csubstr(0,pos) + Colorstring(str) + csubstr(pos);
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
	int len = strlen(str);
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
					default: 
						m_str[c++] = '^';
						continue;
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

void Colorstring::erase (size_t pos, size_t len) {
	if(pos + len > m_str.size()) return;
	auto begin = m_attr.begin();
	auto end = m_attr.end();
	
	// delete middle part
	begin = std::find_if(begin, end, [&](const Attribute& a) { return a.pos >= pos && a.pos < pos+len; });
	if(begin != m_attr.end()) {
		end = std::find_if_not(begin, end, [&](const Attribute& a) { return a.pos >= pos && a.pos < pos+len; });
		if(end != begin ) {
			auto it = m_attr.erase(begin,end);
		}
	}
	
	
	if(len != std::string::npos && pos+len < m_str.size()) {
		begin = std::find_if(m_attr.begin(), m_attr.end(), [&](const Attribute& a) { return a.pos >= pos+len; });
		for(auto it = begin; it != m_attr.end(); it++) {
			it->pos -= len;
		}
	}
	
	m_str.erase(pos,len);
}



std::ostream& operator<<(std::ostream& o, const Colorstring& s) {
	int last_pos = 0;
	for(auto &i : s.m_attr) {
		if(last_pos >= s.m_str.size()) break;
		o << s.m_str.substr(last_pos,i.pos-last_pos);
		o << colormap[i.color];
		last_pos = i.pos;
	}
	if(last_pos < s.m_str.size())
	o << s.m_str.substr(last_pos);
	o << colormap[0];
	return o;
}

Colorstring& Colorstring::operator+=(const Colorstring &b) {
	
	auto begin = b.m_attr.begin();
	if(begin != b.m_attr.end()) {
		int i = m_attr.size();
		int p = m_str.size();
		
		if(i > 0 && m_attr.back().pos == p && begin->pos == 0) {
			begin++;
		}
		
		m_attr.insert(m_attr.end(), begin, b.m_attr.end());
		for(; i < m_attr.size(); i++)
			m_attr[i].pos += p;
	}
	m_str += b.m_str;
	
	return *this;
}

const char* Colorstring::c_str() {
	return m_str.c_str();
}

const std::string& Colorstring::str() {
	return m_str;
}

Colorstring operator+(const Colorstring& a, const Colorstring& b) {
	Colorstring s(a);
	s += b;
	return s;
}



Colorstring& Colorstring::operator=(const Colorstring& b) {
	m_attr = b.m_attr;
	m_str = b.m_str;
	return *this;
}

std::string Colorstring::substr (size_t pos, size_t len, bool passw) const {
	if(passw) {
		std::string s;
		if(len == std::string::npos)
			s.resize(m_str.size() - pos);
		else
			s.resize( std::min<size_t>( len, m_str.size()-pos ) );
		for(auto& i : s) i='*';
		return s;
	} else {
		return m_str.substr(pos, len);
	}
}

Colorstring Colorstring::csubstr (size_t pos, size_t len) const {
	Colorstring s;
	if(pos >= m_str.size() && !(m_str.empty() && !m_attr.empty())) return s;
	if(len == std::string::npos) len = m_str.size();
	
	auto begin = m_attr.begin();
	auto end = m_attr.end();
	begin = std::find_if(begin, end, [&](const Attribute& a) { return a.pos >= pos && a.pos < pos+len; });
	end = std::find_if_not(begin, end, [&](const Attribute& a) { return a.pos >= pos && a.pos < pos+len; });
	
	s.m_attr.insert(s.m_attr.begin(), begin, end);
	
	if(pos+len == m_str.size() && m_attr.size() > 0 && m_attr.back().pos == m_str.size()) {
		s.m_attr.push_back(m_attr.back());
	}
	
	if(pos > 0)
	for(auto& t : s.m_attr) {
		t.pos -= pos;
	}
	
	s.m_str = m_str.substr(pos, len);
	return s;
}

SDL_Surface* Colorstring::get_surface(TTF_Font* font, int color, bool passw) {
	Attribute::Color last_color = (Attribute::Color)color;
	SDL_Surface* surf;
	if(passw) {
		std::string s;
		s.resize( m_str.size() );
		for(auto& i : s) i='*';
		return TTF_RenderUTF8_Blended( font, s.empty() ? " " : s.c_str(), Colors::toSDL_Color(i_colormap[0]) );
	}
	if(m_str.empty() || m_attr.empty())
		return TTF_RenderUTF8_Blended( font, m_str.empty() ? " " : m_str.c_str(), Colors::toSDL_Color(i_colormap[last_color]) );
		
	int w = ng::Fonts::getTextSize(font, m_str);
	int h = TTF_FontHeight(font);
	
	surf = SDL_CreateRGBSurface(0, w, h, 32, 0xff0000, 0x00ff00, 0x0000ff, 0xff000000);
	
	int last_pos = 0;
	SDL_Rect dst;
	dst.y = dst.x = 0;
	dst.h = h;
	
	for(auto& i : m_attr) {
		if(last_pos >= m_str.size()) break;
		std::string s = m_str.substr(last_pos, i.pos-last_pos);
		if(s.size() > 0) {
			SDL_Surface* mini_surf = TTF_RenderUTF8_Blended( font, s.c_str(), Colors::toSDL_Color(i_colormap[last_color]) );
			SDL_BlitSurface(mini_surf, nullptr, surf, &dst);
			dst.x += mini_surf->w;
			SDL_FreeSurface(mini_surf);
		}
		last_color = i.color;
		last_pos = i.pos;
	}
	if(last_pos < m_str.size()) {
		std::string s = m_str.substr(last_pos);
		if(s.size() > 0) {
			SDL_Surface* mini_surf = TTF_RenderUTF8_Blended( font, s.c_str(), Colors::toSDL_Color(i_colormap[last_color]) );
			SDL_BlitSurface(mini_surf, nullptr, surf, &dst);
			dst.x += mini_surf->w;
			SDL_FreeSurface(mini_surf);
		}
	}
	
	return surf;
}

int Colorstring::GetLastColor() {
	return m_attr.empty() ? -1 : m_attr.back().color;
}

