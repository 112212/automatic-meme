#ifndef NG_COLORSTRING_HPP
#define NG_COLORSTRING_HPP
#include <string>
#include <vector>
#include <iostream>
#include <utility>

#include "Font.hpp"

namespace ng {
class ColorString {
	public:
	ColorString();
	ColorString& operator=(const char* s) { 
		process_string(s);
		return *this; 
	}
	
	ColorString(const char* s) {
		process_string(s);
	}
	
	ColorString(const std::string& a);
	
	friend std::ostream& operator<<(std::ostream& o, const ColorString& s);
	
	const char* c_str();
	const std::string& str();
	
	size_t size() const { return m_str.size(); }
	size_t utf8_size() const;
	size_t length() const { return m_str.size(); }
	bool empty() const { return m_str.empty(); }
	
	void erase (size_t pos = 0, size_t len = std::string::npos);
	void utf8_erase (size_t pos = 0, size_t len = std::string::npos);
	
	ColorString& operator=(const std::string& str) {
		m_attr.clear();
		m_str.clear();
		process_string(str.c_str());
		return *this;
	}
	
	ColorString& operator=(const ColorString& b);
	ColorString& operator+=(const ColorString &b);
	
	ColorString& operator+=(const std::string &b) {
		*this += ColorString(b);
		return *this;
	}
	
	// friend std::string operator+(const char* a, const ColorString& b);
	friend ColorString operator+(const ColorString& a, const ColorString& b);
	// friend std::string operator+(const std::string& a, const ColorString& b);
	
	uint32_t operator[](int i);
	
	
	int GetLastColor();
	std::string substr (size_t pos = 0, size_t len = std::string::npos, bool passw=false) const;
	std::string utf8_substr(size_t pos = 0, size_t len = std::string::npos, bool passw=false) const;
	
	ColorString csubstr (size_t pos = 0, size_t len = std::string::npos) const;
	ColorString utf8_csubstr(size_t pos = 0, size_t len = std::string::npos, bool passw=false) const;
	using size_type=typename std::string::size_type;
	static const std::string::size_type npos;
	
	Image* get_image(Font* font, int color = 0, bool passw = false);
	ColorString& insert (size_t pos, const std::string& str);
	std::string GetRawString();
	
	// returns (byte pos, glyph pos)
	std::pair<size_type,size_type> utf8_find(uint32_t val, size_type start_pos=0);
	// returns (byte pos, glyph pos)
	std::pair<size_type,size_type> utf8_rfind(uint32_t val, size_type end_pos = npos);
	
	size_type utf8_byte_pos(size_type glyph_pos);
	size_type utf8_glyph_pos(size_type byte_pos);
	
	private:
		void process_string(const char* str);
		void calc_len();
		struct Attribute {
			int pos;
			enum Color {
				white,
				red,
				blue,
				green,
				yellow,
				cyan,
				purple,
				black
			} color;
		};
		
		int m_str_len;
		std::string m_str;
		std::vector<Attribute> m_attr;
};
}

#endif
