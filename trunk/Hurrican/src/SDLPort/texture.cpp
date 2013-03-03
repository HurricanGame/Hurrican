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

#include "texture.h"
#include "Main.h"

std::vector<GLuint> textures;

#if defined(USE_PVRTC)
#define GL_COMPRESSED_RGB_PVRTC_4BPPV1_IMG			0x8C00
#define GL_COMPRESSED_RGB_PVRTC_2BPPV1_IMG			0x8C01
#define GL_COMPRESSED_RGBA_PVRTC_4BPPV1_IMG			0x8C02
#define GL_COMPRESSED_RGBA_PVRTC_2BPPV1_IMG			0x8C03

#define PVRTC_HEADER_SIZE 52

enum
{
    PVRTC_RGB_2BPP=0,
    PVRTC_RGBA_2BPP,
    PVRTC_RGB_4BPP,
    PVRTC_RGBA_4BPP
};
#endif

bool isPowerOfTwo(int x)
{
    return ((x != 0) && !(x & (x - 1)));
}

int nextPowerOfTwo(int x)
{
    double logbase2 = log(x) / log(2);
    return (int)round(pow(2,ceil(logbase2)));
}

GLuint loadTexture( const char* path, SDL_Rect& dims, uint32_t size )
{
    image_t image;
    GLuint texture;			// This is a handle to our texture object

    // Get the image data into memory
    if (loadCompressedImage( image, path ) == false)
    {
        if (loadImage( image, path, size ) == false)
        {
            printf( "ERROR Image was not loaded to memory\n" );
            return GL_INVALID_VALUE;
        }
    }

    if (image.data != NULL)
    {
        dims.w = image.w;
        dims.h = image.h;

        // Have OpenGL generate a texture object handle for us
        glGenTextures( 1, &texture );

        // Bind the texture object
        glBindTexture( GL_TEXTURE_2D, texture );

        // Set the texture's stretching properties
        glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
        glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
        glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
        glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );

        if (image.compressed == true)
        {
            glCompressedTexImage2D( GL_TEXTURE_2D, 0, image.format,
                                    image.tex_w, image.tex_h, 0, image.size, image.data+image.offset );
        }
        else
        {
            glTexImage2D( GL_TEXTURE_2D, 0, image.format, image.tex_w, image.tex_h, 0,
                          image.format, image.type, image.data );
        }

        textures.push_back( texture ); /* save a ref for deletion leter */

#if defined(DEBUG)
        int error = glGetError();
        if (error != 0)
            printf( "GL Tex Error %X %s\n", error, path );
#endif

        // Cleanup
        delete [] image.data;
    }
    else
    {
        printf( "ERROR Image data reference is NULL\n" );
        return GL_INVALID_VALUE;
    }

    return texture;
}

