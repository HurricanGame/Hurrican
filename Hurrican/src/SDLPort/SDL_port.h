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

#include <stdint.h>
#include <iostream>
#include <string>
#include <math.h>
#include <sstream>
#include <fstream>
#include <vector>

#include "cml/cml.h"
#include "SDL.h"
#include "SDL_image.h"
#include "keymap.h"
#include "opengl.h"
#include "Logdatei.h"

#define CONST       const
#ifndef __WIN32__
#define FALSE       false
#define TRUE        true
typedef bool        BOOL;
typedef float       FLOAT;
typedef int32_t     LONG;
typedef uint32_t    DWORD, UINT;
typedef char*       LPSTR;
typedef const char* LPCTSTR;
#endif
typedef int64_t     LONGLONG;
typedef void*       PVOID,*LPVOID;
typedef int32_t     LPDIRECT3DSURFACE8;
typedef GLuint      LPDIRECT3DTEXTURE8, LPDIRECT3DTEXTURE9, LPDIRECT3DVERTEXBUFFER8, LPDIRECT3DVERTEXBUFFER9;
typedef uint32_t    LPDIRECTINPUT8;

#ifndef __WIN32__
typedef uint32_t HINSTANCE, HANDLE, HWND, HRESULT;

//DKS - Converted to int32_t from LONG:
typedef struct tagRECT
{
    int32_t left;
    int32_t top;
    int32_t right;
    int32_t bottom;
} RECT;
#endif
typedef uint32_t LPDIRECT3D8, LPDIRECT3D9, LPDIRECT3DDEVICE8, LPDIRECT3DDEVICE9, LPDIRECTSOUND8, D3DCOLOR;

#define MAX(a,b)    ((a)>(b)?(a):(b))
#define MIN(a,b)    ((a)<(b)?(a):(b))
#define LIM(a,b,c)  MAX(MIN(a,c),b)

#define D3DX_PI ((float)(M_PI))

#if SDL_BYTEORDER == SDL_LIL_ENDIAN
#define D3DCOLOR_RGBA(r,g,b,a)  (((a)<<24) + ((r)<<16) + ((g)<<8) + (b))
#else
#define D3DCOLOR_RGBA(r,g,b,a)  (((b)<<24) + ((g)<<16) + ((r)<<8) + (a))
#endif
#define timeGetTime SDL_GetTicks

#define LPDIRECTINPUTDEVICE8 SDL_Joystick*

typedef cml::matrix44f_r    D3DXMATRIX, D3DXMATRIXA16;

void D3DXMatrixIdentity( D3DXMATRIXA16* m );

#define D3DXMatrixScaling(m,x,y,z)      cml::matrix_scale((*m),(x),(y),(z))
#define D3DXMatrixTranslation(m,x,y,z)  cml::matrix_translation((*m),(x),(y),(z))
#define D3DXMatrixMultiply(m,j,k)       (*m) = (*j)*(*k)
#define D3DXMatrixRotationX(m,a)        cml::matrix_rotation_world_x((*m),(a))
#define D3DXMatrixRotationY(m,a)        cml::matrix_rotation_world_y((*m),(a))
#define D3DXMatrixRotationZ(m,a)        cml::matrix_rotation_world_z((*m),(a))

typedef enum D3DPRIMITIVETYPE
{
    D3DPT_POINTLIST       = GL_POINTS,
    D3DPT_LINELIST        = GL_LINES,
    D3DPT_LINESTRIP       = GL_LINE_STRIP,
    D3DPT_TRIANGLELIST    = GL_TRIANGLES,
    D3DPT_TRIANGLESTRIP   = GL_TRIANGLE_STRIP,
    D3DPT_TRIANGLEFAN     = GL_TRIANGLE_FAN,
    D3DPT_FORCE_DWORD     = 0x7fffffff
} D3DPRIMITIVETYPE, *LPD3DPRIMITIVETYPE;

//DKS - No need for construction or destruction or virtual types, changed to struct:
struct D3DXVECTOR2
{
    FLOAT x;
    FLOAT y;
};

#define sprintf_s   sprintf
void fopen_s        ( FILE** file, const char* path, const char* mode );
void strcat_s       ( char* dst, const char* src );
void strcat_s       ( char* dst, uint32_t size, const char* src );
void strncat_s      ( char* dst, const char* src, uint32_t size );
void strcpy_s       ( char* dst, int size, const char* src );
void strcpy_s       ( char* dst, const char* src );
void strncpy_s      ( char* dst, const char* src, int size );
void fprintf_s      ( FILE* file, const char* src );
void _itoa_s        ( int value, char* dst, int size );
char* _strrev       ( char *);

#ifndef __WIN32__
void DeleteFile( const char* filename );
#else
uint8_t LoadGLFunctions( void );
#endif
uint32_t getpixel( SDL_Surface *surface, int16_t x, int16_t y );
void putpixel( SDL_Surface *surface, int16_t x, int16_t y, uint32_t pixel );
void get_components( SDL_Surface *surface, int16_t x, int16_t y, uint8_t& r, uint8_t& g, uint8_t& b, uint8_t& a );
uint8_t* LoadFileToMemory( const std::string& name, uint32_t& size );
bool isPowerOfTwo(int x);
int nextPowerOfTwo(int x);
#if defined(USE_GL1)
void load_matrix( GLenum mode, const GLfloat* m );
#endif

extern D3DXMATRIXA16 g_matView;
extern D3DXMATRIXA16 g_matModelView;

#endif // _SDLPORT_H_
