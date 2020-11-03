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
#include "SDL.h"
#include "SDL_image.h"
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "keymap.hpp"
#include "opengl.hpp"

using LPDIRECT3DTEXTURE8 = GLuint;

struct RECT_struct {
    std::int32_t left;
    std::int32_t top;
    std::int32_t right;
    std::int32_t bottom;
};
using LPDIRECT3DDEVICE8 = std::uint32_t;

class HCOLOR {
public:
    inline HCOLOR(std::uint32_t c) {
#if SDL_BYTEORDER == SDL_LIL_ENDIAN
        this->color = glm::tvec4<std::uint8_t>((c >> 16) & 0xFF, (c >> 8) & 0xFF, c & 0xFF, (c >> 24) & 0xFF);
#else
        this->color = glm::tvec4<std::uint8_t>((c >> 8) & 0xFF, (c >> 16) & 0xFF, (c >> 24) & 0xFF, c & 0xFF);
#endif
    };
    inline HCOLOR() = default;
    friend HCOLOR operator| (HCOLOR lhs, const HCOLOR& rhs) {
        lhs.color |= rhs.color;
        return lhs;
    };
private:
    glm::tvec4<std::uint8_t> color;
};
using D3DCOLOR = HCOLOR;

#if SDL_BYTEORDER == SDL_LIL_ENDIAN
#define D3DCOLOR_RGBA(r, g, b, a) ((static_cast<std::uint32_t>(a) << 24u) | (static_cast<std::uint32_t>(r) << 16u) | (static_cast<std::uint32_t>(g) << 8u) | static_cast<std::uint32_t>(b))
#else
#define D3DCOLOR_RGBA(r, g, b, a) ((static_cast<std::uint32_t>(b) << 24u) | (static_cast<std::uint32_t>(g) << 16u) | (static_cast<std::uint32_t>(r) << 8u) | static_cast<std::uint32_t>(a))
#endif

#define LPDIRECTINPUTDEVICE8 SDL_Joystick *

#define D3DXMatrixScaling(m, x, y, z) (*(m)) = glm::scale(glm::mat4x4(1.0f), glm::vec3((x), (y), (z)))
#define D3DXMatrixTranslation(m, x, y, z) (*(m))=glm::translate(glm::mat4x4(1.0f), glm::vec3((x), (y), (z)))

// DKS - No need for construction or destruction or virtual types, changed to struct:
struct D3DXVECTOR2 {
    float x;
    float y;
};

#if !defined( __WIN32__) || defined(__MINGW32__)
void strcat_s(char *dst, const char *src);
void strcat_s(char *dst, uint32_t size, const char *src);
void strncat_s(char *dst, const char *src, uint32_t size);
void strcpy_s(char *dst, int size, const char *src);
void strcpy_s(char *dst, const char *src);
#endif

std::vector<char> LoadFileToMemory(const std::string &name);
bool isPowerOfTwo(int x);
int nextPowerOfTwo(int x);
#if defined(USE_GL1)
void load_matrix(GLenum mode, const GLfloat *m);
#endif

extern glm::mat4x4 g_matView;
extern glm::mat4x4 g_matModelView;

#endif  // _SDLPORT_H_