bool loadCompressedImage( image_t& image, const char* path )
{
#if defined(USE_PVRTC)
    uint32_t* pvrtc_buffer32 = NULL;
    uint32_t pvrtc_filesize, pvrtc_depth, pvrtc_bitperpixel;
    std::string filename = path;

    filename = path + std::string(".pvr");

    image.data = LoadFileToMemory( filename, pvrtc_filesize );

    if (image.data != NULL)
    {
        pvrtc_buffer32  = (uint32_t*)image.data;

        switch ( pvrtc_buffer32[2] )
        {
            case PVRTC_RGB_2BPP:
                image.format = GL_COMPRESSED_RGB_PVRTC_2BPPV1_IMG;
            case PVRTC_RGBA_2BPP:
                image.format = GL_COMPRESSED_RGBA_PVRTC_2BPPV1_IMG;
                pvrtc_bitperpixel = 2;
                break;
            case PVRTC_RGB_4BPP:
                image.format = GL_COMPRESSED_RGB_PVRTC_4BPPV1_IMG;
            case PVRTC_RGBA_4BPP:
                image.format = GL_COMPRESSED_RGBA_PVRTC_4BPPV1_IMG;
                pvrtc_bitperpixel = 4;
                break;
            default:
                printf( "ERROR Unknown PVRTC format %X\n",  pvrtc_buffer32[2] );
                delete [] image.data;
        }

        image.h = image.tex_h = pvrtc_buffer32[6];
        image.w = image.tex_w = pvrtc_buffer32[7];
        pvrtc_depth     = pvrtc_buffer32[8];
        image.size      = (image.w * image.h * pvrtc_depth * pvrtc_bitperpixel) / 8;
        image.offset    = PVRTC_HEADER_SIZE;
        image.type      = 0; /* dont care */
        image.compressed = true;

        printf( "Loaded PVRTC type %d for %s\n", pvrtc_buffer32[2], filename.c_str() );

#if defined(DEBUG)
        printf( "Version %X\nFlags %d\nPFormatA %d\nPFormatB %d\nColorS %d\nChanType %d\nHeight %d\nWidth %d\nDepth %d\nNumSurf %d\nNumFaces %d\nMipmap %d\nMeta %d\n", pvrtc_buffer32[0], pvrtc_buffer32[1], pvrtc_buffer32[2], pvrtc_buffer32[3],
                pvrtc_buffer32[4], pvrtc_buffer32[5], pvrtc_buffer32[6], pvrtc_buffer32[7], pvrtc_buffer32[8],
                pvrtc_buffer32[9], pvrtc_buffer32[10], pvrtc_buffer32[11], pvrtc_buffer32[12] );
#endif

        return true;
    }
#endif

    return false;
}

bool loadImage( image_t& image, const char* path, uint32_t size )
{
    uint8_t factor;
    SDL_Rect rawDimensions;
    SDL_Surface* rawSurf = NULL;	// This surface will tell us the details of the image
    SDL_Surface* finSurf = NULL;

    // Init
    image.data          = NULL;
    image.size          = 0; /* dont care */
    image.compressed    = false;
    image.format        = GL_RGBA;

    if (size <= 0)  // Load from file
    {
        rawSurf = IMG_Load(path);
    }
    else            // Load from memory
    {
        SDL_RWops* sdl_rw = SDL_RWFromConstMem( (const void*)path, size );

        if (sdl_rw != NULL)
        {
            rawSurf = IMG_Load_RW( sdl_rw, 1 );
        }
        else
        {
            printf( "ERROR Texture: Failed to load texture: %s\n", SDL_GetError() );
            return false;
        }
    }

    if (rawSurf != NULL)
    {
        //  Store dimensions of original RAW surface
        rawDimensions.x = rawSurf->w;
        rawDimensions.y = rawSurf->h;

#if defined(USE_GLES1) || defined(RGBA_5551) /* Since images are nearest and not mipmapped GLES2 should support NPOT */
        //  Check if surface is PoT
        if (!isPowerOfTwo(rawSurf->w))
            rawDimensions.x = nextPowerOfTwo(rawSurf->w);
        if (!isPowerOfTwo(rawSurf->h))
            rawDimensions.y = nextPowerOfTwo(rawSurf->h);
#endif

        finSurf = SDL_CreateRGBSurface( SDL_SWSURFACE, rawDimensions.x, rawDimensions.y, 32,
#if SDL_BYTEORDER == SDL_LIL_ENDIAN // OpenGL RGBA masks
                                        0x000000FF, 0x0000FF00, 0x00FF0000, 0xFF000000
#else
                                        0xFF000000, 0x00FF0000, 0x0000FF00, 0x000000FF
#endif
                                      );

        //  check if original image uses an alpha channel
        if (!(rawSurf->flags & SDL_SRCALPHA))
        {
            // if no alpha use MAGENTA and key it out.
            SDL_SetColorKey( rawSurf, SDL_SRCCOLORKEY | SDL_RLEACCEL, SDL_MapRGB( rawSurf->format, 255, 0, 255 ) );
        }
        else
        {
            SDL_SetAlpha( rawSurf, 0, 0 );
        }

        SDL_BlitSurface( rawSurf, 0, finSurf, 0 );
        SDL_FreeSurface( rawSurf );

        if (((finSurf->w >= CommandLineParams.TexSizeMin) || (finSurf->h >= CommandLineParams.TexSizeMin))
             && (CommandLineParams.TexFactor > 1))
        {
            factor = CommandLineParams.TexFactor;
        }
        else
        {
            factor = 1;
        }

        if (strstr(path, "font") == NULL)
        {
#if defined(USE_GLES1) || defined(USE_GLES2)
            //LowerResolution( finSurf, 0 );
#endif
#if defined(RGBA_5551)
            if (strstr(path, "lightmap") == NULL)
            {
                image.data = ConvertRGBA5551( finSurf, factor );
                image.type = GL_UNSIGNED_SHORT_5_5_5_1;
            }
#endif
        }

        image.w = finSurf->w;
        image.h = finSurf->h;

        image.tex_w = finSurf->w / factor;
        image.tex_h = finSurf->h / factor;

        if (image.data == NULL)
        {
            image.size = finSurf->w*finSurf->h*sizeof(uint32_t);
            image.data = new uint8_t[image.size];
            memcpy( image.data, finSurf->pixels, image.size );
            image.type = GL_UNSIGNED_BYTE;
        }

        SDL_FreeSurface( finSurf );
    }

    return true;
}

