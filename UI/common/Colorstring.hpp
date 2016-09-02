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
	Colorstring();
	Colorstring& operator=(const char* s) { 
		process_string(s);
		return *this; 
	}
	
	Colorstring(const char* s) {
		process_string(s);
	}
	
	Colorstring(const std::string& a);
	
	friend std::ostream& operator<<(std::ostream& o, const Colorstring& s);
	
	const char* c_str();
	const std::string& str();
	
	size_t size() { return m_str.size(); }
	size_t length() { return m_str.size(); }
	
	void erase (size_t pos = 0, size_t len = std::string::npos);
	
	Colorstring& operator=(const std::string& str) {
		process_string(str.c_str());
		return *this;
	}
	
	Colorstring& operator=(const Colorstring& b);
	Colorstring& operator+=(const Colorstring &b);
	
	Colorstring& operator+=(const std::string &b) {
		*this += Colorstring(b);
		return *this;
	}
	
	// friend std::string operator+(const char* a, const Colorstring& b);
	friend Colorstring operator+(const Colorstring& a, const Colorstring& b);
	// friend std::string operator+(const std::string& a, const Colorstring& b);
	
	char operator[](int i) {
		return m_str[i];
	}
	
	// operator const std::string& () {
		// return m_str;
	// }
	
	int GetLastColor();
	std::string substr (size_t pos = 0, size_t len = std::string::npos, bool passw=false) const;
	Colorstring csubstr (size_t pos = 0, size_t len = std::string::npos) const;
	const std::string::size_type npos = std::string::npos;
	
	SDL_Surface* get_surface(TTF_Font* font, int color = 0, bool passw = false);
	Colorstring& insert (size_t pos, const std::string& str);
	std::string GetRawString();
	
	private:
		void process_string(const char* str);
		
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
};
