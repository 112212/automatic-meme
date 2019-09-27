#include "TiledFont.hpp"
#include <chrono>
#include <iostream>
#include <cstring>
#include <algorithm>
#include "Color.hpp"
#include "BasicImage.hpp"
#include "managers/ResourceManager.hpp"
#include "managers/Images.hpp"
#include "common.hpp"
#include "utf8/utf8.h"

namespace ng {
	
TiledFont::TiledFont(Image* img, Size tile_size, unsigned int bg_key, int fontsize, bool monosized, char* charset) {
	m_img = img;
	m_tilesize = tile_size;
	m_monosize = monosized;
	m_fontsize = fontsize;
	m_bg_key = bg_key;
	if(charset) {
		int len = strlen(charset);
		// m_char_index.resize(256);
		// std::fill(m_char_index.begin(), m_char_index.end(), -1);
		for(int i=0; i < len; i++) {
			m_char_index[charset[i]] = i;
		}
	} else {
		// m_char_index.resize(256);
		for(int i=0; i < 256; i++) {
			m_char_index[i] = i;
		}
	}
	process_image();
	
	m_fontsize /= 13.0f;
}

Resource* TiledFont::GetFont(File* f, Kvp kvp) {
	bool monosized = toBool(GetKv(kvp, "monosize"));
	char* charset = 0;
	auto cs = GetKv(kvp, "charset");
	if(!cs.empty()) {
		charset = (char*)cs.c_str();
	}
	int fontsize = 13;
	auto fs = GetKv(kvp, "fontsize");
	if(!fs.empty()) {
		fontsize = std::stoi(fs);
	}
	auto bgc = GetKv(kvp, "bgcolor");
	unsigned int bg_key = 0;
	if(!bgc.empty()) {
		bg_key = Color(bgc).GetUint32();
	}
	
	Size tilesize = {64,64};
	auto ts = GetKv(kvp, "tilesize");
	std::vector<std::string> vs;
	split_string(ts, vs, ',');
	if(vs.size() == 2) {
		tilesize.w = std::stoi(vs[0]);
		tilesize.h = std::stoi(vs[1]);
	}
	std::cout << "[TiledFont] loading tiledfont: " << f->path << "\n";
	Image* img = Images::GetImage(f->path);
	std::cout << "[TiledFont] result: " << img << "\n";
	if(!img) {
		return 0;
	}
	
	return new TiledFont(img, tilesize, bg_key, fontsize, monosized, charset);
}

Rect TiledFont::getCharRect(uint32_t c) {
	return m_char_rect[ m_char_index[c] ];
}

void TiledFont::setCharRect(uint32_t c, Rect r) {
	m_char_rect[ m_char_index[c] ] = r;
}

// determine size of each character
void TiledFont::process_image() {
	Size s = m_img->GetImageSize();
	int tw = m_tilesize.w;
	int th = m_tilesize.h;
	row_tiles = s.w / tw;
	int rows = s.h / th;
	n_tiles = row_tiles * rows;
	m_char_rect.resize(n_tiles);
	
	int space_tile = m_char_index[' '];
	
	int tile=0;
	for(int row = 0; row < rows; row++) {
		for(int col = 0; col < row_tiles; col++) {
			Rect o(col*m_tilesize.w, row*m_tilesize.h, m_tilesize.w, m_tilesize.h);
			
			if(!m_monosize && tile != space_tile) {
				
				Rect r;
				
				const unsigned int* pixels = m_img->GetImage();
				int ylen = (row+1)*th;
				int xlen = (col+1)*tw;
				for(int y = row*th; y < ylen; ++y) {
					for(int x = col*tw; x < xlen; ++x) {
						const unsigned int &p = pixels[y*s.w+x];
						if((p&0xffffff) != m_bg_key) {
							r.y = y;
							goto j1;
							break;
						}
					}
				}
				j1:
				
				ylen = row*th;
				xlen = (col+1)*tw;
				for(int y = (row+1)*th-1; y > ylen; --y) {
					for(int x = col*tw; x < xlen; ++x) {
						const unsigned int &p = pixels[y*s.w+x];
						if((p&0xffffff) != m_bg_key) {
							r.h = y-r.y;
							goto j2;
							break;
						}
					}
				}
				j2:
				
				xlen = (col+1)*tw;
				ylen = (row+1)*th;
				for(int x = col*tw; x < xlen; ++x) {
					for(int y = row*th; y < ylen; ++y) {
						const unsigned int &p = pixels[y*s.w+x];
						if((p&0xffffff) != m_bg_key) {
							r.x = x;
							goto j3;
							break;
						}
					}
				}
				j3:
				
				
				xlen = (col)*tw;
				ylen = (row+1)*th;
				for(int x = (col+1)*tw-1; x > xlen; --x) {
					for(int y = (row)*th; y < ylen; ++y) {
						const unsigned int &p = pixels[y*s.w+x];
						if((p&0xffffff) != m_bg_key) {
							r.w = x-r.x;
							goto j4;
							break;
						}
					}
				}
				j4:
				r.w = std::max(4, r.w);
				r.x--;
				if(r.x < 0) r.x = 0;
				r.w+=2;
				r.y--;
				if(r.y < 0) r.y = 0;
				r.h+=2;
				o = r;

			}
			
			m_char_rect[tile] = o;
			tile++;
		}
	}
	
	Rect r = getCharRect('a');
	
	Rect space_rect = getCharRect(' ');
	tile = m_char_index[' '];
	
	int tile_y = tile / row_tiles;
	int tile_x = tile % row_tiles;
	space_rect.w = r.w;
	space_rect.h = r.h;
	
	setCharRect(' ', space_rect);
}

GlyphMetrics TiledFont::GetGlyphMetrics( uint32_t c ) {
	GlyphMetrics m;
	// int idx = m_char_index[c];
	auto idx = m_char_index.find(c);
	if(idx == m_char_index.end()) {
		idx = m_char_index.find('?');
		if(idx == m_char_index.end()) {
			return m;
		}
	}
	Rect r = m_char_rect[idx->second];
	
	int h;
	auto idx2 = m_char_index.find('l');
	if(idx2 == m_char_index.end()) {
		idx2 = m_char_index.find('a');
		if(idx2 == m_char_index.end()) {
			return m;
		}
	}
	Rect a = m_char_rect[idx2->second];
	// r.w = a.w;
	// r.h = a.h;
	m.height = a.h * m_fontsize + 5;
	m.advance = r.w * m_fontsize;
	// m.height = m_fontsize*2;
	// m.advance = m_fontsize;
	return m;
}

Image* TiledFont::GetTextImage( std::string text, uint32_t color ) {
	int w = 0;
	int h = 0;
	
	auto it_end = text.end();
	for(auto it = text.begin(); it != it_end; ) {
		uint32_t glyph = utf8::next(it, it_end);
		auto idx = m_char_index.find(glyph);
		if(idx == m_char_index.end()) {
			idx = m_char_index.find('?');
			if(idx == m_char_index.end()) {
				continue;
			}
		}
		
		Rect &r = m_char_rect[idx->second];
		w += m_fontsize * r.w;
		h = std::max<int>(h, m_fontsize*r.h);
	}
	
	h+=1;
	
	// std::cout << "img: " << w << ", " << h << "\n";
	Image* img = new BasicImage(w, h);
	Rect dst(0,0,0,0);
	for(auto it = text.begin(); it != it_end; ) {
		uint32_t glyph = utf8::next(it, it_end);
		auto idx = m_char_index.find(glyph);
		
		if(idx == m_char_index.end()) {
			idx = m_char_index.find('?');
			if(idx == m_char_index.end()) {
				continue;
			}
		}
		Rect &r = m_char_rect[idx->second];
		dst.w = m_fontsize * r.w;
		dst.h = m_fontsize * r.h;
		
		dst.y = std::max(0, h - dst.h - 5);
		// std::cout << "render: " << (char)idx->second << " : " << r << " : " << dst << "\n";
		img->PutImage(m_img, dst, r, m_bg_key, 0x00ffffff, color);
		dst.x += dst.w;
	}
	return img;
}

TiledFont::~TiledFont() {
	
}

}
