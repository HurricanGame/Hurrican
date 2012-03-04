#ifndef _SDLPORT_H_
#define _SDLPORT_H_

#include <stdint.h>
#include <iostream>
#include <string>
#include <math.h>
#include <sstream>
#include <vector>

#include "cml/cml.h"
#include "SDL.h"
#include "SDL_image.h"
#include "keymap.h"

#define GL_GLEXT_PROTOTYPES 1
#if defined(USE_GLES1)
#include "GLES/gl.h"
#include "GLES/glext.h"
#define glClearDepth glClearDepthf
#else
#include "SDL_opengl.h"
#endif


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
typedef GLuint      LPDIRECT3DTEXTURE8, LPDIRECT3DTEXTURE9, LPDIRECT3DSURFACE8, LPDIRECT3DVERTEXBUFFER8, LPDIRECT3DVERTEXBUFFER9;
typedef uint32_t    LPDIRECTINPUT8;

#ifndef __WIN32__
typedef uint32_t HINSTANCE, HANDLE, HWND, HRESULT;

typedef struct tagRECT {
	LONG left;
	LONG top;
	LONG right;
	LONG bottom;
} RECT;
#endif
typedef uint32_t LPDIRECT3D8, LPDIRECT3D9, LPDIRECT3DDEVICE8, LPDIRECT3DDEVICE9, LPDIRECTSOUND8, D3DCOLOR;

#define MAX(a,b) ((a)>(b)?(a):(b))
#define MIN(a,b) ((a)<(b)?(a):(b))

#define D3DX_PI M_PI
#define D3DCOLOR_XRGB(r,g,b)    (float)((r)/255), (float)((g)/255), (float)((b)/255), 1.0f
#define D3DCOLOR_RGBA(r,g,b,a)  (((a)<<24) + ((b)<<16) + ((g)<<8) + (r))
#define timeGetTime             SDL_GetTicks

#define LPDIRECTINPUTDEVICE8 SDL_Joystick*

typedef cml::matrix44f_r    D3DXMATRIX, D3DXMATRIXA16;

void D3DXMatrixIdentity( D3DXMATRIXA16* m );

#define D3DXMatrixScaling(m,x,y,z)      cml::matrix_scale((*m),(x),(y),(z))
#define D3DXMatrixTranslation(m,x,y,z)  cml::matrix_translation((*m),(x),(y),(z))
#define D3DXMatrixMultiply(m,j,k)       (*m) = (*j)*(*k)
#define D3DXMatrixRotationX(m,a)        cml::matrix_rotation_world_x((*m),(a))
#define D3DXMatrixRotationY(m,a)        cml::matrix_rotation_world_y((*m),(a))
#define D3DXMatrixRotationZ(m,a)        cml::matrix_rotation_world_z((*m),(a))

typedef enum D3DPRIMITIVETYPE {
    D3DPT_POINTLIST       = GL_POINTS,
    D3DPT_LINELIST        = GL_LINES,
    D3DPT_LINESTRIP       = GL_LINE_STRIP,
    D3DPT_TRIANGLELIST    = GL_TRIANGLES,
    D3DPT_TRIANGLESTRIP   = GL_TRIANGLE_STRIP,
    D3DPT_TRIANGLEFAN     = GL_TRIANGLE_FAN,
    D3DPT_FORCE_DWORD     = 0x7fffffff
} D3DPRIMITIVETYPE, *LPD3DPRIMITIVETYPE;

class D3DXVECTOR2 {
    public:
        FLOAT x;
        FLOAT y;

        D3DXVECTOR2() : x(0), y(0) {};
        D3DXVECTOR2(FLOAT X, FLOAT Y) : x(X), y(Y) {};
        virtual ~D3DXVECTOR2() {};
};

class D3DXVECTOR3 : public D3DXVECTOR2 {
    public:
        FLOAT z;

        D3DXVECTOR3() : D3DXVECTOR2(0,0), z(0) {};
        D3DXVECTOR3(FLOAT X, FLOAT Y, FLOAT Z) : D3DXVECTOR2(X,Y),  z(Z) {};
        virtual ~D3DXVECTOR3() {};
};

class D3DXVECTOR4 : public D3DXVECTOR3 {
    public:
        FLOAT w;

        D3DXVECTOR4() : D3DXVECTOR3(0,0,0), w(0) {};
        D3DXVECTOR4(FLOAT X, FLOAT Y, FLOAT Z, FLOAT W) : D3DXVECTOR3(X,Y,Z), w(W) {};
        virtual ~D3DXVECTOR4() {};
};

#define sprintf_s   sprintf
void fopen_s        ( FILE** file, const char* path, const char* mode );
void strcat_s       ( const char* dst, const char* src );
void strcat_s       ( const char* dst, uint32_t size, const char* src );
void strncat_s      ( const char* dst, const char* src, uint32_t size );
void strcpy_s       ( const char* dst, int size, const char* src );
void strcpy_s       ( const char* dst, const char* src );
void strncpy_s      ( const char* dst, const char* src, int size );
void fprintf_s      ( FILE* file, const char* src );
void _itoa_s        ( int value, const char* dst, int size );
char* _strrev       ( char *);

#ifndef __WIN32__
void DeleteFile( const char* filename );
#endif
SDL_Surface* loadImage( const char* path, uint32_t size=0 );
GLuint loadTexture( const char* path, SDL_Rect& dims, uint32_t size=0 );
uint32_t getpixel( SDL_Surface *surface, int16_t x, int16_t y );
void putpixel( SDL_Surface *surface, int16_t x, int16_t y, uint32_t pixel );
void int_to_rgb( uint32_t color, SDL_Color& components );
void matrixmode( GLenum mode );
void delete_texture( GLuint texture );
void delete_textures( void );

extern D3DXMATRIXA16 g_matView;

#endif // _SDLPORT_H_
