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

#include <cstdint>
#include <cstring>
#include <filesystem>
namespace fs = std::filesystem;

#include "DX8Graphics.hpp"
#include "DX8Texture.hpp"
#include "Main.hpp"
#include "Texts.hpp"  //For ReplaceAll() function
#include "texture.hpp"

#if defined(USE_ETC1)
constexpr int ETC1_HEADER_SIZE = 16;
#endif

#if defined(USE_PVRTC)
#  define GL_COMPRESSED_RGB_PVRTC_4BPPV1_IMG 0x8C00
#  define GL_COMPRESSED_RGB_PVRTC_2BPPV1_IMG 0x8C01
#  define GL_COMPRESSED_RGBA_PVRTC_4BPPV1_IMG 0x8C02
#  define GL_COMPRESSED_RGBA_PVRTC_2BPPV1_IMG 0x8C03

constexpr int PVRTC_HEADER_SIZE = 52;

enum { PVRTC_RGB_2BPP = 0, PVRTC_RGBA_2BPP, PVRTC_RGB_4BPP, PVRTC_RGBA_4BPP };
#endif

// DKS - Textures are now managed in DX8Sprite.cpp in new TexturesystemClass.
//      This function now returns a TextureHandle object, and if it fails to load the file,
//      all of the returned TextureHandle's data members will be set to 0.
//      In th.npot_scalex and th.npot_scaley, it sets a correction
//      factor to apply to each dimension to compensate for any increases in size
//      from power-of-two expansion (each will remain 1.0 if none occurred).
bool SDL_LoadTexture(const std::string &path,
                     const std::string &filename,
                     void *buf,
                     unsigned int buf_size,
                     TextureHandle &th) {
    image_t image;
    bool success = false;
    bool load_from_memory = buf_size > 0;
    std::string fullpath;
    std::string filename_sans_ext(filename);
    ReplaceAll(filename_sans_ext, ".png", "");

    if (load_from_memory && !buf) {
        Protokoll << "Error in SDL_LoadTexture() loading texture from memory!" << std::endl;
        GameRunning = false;
        return false;
    } else if (!load_from_memory && filename.empty()) {
        Protokoll << "Error in SDL_LoadTexture(): empty filename parameter" << std::endl;
        GameRunning = false;
        return false;
    }

#if defined(USE_ETC1)
    if (DirectGraphics.IsETC1Supported()) {
        image_t alpha_image;

        // First, load the RGB texture:
        fullpath = path + "/etc1/" + filename_sans_ext + ".pkm";
        success = loadImageETC1(image, fullpath);

        if (success) {
            // Then, load the alpha texture:
            fullpath = path + "/etc1/" + filename_sans_ext + "_alpha.pkm";
            success = loadImageETC1(alpha_image, fullpath);
        }

        if (success) {
            success = load_texture(image, th.tex) && load_texture(alpha_image, th.alphatex);
        }

        image.data = std::vector<char>();
        alpha_image.data = std::vector<char>();
        ;

        if (success)
            goto loaded;
        else
            th.tex = th.alphatex = 0;
    }
#endif

#if defined(USE_PVRTC)
    if (DirectGraphics.IsPVRTCSupported()) {
        fullpath = path + "/pvr/" + filename + ".pvr";

#ifndef NDEBUG
        Protokoll << "Using PVR looking for " << fullpath << std::endl;
#endif

        success = loadImagePVRTC(image, fullpath) && load_texture(image, th.tex);

        image.data = std::vector<char>();
        if (success)
            goto loaded;
        else
            th.tex = 0;
    }
#endif

    fullpath = path + "/" + filename;
    success = loadImageSDL(image, fullpath, buf, buf_size) && load_texture(image, th.tex);
    image.data = std::vector<char>();

    if (success)
        goto loaded;
    else
        th.tex = 0;

loaded:
    if (success) {
        th.instances = 1;
        th.npot_scalex = image.npot_scalex;
        th.npot_scaley = image.npot_scaley;
    } else {
        Protokoll << "Error loading texture " << filename << " in SDL_LoadTexture()" << std::endl;
        GameRunning = false;
    }

    return success;
}

void SDL_UnloadTexture(TextureHandle &th) {
    glDeleteTextures(1, &th.tex);
    th.tex = 0;
#if defined(USE_ETC1)
    glDeleteTextures(1, &th.alphatex);
    th.alphatex = 0;
#endif
    th.instances = 0;
}

