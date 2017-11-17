
#include <SDL2/SDL_image.h>
// #include <libpng/png.h>
#include <iostream>
#include <chrono>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "png.h"

#include "SDL_Surface_Image.hpp"
#include "../Screen.hpp"

namespace ng {

SDL_Surface_Image::SDL_Surface_Image( SDL_Surface* surf ) : Image() {
	// std::cout << "creating font text\n";
	cur_frame = 0;
	this->surf = surf;
	s.w = surf->w;
	s.h = surf->h;
	if(!this->surf) {
		// std::cout << "FAIL creating font text\n";
	}
	SetFullAffectedRegion();
}

SDL_Surface_Image::SDL_Surface_Image( std::string image_filename ) {
	cur_frame = 0;
	SDL_Surface* surf2 = IMG_Load(image_filename.c_str());
	SDL_PixelFormat pfm;
	pfm.format = SDL_PIXELFORMAT_BGRA8888;
	pfm.palette = 0;
	pfm.BitsPerPixel = 32;
	pfm.BytesPerPixel = 4;
	pfm.Rmask = 0x00ff0000;
	pfm.Gmask = 0x0000ff00;
	pfm.Bmask = 0x000000ff;
	pfm.Amask = 0xff000000;
	surf = SDL_ConvertSurface(surf2, &pfm, 0);
	s.w = surf->w;
	s.h = surf->h;
	SetFullAffectedRegion();
}

SDL_Surface_Image::SDL_Surface_Image( const std::vector<SDL_Surface*>& surfs ) {
	surf = 0;
	s.w = 0;
	s.h = 0;
	frames = surfs;
	cur_frame = 0;
	if(!frames.empty()) {
		s.w = frames[0]->w;
		s.h = frames[0]->h;
		caches.resize(frames.size());
		for(int& i : caches) {
			i=NO_TEXTURE;
		}
	}
	SetFullAffectedRegion();
	m_tp = std::chrono::high_resolution_clock::now();
}

Image* SDL_Surface_Image::LoadPNG(File file) {
	// png_image png;
	// png.version = PNG_IMAGE_VERSION;
	// png.opaque = 0;
	// png_image_begin_read_from_file(&png, file.name.c_str());
	// png.format = PNG_FORMAT_BGRA;
	// png_color color;
	std::vector<SDL_Surface*> vect;
	load_png(file.name, vect);
	if(vect.empty()) {
		return 0;
	}
	// std::cout << "loaded imgs: " << vect.size() << "\n";
	return new SDL_Surface_Image(vect);
    // std::cout << png.width << ", " << png.height << "\n";
	
	png_structp str = png_create_read_struct (PNG_LIBPNG_VER_STRING, 0, 0, 0);
	//void png_read_frame_head(png_structp png_ptr, png_infop info_ptr)
	png_infop p_info = png_create_info_struct (str);
	
	FILE* fp = fopen(file.name.c_str(), "rb");
	png_init_io(str, fp);
	png_read_info(str, p_info);
	int w = png_get_image_width(str, p_info);
	int h = png_get_image_height(str, p_info);
	SDL_Surface* surf = SDL_CreateRGBSurface(0, w, h, 32, 0x00ff0000, 0x0000ff00, 0x000000ff, 0xff000000);
	
	// for(int i=0; i < 10; i++) {
		// png_read_frame_head(str, p_info);
	// }

	
	png_bytepp rows = new png_byte*[h];
	// png_byte* p_img = new png_byte[w*h];
	png_byte* p_img = (png_byte*)surf->pixels;
	for(int i=0; i < h; i++) {
		rows[i] = p_img+i*w*4;
	}
	png_set_bgr(str);
	png_read_image(str, rows);
	
	// png_image_finish_read(&png, &color, surf->pixels, 0, 0);
	
	return new SDL_Surface_Image(surf);
}

// std::chrono::high_resolution_clock::time_point tp = std::chrono::high_resolution_clock::now();
void SDL_Surface_Image::Update(int ms) {
	if(frames.empty()) return;
	int speed = 20;
	if(ms == -1) {
		std::chrono::duration<float, std::milli> d = std::chrono::high_resolution_clock::now() - m_tp;
			
		if(d > std::chrono::milliseconds(speed)) {
			// int ms = std::chrono::duration_cast<std::chrono::milliseconds>(d).count();
			ms = (int)d.count();
			m_tp = std::chrono::high_resolution_clock::now();
		}
	}
	if(ms > 0) {
		int old_frame = cur_frame;
		cur_frame = (cur_frame + (ms / speed)) % frames.size();
		// std::cout << "update " << cur_frame << "\n";
		if(old_frame != cur_frame) {
			if(caches[cur_frame] == NO_TEXTURE) {
				// std::cout << "update\n";
				SetFullAffectedRegion();
			}
		}
	}
}

const unsigned int* SDL_Surface_Image::GetImage() {
	if(!surf) {
		// std::cout << "getting frame\n";
		if(!frames.empty()) {
			return (unsigned int*)frames[cur_frame]->pixels;
		} else {
			return 0;
		}
	} else {
		return (unsigned int*)surf->pixels;
	}
}

SDL_Surface_Image::~SDL_Surface_Image() {
	if(surf) {
		Free();
		SDL_FreeSurface(surf);
	}
}

Size SDL_Surface_Image::GetImageSize() {
	return s;//Size( surf->w, surf->h );
}

Image* SDL_Surface_Image::LoadImage(File f) {
	return new SDL_Surface_Image( f.name );
}

uint32_t SDL_Surface_Image::GetTextureId() {
	if(surf) {
		return Image::GetTextureId();
	} else {
		return caches[cur_frame];
	}
}

void SDL_Surface_Image::SetCache(uint32_t cache_id) {
	if(!caches.empty()) {
		caches[cur_frame] = cache_id;
	} else {
		Image::SetCache(cache_id);
	}
}

void SDL_Surface_Image::FreeCache() {
	if(surf) {
		Image::FreeCache();
	} else {
		getScreen()->RemoveFromCache(caches[cur_frame]);
	}
}

void SDL_Surface_Image::PutImage(Image* img, Rect_t dstRegion, Rect_t srcRegion) {
	Size s = img->GetImageSize();
	const unsigned int* pixels = img->GetImage();
	SDL_Surface* tmp_surf = SDL_CreateRGBSurfaceFrom((void*)pixels,
                                      s.w,
                                      s.h,
                                      24,
                                      4*s.w,
                                      0x00ff0000,
                                      0x0000ff00,
                                      0x000000ff,
                                      0xff000000);
	
	SDL_Rect src_rect;
	src_rect.x = srcRegion.x;
	src_rect.y = srcRegion.y;
	src_rect.w = srcRegion.w;
	src_rect.h = srcRegion.h;
	
	SDL_Rect dst_rect;
	dst_rect.x = dstRegion.x;
	dst_rect.y = dstRegion.y;
	dst_rect.w = dstRegion.w;
	dst_rect.h = dstRegion.h;
	
	SDL_BlitSurface(tmp_surf,
                    &src_rect,
                    this->surf,
                    &dst_rect);

}




/*
 * load4apng.c
 *
 * loads APNG file, saves all frames as TGA (32bpp).
 * including frames composition.
 *
 * needs apng-patched libpng.
 *
 * Copyright (c) 2012-2014 Max Stepin
 * maxst at users.sourceforge.net
 *
 * zlib license
 * ------------
 *
 * This software is provided 'as-is', without any express or implied
 * warranty.  In no event will the authors be held liable for any damages
 * arising from the use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software. If you use this software
 *    in a product, an acknowledgment in the product documentation would be
 *    appreciated but is not required.
 * 2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 * 3. This notice may not be removed or altered from any source distribution.
 *
 */


/*
void save_tga(unsigned char ** rows, unsigned int w, unsigned int h, unsigned int channels, unsigned int frame)
{
  char szOut[512];
  FILE * f2;
  if (channels == 4)
  {
    unsigned short tgah[9] = {0,2,0,0,0,0,(unsigned short)w,(unsigned short)h,0x0820};
    sprintf(szOut, "test_load4_%03d.tga", frame);
    if ((f2 = fopen(szOut, "wb")) != 0)
    {
      unsigned int j;
      if (fwrite(&tgah, 1, 18, f2) != 18) return;
      for (j=0; j<h; j++)
        if (fwrite(rows[h-1-j], channels, w, f2) != w) return;
      fclose(f2);
    }
    printf("  [libpng");
#ifdef PNG_APNG_SUPPORTED
    printf("+apng");
#endif
    printf(" %s]:  ", PNG_LIBPNG_VER_STRING);
    printf("%s : %dx%d   %c\n", szOut, w, h, frame>0 ? '*' : ' ');
  }
}
*/

#ifdef PNG_APNG_SUPPORTED
static void BlendOver(unsigned char ** rows_dst, unsigned char ** rows_src, unsigned int x, unsigned int y, unsigned int w, unsigned int h)
{
  unsigned int  i, j;
  int u, v, al;

  for (j=0; j<h; j++)
  {
    unsigned char * sp = rows_src[j];
    unsigned char * dp = rows_dst[j+y] + x*4;

    for (i=0; i<w; i++, sp+=4, dp+=4)
    {
      if (sp[3] == 255)
        memcpy(dp, sp, 4);
      else
      if (sp[3] != 0)
      {
        if (dp[3] != 0)
        {
          u = sp[3]*255;
          v = (255-sp[3])*dp[3];
          al = u + v;
          dp[0] = (sp[0]*u + dp[0]*v)/al;
          dp[1] = (sp[1]*u + dp[1]*v)/al;
          dp[2] = (sp[2]*u + dp[2]*v)/al;
          dp[3] = al/255;
        }
        else
          memcpy(dp, sp, 4);
      }
    }  
  }
}
#endif

void SDL_Surface_Image::load_png(std::string filename, std::vector<SDL_Surface*>& out_vector)
// void load_png(char * szImage)
{
  FILE * f1;

  if ((f1 = fopen(filename.c_str(), "rb")) != 0)
  {
    unsigned int    width, height, channels, rowbytes, size, i, j;
    png_bytepp      rows_image;
    png_bytepp      rows_frame;
    unsigned char * p_image;
    unsigned char * p_frame;
    unsigned char * p_temp;
    unsigned char   sig[8];

    if (fread(sig, 1, 8, f1) == 8 && png_sig_cmp(sig, 0, 8) == 0)
    {
      png_structp png_ptr  = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
      png_infop   info_ptr = png_create_info_struct(png_ptr);
      if (png_ptr && info_ptr)
      {
        if (setjmp(png_jmpbuf(png_ptr)))
        {
          png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
          fclose(f1);
          return;
        }
        png_init_io(png_ptr, f1);
        png_set_sig_bytes(png_ptr, 8);
        png_read_info(png_ptr, info_ptr);
        png_set_expand(png_ptr);
        png_set_strip_16(png_ptr);
        png_set_gray_to_rgb(png_ptr);
        png_set_add_alpha(png_ptr, 0xff, PNG_FILLER_AFTER);
        png_set_bgr(png_ptr);
        (void)png_set_interlace_handling(png_ptr);
        png_read_update_info(png_ptr, info_ptr);
        width    = png_get_image_width(png_ptr, info_ptr);
        height   = png_get_image_height(png_ptr, info_ptr);
        channels = png_get_channels(png_ptr, info_ptr);
        if(channels != 4) {
			std::cout << "unsupported channels count (" << channels << ") \n";
			return;
		}
        rowbytes = png_get_rowbytes(png_ptr, info_ptr);
        size = height*rowbytes;
        p_image = (unsigned char *)malloc(size);
        p_frame = (unsigned char *)malloc(size);
        p_temp  = (unsigned char *)malloc(size);
        rows_image = (png_bytepp)malloc(height*sizeof(png_bytep));
        rows_frame = (png_bytepp)malloc(height*sizeof(png_bytep));
        if (p_image && p_frame && p_temp && rows_image && rows_frame)
        {
          png_uint_32     frames = 1;
          png_uint_32     x0 = 0;
          png_uint_32     y0 = 0;
          png_uint_32     w0 = width;
          png_uint_32     h0 = height;
#ifdef PNG_APNG_SUPPORTED
          png_uint_32     plays = 0;
          unsigned short  delay_num = 1;
          unsigned short  delay_den = 10;
          unsigned char   dop = 0;
          unsigned char   bop = 0;
          unsigned int    first = (png_get_first_frame_is_hidden(png_ptr, info_ptr) != 0) ? 1 : 0;
          if (png_get_valid(png_ptr, info_ptr, PNG_INFO_acTL)) {
            png_get_acTL(png_ptr, info_ptr, &frames, &plays);
            // std::cout << "loaded frames " << frames << "\n";
		  }
#else
			std::cout << "apng not supported\n";
#endif
          for (j=0; j<height; j++)
            rows_image[j] = p_image + j*rowbytes;

          for (j=0; j<height; j++)
            rows_frame[j] = p_frame + j*rowbytes;

          for (i=0; i<frames; i++)
          {
#ifdef PNG_APNG_SUPPORTED
            if (png_get_valid(png_ptr, info_ptr, PNG_INFO_acTL))
            {
              png_read_frame_head(png_ptr, info_ptr);
              png_get_next_frame_fcTL(png_ptr, info_ptr, &w0, &h0, &x0, &y0, &delay_num, &delay_den, &dop, &bop);
            }
            if (i == first)
            {
              bop = PNG_BLEND_OP_SOURCE;
              if (dop == PNG_DISPOSE_OP_PREVIOUS)
                dop = PNG_DISPOSE_OP_BACKGROUND;
            }
#endif
            png_read_image(png_ptr, rows_frame);

#ifdef PNG_APNG_SUPPORTED
            if (dop == PNG_DISPOSE_OP_PREVIOUS)
              memcpy(p_temp, p_image, size);

            if (bop == PNG_BLEND_OP_OVER)
              BlendOver(rows_image, rows_frame, x0, y0, w0, h0);
            else
#endif
            for (j=0; j<h0; j++)
              memcpy(rows_image[j+y0] + x0*4, rows_frame[j], w0*4);

            //save_tga(rows_image, width, height, channels, i);
            // SDL_Surface* s = 
            SDL_Surface* s = SDL_CreateRGBSurface(0, width, height, 32, 0x00ff0000, 0x0000ff00, 0x000000ff, 0xff000000);
            uint32_t *p = (uint32_t*)s->pixels;
            
            for(int y=0; y < height; y++) {
				uint32_t *r = (uint32_t*)rows_image[y];
				for(int x=0; x < width; x++) {
					p[y*s->w+x] = r[x];
				}
			}
            out_vector.push_back(s);

#ifdef PNG_APNG_SUPPORTED
            if (dop == PNG_DISPOSE_OP_PREVIOUS)
              memcpy(p_image, p_temp, size);
            else
            if (dop == PNG_DISPOSE_OP_BACKGROUND)
              for (j=0; j<h0; j++)
                memset(rows_image[j+y0] + x0*4, 0, w0*4);
#endif
          }
          png_read_end(png_ptr, info_ptr);
          free(rows_frame);
          free(rows_image);
          free(p_temp);
          free(p_frame);
          free(p_image);
        }
      }
      png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
    }
    fclose(f1);
  }
}



}

