/**
 *
 *  Copyright (C) 2011-2012 Scott R. Smith
 *
 *  Permission is hereby granted, free of charge, to any person obtaining a copy
 *  of this software and associated documentation files (the "Software"), to deal
 *  in the Software without restriction, including without limitation the rights
 *  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 *  copies of the Software, and to permit persons to whom the Software is
 *  furnished to do so, subject to the following conditions:
 *
 *  The above copyright notice and this permission notice shall be included in
 *  all copies or substantial portions of the Software.
 *
 *  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 *  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 *  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 *  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 *  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 *  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 *  THE SOFTWARE.
 *
 */

#ifndef _TEXTURE_H_
#define _TEXTURE_H_

#include "SDL_port.h"

typedef struct IMAGE_T {
    uint8_t* data;
    bool compressed;
    uint32_t w, h;
    uint32_t tex_w, tex_h;
    uint32_t size;
    uint32_t offset;
    GLenum type;
    GLenum format;

    IMAGE_T() :
        data        (NULL),
        compressed  (false),
        w           (0),
        h           (0),
        tex_w       (0),
        tex_h       (0),
        size        (0),
        offset      (0),
        type        (GL_UNSIGNED_BYTE),
        format      (GL_RGBA)
    { }
} image_t;

bool isPowerOfTwo(int x);
int nextPowerOfTwo(int x);
GLuint loadTexture( const char* path, SDL_Rect& dims, uint32_t size=0 );
bool loadCompressedImage( image_t& image, const char* path );
bool loadImage( image_t& image, const char* path, uint32_t size=0 );
#if defined(USE_GLES1) || defined(USE_GLES2)
void LowerResolution( SDL_Surface* surface, int32_t dimension );
#endif
#if defined(RGBA_5551)
uint8_t* ConvertRGBA5551( SDL_Surface* surface, uint8_t factor );
#endif
void delete_texture( GLuint texture );
void delete_textures( void );

#endif /* _TEXTURE_H_ */
