#include "Image_libpng.hpp"
#include "../BasicImage.hpp"
#include <iostream>
namespace ng {

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

namespace Image_libpng {

void userReadData(png_structp pngPtr, png_bytep data, png_size_t length) {
    //Here we get our IO pointer back from the read struct.
    //This is the parameter we passed to the png_set_read_fn() function.
    //Our std::istream pointer.
    png_voidp a = png_get_io_ptr(pngPtr);
    //Cast the pointer to std::istream* and read 'length' bytes into 'data'
    ((File*)a)->Read(data, length);
}

Resource* LoadPNG(File* file) {
	std::vector<Image*> vect;
	Image_libpng::load_png(file, vect);
	return vect.front();
}

void load_png(File* f, std::vector<Image*>& out_vector)
{
    unsigned int    width, height, channels, rowbytes, size, i, j;
    png_bytepp      rows_image;
    png_bytepp      rows_frame;
    unsigned char * p_image;
    unsigned char * p_frame;
    unsigned char * p_temp;
    unsigned char   sig[8];
    if (f->Read(sig, 8) == 8 && png_sig_cmp(sig, 0, 8) == 0)
    {
      png_structp png_ptr  = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
      png_set_read_fn(png_ptr,(png_voidp)&f, userReadData);
      png_infop   info_ptr = png_create_info_struct(png_ptr);
      if (png_ptr && info_ptr)
      {
        if (setjmp(png_jmpbuf(png_ptr)))
        {
          png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
          f->Close();
          return;
        }
        png_init_io(png_ptr, (FILE*)f);
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

			
			BasicImage* img = new BasicImage(width, height);
			img->BlitRows((const uint32_t**)rows_image, Size(width,height));
			out_vector.push_back(img);
			
            // SDL_Surface* s = SDL_CreateRGBSurface(0, width, height, 32, 0x00ff0000, 0x0000ff00, 0x000000ff, 0xff000000);
            // uint32_t *p = (uint32_t*)s->pixels;
            /*
            for(int y=0; y < height; y++) {
				uint32_t *r = (uint32_t*)rows_image[y];
				for(int x=0; x < width; x++) {
					p[y*s->w+x] = r[x];
				}
			}
            out_vector.push_back(s);
            */

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
    f->Close();
}

} // Image_libpng

} // ng
