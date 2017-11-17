#include <algorithm>

#include "ColorString.hpp"
#include "Color.hpp"
#include "managers/Fonts.hpp"
#include "BasicImage.hpp"
#include <cstring>
#include "utf8/utf8.h"

#define USE_UTF8

namespace ng {
static const char* colormap[10] = { 
	"\x1b[0m", // white
	"\x1b[31m", // red
	"\x1b[34m", // blue
	"\x1b[32m", // green
	"\x1b[33m", // yellow
	"\x1b[36m", // cyan
	"\x1b[35m", // purple
};

static const unsigned int i_colormap[] = {
	0xffffffff,
	0xffff0000,
	0xff0000ff,
	0xff00ff00,
	0xffffff00,
	0xff00ffff,
	0xffff00ff
};
const char *c_colormap = "wrbgycp";

ColorString::ColorString() {}

ColorString::ColorString(const std::string& a) {
	process_string(a.c_str());
}

ColorString& ColorString::insert (size_t pos, const std::string& str) {
	*this = csubstr(0,pos) + ColorString(str) + csubstr(pos);
	calc_len();
}

std::string ColorString::GetRawString() {
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

void ColorString::process_string(const char* str) {
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
				
				if(m_attr.size() > 0 && m_attr.back().pos == c) {
					m_attr.back() = a;
				} else {
					m_attr.push_back(a);
				}
			}
		} else {
			m_str[c++] = str[i];
		}
	}
	m_str.resize(c);
	
	calc_len();
}