bool load_texture(image_t &image, GLuint &new_texture) {
    GLuint texture;

    if (!image.data.empty()) {
        // Have OpenGL generate a texture object handle for us
        glGenTextures(1, &texture);

        // Bind the texture object
        glBindTexture(GL_TEXTURE_2D, texture);

        // Set the texture's stretching properties
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

        // DKS - this fixes the lightning-beam drawing in low,med-detail modes..
        //      and from looking at its code in Player.cpp, I am convinced this is
        //      correct edge setting for textures for this game. My play-testing
        //      shows no unwanted side effects from the change to GL_REPEAT.
        // glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
        // glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

        if (image.compressed) {
            glCompressedTexImage2D(GL_TEXTURE_2D, 0, image.format, image.w, image.h, 0, image.data.size(),
                                   image.data.data() + image.offset);
        } else {
            glTexImage2D(GL_TEXTURE_2D, 0, image.format, image.w, image.h, 0, image.format, image.type,
                         image.data.data());
        }

#ifndef NDEBUG
        int error = glGetError();
        if (error != 0) {
            Protokoll << "GL load_texture Error " << error << std::endl;
            Protokoll << "Format " << std::hex << image.format << std::dec << " W " << image.w << " H " << image.h
                      << " S " << image.data.size() << " Data " << std::hex << reinterpret_cast<std::uintptr_t>(image.data.data())
                      << std::dec << " + " << image.offset << std::endl;
            return false;
        }
#endif

    } else {
        Protokoll << "ERROR Image data reference is NULL" << std::endl;
        return false;
    }

    new_texture = texture;
    return true;
}

#if defined(USE_ETC1)
bool loadImageETC1(image_t &image, const std::string &fullpath) {
    /*
        00-03 4 bytes header "PKM "
        04-05 2 bytes version "10"
        06-07 2 bytes data type (always zero)
        08-09 2 bytes extended width
        10-11 2 bytes extended height
        12-13 2 bytes original width
        14-15 2 bytes original height
        rest is data

        compressed size = (extended width / 4) * (extended height / 4) * 8
    */

    uint32_t etc1_filesize;

    if (fullpath.empty() || !fs::exists(fullpath) || !fs::is_regular_file(fullpath))
        return false;

    image.data = LoadFileToMemory(fullpath);

    if (!image.data.empty()) {
        if ((image.data[0] == 'P') && (image.data[1] == 'K') && (image.data[2] == 'M') && (image.data[3] == ' ')) {
            image.format = GL_ETC1_RGB8_OES;
            image.h = (image.data[14] << 8) + image.data[15];
            image.w = (image.data[12] << 8) + image.data[13];
            // image.size      = (((image.data[8]<<8)+image.data[9]) / 4) * (((image.data[10]<<8)+image.data[11]) / 4) *
            // 8;
            image.offset = ETC1_HEADER_SIZE;
            image.type = 0; /* dont care */
            image.compressed = true;

            Protokoll << "Loaded ETC type " << static_cast<char>(image.data[0]) << static_cast<char>(image.data[1])
                      << static_cast<char>(image.data[2]) << " for " << fullpath << std::endl;

#ifndef NDEBUG
            Protokoll << "Header " << static_cast<char>(image.data[0]) << static_cast<char>(image.data[1])
                      << static_cast<char>(image.data[2]) << static_cast<char>(image.data[3]) << "\nVersion "
                      << std::hex << (image.data[4] << 8) + image.data[5] << "\nType " << std::dec
                      << (image.data[6] << 8) + image.data[7] << "\nExt Width " << (image.data[8] << 8) + image.data[9]
                      << "\nExt Height " << (image.data[10] << 8) + image.data[11] << "\nWidth "
                      << (image.data[12] << 8) + image.data[13] << "\nHeight " << (image.data[14] << 8) + image.data[15]
                      << std::endl;
#endif
            return true;
        } else {
            Protokoll << "ERROR Unknown file type " << image.data[0] << image.data[1] << image.data[2] << image.data[3]
                      << std::endl;
            image.data = std::vector<char>();
        }
    }

    return false;
}
#endif

