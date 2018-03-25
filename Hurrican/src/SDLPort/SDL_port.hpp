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

#ifndef _SDLPORT_H_
#define _SDLPORT_H_

#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include "Logdatei.hpp"
#include "SDL/SDL.h"
#include "SDL/SDL_image.h"
#include "cml/cml.h"
#include "keymap.hpp"
#include "opengl.hpp"

typedef std::int32_t LPDIRECT3DSURFACE8;
typedef GLuint LPDIRECT3DTEXTURE8, LPDIRECT3DTEXTURE9, LPDIRECT3DVERTEXBUFFER8, LPDIRECT3DVERTEXBUFFER9;
typedef std::uint32_t LPDIRECTINPUT8;

#ifndef __WIN32__
typedef struct tagRECT {
    std::int32_t left;
    std::int32_t top;
    std::int32_t right;
    std::int32_t bottom;
} RECT;
#endif
typedef std::uint32_t LPDIRECT3D8, LPDIRECT3D9, LPDIRECT3DDEVICE8, LPDIRECT3DDEVICE9, LPDIRECTSOUND8, D3DCOLOR;

//#define D3DX_PI ((float)(M_PI))

#if SDL_BYTEORDER == SDL_LIL_ENDIAN
#define D3DCOLOR_RGBA(r, g, b, a) (((a) << 24) + ((r) << 16) + ((g) << 8) + (b))
#else
#define D3DCOLOR_RGBA(r, g, b, a) (((b) << 24) + ((g) << 16) + ((r) << 8) + (a))
#endif

#define LPDIRECTINPUTDEVICE8 SDL_Joystick *

typedef cml::matrix44f_r D3DXMATRIX, D3DXMATRIXA16;

void D3DXMatrixIdentity(D3DXMATRIXA16 *m);

#define D3DXMatrixScaling(m, x, y, z) cml::matrix_scale((*m), (x), (y), (z))
#define D3DXMatrixTranslation(m, x, y, z) cml::matrix_translation((*m), (x), (y), (z))
#define D3DXMatrixMultiply(m, j, k) (*m) = (*j) * (*k)
#define D3DXMatrixRotationX(m, a) cml::matrix_rotation_world_x((*m), (a))
#define D3DXMatrixRotationY(m, a) cml::matrix_rotation_world_y((*m), (a))
#define D3DXMatrixRotationZ(m, a) cml::matrix_rotation_world_z((*m), (a))

// DKS - No need for construction or destruction or virtual types, changed to struct:
struct D3DXVECTOR2 {
    float x;
    float y;
};

#define sprintf_s sprintf
void strcat_s(char *dst, const char *src);
void strcat_s(char *dst, uint32_t size, const char *src);
void strncat_s(char *dst, const char *src, uint32_t size);
void strcpy_s(char *dst, int size, const char *src);
void strcpy_s(char *dst, const char *src);

#ifdef __WIN32__
std::uint8_t LoadGLFunctions(void);
#endif
std::uint32_t getpixel(SDL_Surface *surface, std::int16_t x, std::int16_t y);
void get_components(SDL_Surface *surface,
                    std::int16_t x,
                    std::int16_t y,
                    std::uint8_t &r,
                    std::uint8_t &g,
                    std::uint8_t &b,
                    std::uint8_t &a);
std::vector<char> LoadFileToMemory(const std::string &name);
bool isPowerOfTwo(int x);
int nextPowerOfTwo(int x);
#if defined(USE_GL1)
void load_matrix(GLenum mode, const GLfloat *m);
#endif

extern D3DXMATRIXA16 g_matView;
extern D3DXMATRIXA16 g_matModelView;

#endif  // _SDLPORT_H_