void ColorString::erase (size_t pos, size_t len) {
	if(pos + len > m_str.size()) return;
	auto begin = m_attr.begin();
	auto end = m_attr.end();
	
	// delete middle part
	auto between_func = [&](const Attribute& a) { return a.pos >= pos && a.pos < pos+len; };
	begin = std::find_if(begin, end, between_func);
	if(begin != m_attr.end()) {
		end = std::find_if_not(begin, end, between_func);
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
	
	calc_len();
}



std::ostream& operator<<(std::ostream& o, const ColorString& s) {
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

ColorString& ColorString::operator+=(const ColorString &b) {
	
	auto begin = b.m_attr.begin();
	if(begin != b.m_attr.end()) {
		int i = m_attr.size();
		int p = utf8_size();
		
		if(i > 0 && m_attr.back().pos == p && begin->pos == 0) {
			begin++;
		}
		
		m_attr.insert(m_attr.end(), begin, b.m_attr.end());
		for(; i < m_attr.size(); i++) {
			m_attr[i].pos += p;
		}
	}
	m_str += b.m_str;
	
	calc_len();
	
	return *this;
}

const char* ColorString::c_str() {
	return m_str.c_str();
}

const std::string& ColorString::str() {
	return m_str;
}

ColorString operator+(const ColorString& a, const ColorString& b) {
	ColorString s(a);
	s += b;
	return s;
}

void ColorString::calc_len() {
	m_str_len = utf8::distance(m_str.begin(), m_str.end());
}

ColorString& ColorString::operator=(const ColorString& b) {
	m_attr = b.m_attr;
	m_str = b.m_str;
	calc_len();
	return *this;
}

std::string ColorString::substr (size_t pos, size_t len, bool passw) const {
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

ColorString ColorString::csubstr (size_t pos, size_t len) const {
	ColorString s;
	if(pos >= m_str.size() && !(m_str.empty() && !m_attr.empty())) return s;
	if(len == std::string::npos) len = m_str.size();
	
	auto begin = m_attr.begin();
	auto end = m_attr.end();
	auto between_func = [&](const Attribute& a) { return a.pos >= pos && a.pos < pos+len; };
	begin = std::find_if(begin, end, between_func);
	end = std::find_if_not(begin, end, between_func);
	
	s.m_attr.insert(s.m_attr.begin(), begin, end);
	
	if(pos+len == m_str.size() && m_attr.size() > 0 && m_attr.back().pos == m_str.size()) {
		s.m_attr.push_back(m_attr.back());
	}
	
	if(pos > 0)
	for(auto& t : s.m_attr) {
		t.pos -= pos;
	}
	
	s.m_str = m_str.substr(pos, len);
	s.calc_len();
	return s;
}

Image* ColorString::get_image(Font* font, int color, bool passw) {
	
	Attribute::Color last_color = (Attribute::Color)color;
	Image* surf;
	if(passw) {
		return font->GetTextImage( std::string(utf8_size(), '*'), i_colormap[0] );
	}
	
	if(m_str.empty() || m_attr.empty()) {
		return font->GetTextImage( m_str, i_colormap[last_color] );
	}
	
	GlyphMetrics g = font->GetGlyphMetrics('A');
	int w = font->GetTextSize(m_str);
	int h = g.height;
	
	surf = new BasicImage( w, h );
	
	int last_pos = 0;
	Rect dst;
	dst.y = dst.x = 0;
	dst.h = h;
	
	for(auto& i : m_attr) {
		if(last_pos >= utf8_size()) break;
		std::string s = utf8_substr(last_pos, i.pos-last_pos);
		if(s.size() > 0) {
			Image* mini_surf = font->GetTextImage( s, i_colormap[last_color] );
			Size s = mini_surf->GetImageSize();
			surf->PutImage(mini_surf, Rect(dst.x, dst.y, s.w, s.h), Rect(0,0,s.w,s.h));
			
			dst.x += s.w;
			delete mini_surf;
		}
		last_color = i.color;
		last_pos = i.pos;
	}
	if(last_pos < utf8_size()) {
		// std::string s = m_str.substr(last_pos);
		std::string s = utf8_substr(last_pos);
		if(s.size() > 0) {
			Image* mini_surf = font->GetTextImage( s, i_colormap[last_color]  );
			Size s = mini_surf->GetImageSize();
			surf->PutImage(mini_surf, Rect(dst.x, dst.y, s.w, s.h), Rect(0,0,s.w,s.h));
			dst.x += s.w;
			delete mini_surf;
		}
	}
	
	return surf;
}

uint32_t ColorString::operator[](int i) {
#ifdef USE_UTF8
	auto it = m_str.begin();
	auto it_end = m_str.end();
	utf8::advance(it, i, it_end);
	return utf8::peek_next(it, it_end);
#else
	return m_str[i];
#endif
}

int ColorString::GetLastColor() {
	return m_attr.empty() ? -1 : m_attr.back().color;
}


size_t ColorString::utf8_size() const {
	return m_str_len;
}

ColorString ColorString::utf8_csubstr(size_t pos, size_t len, bool passw) const {
	ColorString s;
	if(pos >= utf8_size() && !(m_str.empty() && !m_attr.empty())) return s;
	if(len == std::string::npos) len = utf8_size();
	
	auto begin = m_attr.begin();
	auto end = m_attr.end();
	
	auto between_func = [&](const Attribute& a) { return a.pos >= pos && a.pos < pos+len; };
	begin = std::find_if(begin, end, between_func);
	end = std::find_if_not(begin, end, between_func);
	
	s.m_attr.insert(s.m_attr.begin(), begin, end);
	
	if(pos+len == utf8_size() && m_attr.size() > 0 && m_attr.back().pos == utf8_size()) {
		s.m_attr.push_back(m_attr.back());
	}
	
	if(pos > 0) {
		for(auto& t : s.m_attr) {
			t.pos -= pos;
		}
	}
	
	s.m_str = utf8_substr(pos, len);
	s.calc_len();
	return s;
}

std::string ColorString::utf8_substr(size_t pos, size_t len, bool passw) const {
	if(passw) {
		if(len == std::string::npos) {
			return std::string(utf8_size() - pos, '*');
		} else {
			return std::string(std::min<size_t>( len, utf8_size()-pos ), '*');
		}
	} else {
		auto it_start = m_str.begin();
		auto it_s_end = m_str.end();
		utf8::advance(it_start, pos, it_s_end);
		auto it_end = it_start;
		if(len == npos) {
			it_end = it_s_end;
		} else {
			try {
				utf8::advance(it_end, len, it_s_end);
			} catch (std::exception e) {
				
			}
		}
		return std::string(it_start, it_end);
	}
}

void ColorString::utf8_erase (size_t pos, size_t len) {
	if(pos + len > utf8_size()) return;
	auto attr_begin = m_attr.begin();
	auto attr_end = m_attr.end();
	
	auto it_beg = m_str.begin();
	auto it = it_beg;
	const auto it_end = m_str.end();
	utf8::advance(it, pos, it_end);
	size_t utf8_pos = std::distance(it_beg, it);
	
	// delete middle part
	auto between_func = [&](const Attribute& a) { return a.pos >= utf8_pos && a.pos < utf8_pos+len; };
	attr_begin = std::find_if(attr_begin, attr_end, between_func);
	if(attr_begin != m_attr.end()) {
		attr_end = std::find_if_not(attr_begin, attr_end, between_func);
		if(attr_end != attr_begin ) {
			auto it = m_attr.erase(attr_begin,attr_end);
		}
	}
	
	if(len != std::string::npos && pos+len < m_str.size()) {
		attr_begin = std::find_if(m_attr.begin(), m_attr.end(), [&](const Attribute& a) { return a.pos >= pos+len; });
		for(auto it = attr_begin; it != m_attr.end(); it++) {
			it->pos -= len;
		}
	}
	
	auto it_end2 = it;
	
	try {
		utf8::advance(it_end2, len, it_end);
	} catch(std::exception e) {
		
	}
	
	m_str.erase(it, it_end2);
	
	calc_len();
}

std::pair<ColorString::size_type,ColorString::size_type> ColorString::utf8_find(uint32_t val, size_type start_pos) {
	size_type i = start_pos;
	auto it = m_str.begin()+start_pos;
	const auto it_end = m_str.end();
	while(it != it_end) {
		uint32_t it_val = utf8::next(it, it_end);
		if(it_val == val) {
			return {std::distance(m_str.begin(), it), i};
		}
		i++;
	}
	return {npos,npos};
}

std::pair<ColorString::size_type,ColorString::size_type> ColorString::utf8_rfind(uint32_t val, size_type end_pos) {
	size_type i = m_str.size();
	std::string::iterator it;
	if(end_pos != npos) {
		it = m_str.begin()+end_pos;
	} else {
		it = m_str.end();
	}
	auto it_beg = m_str.begin();
	while(it != it_beg) {
		uint32_t it_val = utf8::previous(it, it_beg);
		if(it_val == val) {
			return {std::distance(m_str.begin(), it), i};
		}
		i--;
	}
	return {npos,npos};
}

ColorString::size_type ColorString::utf8_byte_pos(size_type glyph_pos) {
	auto it_beg = m_str.begin();
	auto it = it_beg;
	utf8::advance(it, glyph_pos, m_str.end());
	return std::distance(it_beg, it);
}

ColorString::size_type ColorString::utf8_glyph_pos(size_type byte_pos) {
	return utf8::distance(m_str.begin(), m_str.begin()+byte_pos);
}

const std::string::size_type ColorString::npos = std::string::npos;

}
