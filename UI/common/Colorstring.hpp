#include <string>
#include <vector>
#include <iostream>
#include <algorithm>
#ifdef USE_SDL
#include <SDL2/SDL_ttf.h>
#endif
using std::cout; using std::endl;
class Colorstring {
	public:
	Colorstring() {}
	Colorstring& operator=(const char* s) { 
		process_string(s);
		return *this; 
	}
	
	Colorstring(const char* s) {
		process_string(s);
	}
	
	friend std::ostream& operator<<(std::ostream& o, const Colorstring& s);
	
	const char* c_str() { return m_str.c_str(); }
	size_t size() { return m_str.size(); }
	size_t length() { return m_str.size(); }
	
	Colorstring& erase (size_t pos = 0, size_t len = std::string::npos);
	
	Colorstring& operator=(const std::string str) {
		process_string(m_str.c_str());
		return *this;
	}
	
	Colorstring& operator+=(const Colorstring &b) {
		m_attr.insert(m_attr.end(), b.m_attr.begin(), b.m_attr.end());
		m_str += b.m_str;
		return *this;
	}
	Colorstring& operator+=(const std::string &b) {
		// m_attr.insert(m_attr.end(), b.m_attr.begin(), b.m_attr.end());
		m_str += b;
		return *this;
	}
	
	Colorstring operator+(const Colorstring &b) {
		Colorstring c;
		c.m_str = m_str + b.m_str;
		return c;
	}
	
	Colorstring operator+(const char* b) {
		Colorstring c;
		c.m_str = m_str + b;
		return c;
	}
	
	friend std::string operator+(const char* a, const Colorstring& b) {
		Colorstring c;
		c.m_str = b.m_str + a;
		return c;
	}
	friend std::string operator+(const std::string& a, const Colorstring& b) {
		Colorstring c;
		c.m_str = b.m_str + a;
		return c;
	}
	
	char operator[](int i) {
		return m_str[i];
	}
	
	operator std::string () {
		return m_str;
	}
	
	// Colorstring substr (size_t pos = 0, size_t len = std::string::npos) const {
		// Colorstring s;
		// s.m_str = m_str.substr(pos, len);
		// return s;
	// }
	std::string substr (size_t pos = 0, size_t len = std::string::npos) const {
		return m_str.substr(pos, len);;
	}
	
	uint32_t get_texture(TTF_Font* font);
	
	Colorstring& insert (size_t pos, const std::string& str);
	
	std::string GetRawString();
	
	private:
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
			} color;
		};
		
		std::string m_str;
		std::vector<Attribute> m_attr;
		void process_string(const char* str);
};
