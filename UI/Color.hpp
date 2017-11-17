#ifndef NG_COLORS_HPP
#define NG_COLORS_HPP

#include <string>

namespace ng {
class Color {
	public:
		Color();
		Color(uint32_t argb);
		Color(uint8_t r, uint8_t g, uint8_t b, uint8_t a = 0xff);
		Color(std::string str);
		
		uint8_t a;
		uint8_t r;
		uint8_t g;
		uint8_t b;
		
		const static uint32_t White;
		const static uint32_t Gray;
		const static uint32_t Dgray;
		const static uint32_t Cyan;
		const static uint32_t Red;
		const static uint32_t Yellow;
		const static uint32_t Green;
		const static uint32_t Blue;
		
		uint32_t GetUint32();
		static uint32_t GetColor(uint8_t r, uint8_t g, uint8_t b, uint8_t a = 0xff);
		static uint32_t ParseColor(std::string str);
};
}
#endif
