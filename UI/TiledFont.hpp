#ifndef TILED_FONT_HPP
#define TILED_FONT_HPP
#include "Font.hpp"
#include "Image.hpp"
#include "common.hpp"
#include <vector>
#include <chrono>
#include <map>
namespace ng {

class TiledFont : public Font {
	private:
		Image* m_img;
		std::map<uint32_t, int> m_char_index;
		std::vector<Rect> m_char_rect;
		bool m_monosize;
		Size m_tilesize;
		int row_tiles;
		float m_fontsize;
		int n_tiles;
		unsigned int m_bg_key;
		void process_image();
		Rect getCharRect(uint32_t c);
		void setCharRect(uint32_t c, Rect r);
	protected:
	public:
		TiledFont(Image* img, Size tile_size, unsigned int bg_key=0, int fontsize=13, bool monosized=false, char* charset=0);
		~TiledFont();
		
		bool Loaded();
		
		virtual GlyphMetrics GetGlyphMetrics( uint32_t c );
		virtual Image* GetTextImage( std::string text, uint32_t color );
		// static Font* GetFont( File file, int font_size );
		
};

}
#endif