#if defined(USE_PVRTC)
bool loadImagePVRTC(image_t &image, const std::string &fullpath) {
    uint32_t *pvrtc_buffer32 = nullptr;
    uint32_t pvrtc_filesize, pvrtc_depth, pvrtc_bitperpixel;

    if (fullpath.empty() || !fs::exists(fullpath) || !fs::is_regular_file(fullpath))
        return false;

    image.data = LoadFileToMemory(fullpath);

    if (!image.data.empty()) {
        pvrtc_buffer32 = (uint32_t *)image.data.data();

        switch (pvrtc_buffer32[2]) {
            case PVRTC_RGB_2BPP:
                image.format = GL_COMPRESSED_RGB_PVRTC_2BPPV1_IMG;
                pvrtc_bitperpixel = 2;
                break;
            case PVRTC_RGBA_2BPP:
                image.format = GL_COMPRESSED_RGBA_PVRTC_2BPPV1_IMG;
                pvrtc_bitperpixel = 2;
                break;
            case PVRTC_RGB_4BPP:
                image.format = GL_COMPRESSED_RGB_PVRTC_4BPPV1_IMG;
                pvrtc_bitperpixel = 4;
                break;
            case PVRTC_RGBA_4BPP:
                image.format = GL_COMPRESSED_RGBA_PVRTC_4BPPV1_IMG;
                pvrtc_bitperpixel = 4;
                break;
            default:
                Protokoll << "ERROR Unknown PVRTC format " << std::hex << pvrtc_buffer32[2] << std::endl;
                // delete [] image.data;
                // image.data = nullptr;
                image.data = std::vector<char>();
                return false;
        }

        image.h = pvrtc_buffer32[6];
        image.w = pvrtc_buffer32[7];
        pvrtc_depth = pvrtc_buffer32[8];
        // image.size      = (image.w * image.h * pvrtc_depth * pvrtc_bitperpixel) / 8;
        image.offset = PVRTC_HEADER_SIZE;
        image.type = 0; /* dont care */
        image.compressed = true;

        Protokoll << "Loaded PVRTC type " << std::dec << pvrtc_buffer32[2] << " for " << fullpath << std::endl;

#ifndef NDEBUG
        Protokoll << "Version " << std::hex << pvrtc_buffer32[0] << "\nFlags " << std::dec << pvrtc_buffer32[1]
                  << "\nPFormatA " << pvrtc_buffer32[2] << "\nPFormatB " << pvrtc_buffer32[3] << "\nColorS "
                  << pvrtc_buffer32[4] << "\nChanType " << pvrtc_buffer32[5] << "\nHeight " << pvrtc_buffer32[6]
                  << "\nWidth " << pvrtc_buffer32[7] << "\nDepth " << pvrtc_buffer32[8] << "\nNumSurf "
                  << pvrtc_buffer32[9] << "\nNumFaces " << pvrtc_buffer32[10] << "\nMipmap " << pvrtc_buffer32[11]
                  << "\nMeta " << pvrtc_buffer32[12] << std::endl;
#endif

        return true;
    }

    return false;
}
#endif