#if defined(USE_GLES1) || defined(USE_GLES2)
void LowerResolution( SDL_Surface* surface, int32_t dimension )
{
    int32_t x, y;
    int32_t xh, yh = 0;
    SDL_Surface* temp;

    if ((surface->h >= dimension || surface->w >= dimension)  )
    {
        temp = surface;

        surface = SDL_CreateRGBSurface(SDL_SWSURFACE, temp->w/2, temp->h/2, 32,
#if SDL_BYTEORDER == SDL_LIL_ENDIAN // OpenGL RGBA masks
                                       0x000000FF, 0x0000FF00, 0x00FF0000, 0xFF000000
#else
                                       0xFF000000, 0x00FF0000, 0x0000FF00, 0x000000FF
#endif
                                      );

        xh = 0;
        yh = 0;
        for (y=0; y<temp->h; y+=2)
        {
            for (x=0; x<temp->w; x+=2)
            {
                if (xh < temp->w && yh < temp->h)
                    putpixel( surface, xh, yh, getpixel( temp, x, y) );
                else
                    printf( "error tex shrink overflow\n" );

                xh++;
            }
            yh++;
            xh = 0;
        }

        SDL_FreeSurface( temp );
    }
}
#endif

#if defined(RGBA_5551)
uint8_t* ConvertRGBA5551( SDL_Surface* surface, uint8_t factor )
{
    uint8_t r, g, b, a;
    int32_t x, y;
    uint8_t* data = NULL;
    uint16_t* data16 = NULL;

    // Create the space for the 16 bpp image
    data = new uint8_t[(surface->w * surface->h * sizeof(uint16_t)) / factor];
    data16 = (uint16_t*)data;

    for (y=0; y<surface->h; y+=factor)
    {
        for (x=0; x<surface->w; x+=factor)
        {
            get_components( surface, x, y, r, g, b, a );

            *data16 = ((uint16_t)(r>>3)<<11) | ((uint16_t)(g>>3)<<6) | ((uint16_t)(b>>3)<<1) | ((a > 0) ? 0x1 : 0x0);
            data16++;
        }
    }

    return data;
}
#endif

void delete_texture( GLuint texture )
{
    uint16_t i;

    for (i=0; i<textures.size(); i++)
    {
        if (texture == textures.at(i))
        {
            glDeleteTextures( 1, &textures.at(i) );
            textures.at(i) = 0;
        }
    }
}

void delete_textures( void )
{
    uint16_t i;

    for (i=0; i<textures.size(); i++)
    {
        if (textures.at(i) != 0)
        {
            glDeleteTextures( 1, &textures.at(i) );
            textures.at(i) = 0;
        }
    }

    textures.clear();
}
