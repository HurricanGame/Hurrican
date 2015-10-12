/**
 *
 *  Copyright (C) 2011-2015 Scott R. Smith
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
#include <string>

typedef struct IMAGE_T
{
    uint8_t* data;
    bool compressed;
    uint32_t w, h;
    uint32_t size;
    uint32_t offset;
    double   npot_scalex, npot_scaley;    //DKS - Correction factors to compensate for any 
                                          //      nearest-power-of-two size expansions.
    GLenum type;
    GLenum format;

    IMAGE_T() :
        data        (NULL),
        compressed  (false),
        w           (0),
        h           (0),
        size        (0),
        offset      (0),
        npot_scalex (1.0),
        npot_scaley (1.0),
        type        (GL_UNSIGNED_BYTE),
        format      (GL_RGBA)
    { }
} image_t;

//DKS - Textures are now managed in DX8Texture.cpp in new TexturesystemClass.
//      This function now takes a reference to a TextureHandle object,
//      returning true on success.
//      Through TextureHandle data members npot_scalex and npot_scaley, it returns a
//      factor to apply to each dimension to compensate for any increases in size
//      from power-of-two expansion (each will be 1.0f if none occurred).
bool SDL_LoadTexture( const std::string &path, const std::string &filename,
                      void *buf, unsigned int buf_size, TextureHandle &th );
void SDL_UnloadTexture( TextureHandle &th );

bool load_texture( image_t& image, GLuint &new_texture);

#if defined(USE_ETC1)
bool loadImageETC1( image_t& image, const std::string &fullpath );
#endif
#if defined(USE_PVRTC)
bool loadImagePVRTC( image_t& image, const std::string &fullpath );
#endif

bool loadImageSDL( image_t& image, const std::string &fullpath, void *buf, unsigned int buf_size );

uint8_t* LowerResolution( SDL_Surface* surface, int factor );

//DKS - disabled (RGBA5551 did not benefit how textures actually were stored in VRAM)
/*
#if defined(RGBA_5551)
uint8_t* ConvertRGBA5551( SDL_Surface* surface, uint8_t factor );
#endif
*/

#endif /* _TEXTURE_H_ */