bool loadImageSDL(image_t &image, const std::string &fullpath, void *buf, unsigned int buf_size) {
    SDL_Surface *rawSurf = nullptr;  // This surface will tell us the details of the image
    SDL_Surface *finSurf = nullptr;

    // Init
    image.data = std::vector<char>();
    image.compressed = false;
    image.format = GL_RGBA;
    image.npot_scalex = 1.0;
    image.npot_scaley = 1.0;

    if (buf_size == 0)  // Load from file
    {
        if (fullpath.empty() || !fs::exists(fullpath) || !fs::is_regular_file(fullpath)) {
            Protokoll << "Error in loadImageSDL loading " << fullpath << std::endl;
            GameRunning = false;
            return false;
        }

        rawSurf = IMG_Load(fullpath.c_str());
    } else  // Load from memory
    {
        SDL_RWops *sdl_rw = SDL_RWFromConstMem(reinterpret_cast<const void *>(buf), buf_size);

        if (sdl_rw != nullptr) {
            rawSurf = IMG_Load_RW(sdl_rw, 1);
        } else {
            Protokoll << "ERROR Texture: Failed to load texture: " << SDL_GetError() << std::endl;
            GameRunning = false;
            return false;
        }
    }

    if (rawSurf != nullptr) {
        SDL_Rect rawDimensions;
        //  Store dimensions of original RAW surface
        rawDimensions.x = rawSurf->w;
        rawDimensions.y = rawSurf->h;

        // DKS - Allowing NPOT textures is now a command line switch:
        //      The safest thing seems to me to leave it off unless specifically requested,
        //      since some GPUs/drivers do not play nicely with it and old GL/GLES doesn't even pretend to.
        if (!CommandLineParams.AllowNPotTextureSizes) {
            //  Check if surface is PoT
            if (!isPowerOfTwo(rawSurf->w)) {
                rawDimensions.x = nextPowerOfTwo(rawSurf->w);
                image.npot_scalex = static_cast<double>(rawSurf->w) / static_cast<double>(rawDimensions.x);
            }

            if (!isPowerOfTwo(rawSurf->h)) {
                rawDimensions.y = nextPowerOfTwo(rawSurf->h);
                image.npot_scaley = static_cast<double>(rawSurf->h) / static_cast<double>(rawDimensions.y);
            }
        }

        finSurf = SDL_CreateRGBSurface(SDL_SWSURFACE, rawDimensions.x, rawDimensions.y, 32,
#if SDL_BYTEORDER == SDL_LIL_ENDIAN  // OpenGL RGBA masks
                                       0x000000FF, 0x0000FF00, 0x00FF0000, 0xFF000000
#else
                                       0xFF000000, 0x00FF0000, 0x0000FF00, 0x000000FF
#endif
        );

#if SDL_VERSION_ATLEAST(2, 0, 0)
        SDL_SetSurfaceAlphaMod(rawSurf, 255);
#else /* SDL 1.2 */
        SDL_SetAlpha(rawSurf, 0, 0);
#endif

        SDL_BlitSurface(rawSurf, nullptr, finSurf, nullptr);
        SDL_FreeSurface(rawSurf);

        uint8_t factor = 1;

        if (((finSurf->w >= CommandLineParams.TexSizeMin) || (finSurf->h >= CommandLineParams.TexSizeMin)) &&
            (CommandLineParams.TexFactor > 1)) {
            factor = CommandLineParams.TexFactor;
        }

        // Blacklist of image filenames (sub-strings) that shouldn't ever be resized, because of
        // resulting graphics glitches
        if (fullpath.find("font") != std::string::npos
            //|| fullpath.find("lightmap")     != std::string::npos           // Lightmaps were never actually used in
            //the game
            || fullpath.find("hurrican_rund") != std::string::npos  // Menu star/nebula background (ugly)
            || fullpath.find("roboraupe") != std::string::npos      // Flat spiky enemy worm-like thing (glitches)
            || fullpath.find("enemy-walker") != std::string::npos   // Frog-like robotic walker (glitches)
            || fullpath.find("stelzsack") != std::string::npos      // Stilt-walker enemy on elevator level
        ) {
            factor = 1;
        }

        if (factor > 1)
            image.data = LowerResolution(finSurf, factor);

        image.w = finSurf->w / factor;
        image.h = finSurf->h / factor;

        if (image.data.empty()) {
            image.data.resize(finSurf->w * finSurf->h * sizeof(uint32_t));
            std::memcpy(image.data.data(), finSurf->pixels, image.data.size());
            image.type = GL_UNSIGNED_BYTE;
        }

        SDL_FreeSurface(finSurf);
    } else {
        Protokoll << "Error in loadImageSDL: Could not read image data into rawSurf" << std::endl;
        GameRunning = false;
        return false;
    }

    return true;
}

// DKS - Assumes rgba8888 input, rgba8888 output
std::vector<char> LowerResolution(SDL_Surface *surface, int factor) {
    if (factor != 2 && factor != 4) {
        Protokoll << "ERROR call to LowerResolution() with factor not equal to 2 or 4" << std::endl;
        return std::vector<char>();
    }

    if (surface->format->BytesPerPixel != 4) {
        Protokoll << "ERROR call to LowerResolution() with source surface bpp other than 4" << std::endl;
        return std::vector<char>();
    }

    std::vector<char> dataout;
    dataout.reserve((surface->h / factor) * (surface->w / factor) * sizeof(uint32_t));

    uint32_t *dataout32 = reinterpret_cast<uint32_t *>(dataout.data());

    for (int y = 0; y < surface->h; y += factor) {
        uint32_t *datain32 = (reinterpret_cast<uint32_t *>(surface->pixels)) + surface->w * y;
        for (int x = 0; x < surface->w; x += factor) {
            *dataout32 = *datain32;
            datain32 += factor;
            dataout32++;
        }
    }

    return dataout;
}
