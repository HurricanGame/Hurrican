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

#include "SDL_port.h"

#if defined(USE_GL1)
GLenum MatrixMode = 0;
#endif
D3DXMATRIXA16 g_matView;
D3DXMATRIXA16 g_matModelView;

#ifndef __WIN32__
void DeleteFile( const char* filename )
{
}
#endif

void D3DXMatrixIdentity( D3DXMATRIXA16* m )
{
    m->identity();
}

void fopen_s( FILE** file, const char* path, const char* mode )
{
    *file = fopen( path, mode );
}

void strcat_s( char* dst, const char* src )
{
    strcat( dst, src );
}

void strcat_s( char* dst, uint32_t size, const char* src )
{
    strncat( dst, src, size );
}

void strncat_s( char* dst, const char* src, uint32_t size )
{
    strncat( dst, src, size );
}

void strcpy_s( char* dst, int size, const char* src )
{
    strncpy( dst, src, size);
}

void strcpy_s( char* dst, const char* src )
{
    strcpy( dst, src );
}

void strncpy_s( char* dst, const char* src, int size )
{
    strncpy( dst, src, size );
}

void fprintf_s( FILE* file, const char* src)
{
    fprintf( file, "%s", src );
}

void _itoa_s( int value, char* dst, int size )
{
    sprintf( (char*)dst, "%d", value );
}

#define SWAP(T, a, b) \
    do { T save = (a); (a) = (b); (b) = save; } while (0)

char* _strrev( char *s )
{
    size_t len = strlen(s);

    if (len > 1)
    {
        char *a = s;
        char *b = s + len - 1;

        for (; a < b; ++a, --b)
            SWAP(char, *a, *b);
    }

    return s;
}

uint32_t getpixel( SDL_Surface *surface, int16_t x, int16_t y )
{
    int16_t bpp = surface->format->BytesPerPixel;
    /* Here p is the address to the pixel we want to retrieve */
    uint8_t *p = (uint8_t *)surface->pixels + y * surface->pitch + x * bpp;

    switch (bpp)
    {
    case 1:
        return *p;
        break;

    case 2:
        return *(uint16_t *)p;
        break;

    case 3:
        if(SDL_BYTEORDER == SDL_BIG_ENDIAN)
            return p[0] << 16 | p[1] << 8 | p[2];
        else
            return p[0] | p[1] << 8 | p[2] << 16;
        break;

    case 4:
        return *(uint32_t *)p;
        break;

    default:
        return 0;       /* shouldn't happen, but avoids warnings */
    }
}

void putpixel( SDL_Surface *surface, int16_t x, int16_t y, uint32_t pixel )
{
    int16_t bpp = surface->format->BytesPerPixel;
    /* Here p is the address to the pixel we want to set */
    uint8_t *p = (uint8_t *)surface->pixels + y * surface->pitch + x * bpp;

    switch (bpp)
    {
    case 1:
        *p = pixel;
        break;

    case 2:
        *(uint16_t *)p = pixel;
        break;

    case 3:
        if(SDL_BYTEORDER == SDL_BIG_ENDIAN)
        {
            p[0] = (pixel >> 16) & 0xff;
            p[1] = (pixel >> 8) & 0xff;
            p[2] = pixel & 0xff;
        }
        else
        {
            p[0] = pixel & 0xff;
            p[1] = (pixel >> 8) & 0xff;
            p[2] = (pixel >> 16) & 0xff;
        }
        break;

    case 4:
    default:
        *(uint32_t *)p = pixel;
        break;
    }
}

void get_components( SDL_Surface *surface, int16_t x, int16_t y, uint8_t& r, uint8_t& g, uint8_t& b, uint8_t& a )
{
    uint32_t color;
    uint32_t comp;

    color = getpixel( surface, x, y );

    /* Get Red component */
    comp=color&surface->format->Rmask; /* Isolate red component */
    comp=comp>>surface->format->Rshift;/* Shift it down to 8-bit */
    comp=comp<<surface->format->Rloss; /* Expand to a full 8-bit number */
    r=(uint8_t)comp;

    /* Get Green component */
    comp=color&surface->format->Gmask; /* Isolate green component */
    comp=comp>>surface->format->Gshift;/* Shift it down to 8-bit */
    comp=comp<<surface->format->Gloss; /* Expand to a full 8-bit number */
    g=(uint8_t)comp;

    /* Get Blue component */
    comp=color&surface->format->Bmask; /* Isolate blue component */
    comp=comp>>surface->format->Bshift;/* Shift it down to 8-bit */
    comp=comp<<surface->format->Bloss; /* Expand to a full 8-bit number */
    b=(uint8_t)comp;

    /* Get Alpha component */
    comp=color&surface->format->Amask; /* Isolate alpha component */
    comp=comp>>surface->format->Ashift;/* Shift it down to 8-bit */
    comp=comp<<surface->format->Aloss; /* Expand to a full 8-bit number */
    a=(uint8_t)comp;
}

uint8_t* LoadFileToMemory( const std::string& name, uint32_t& size )
{
    std::fstream file;
    uint8_t* buffer;

    file.open( name.c_str(), std::ios_base::in );
    if (file.is_open() == 0)
    {
        Protokoll.WriteText( false, "Error file operation: File: %s\n", name.c_str() );
        return NULL;
    }

    file.seekg( 0, std::ios::end );
    size = file.tellg();
    file.seekg( 0, std::ios::beg );

    buffer = new uint8_t[size+1];
    file.read( (char*)buffer, size );
    buffer[size] = '\0';
    size++;

    file.close();

    return buffer;
}

bool isPowerOfTwo(int x)
{
    return ((x != 0) && !(x & (x - 1)));
}

int nextPowerOfTwo(int x)
{
    double logbase2 = log(x) / log(2);
    return (int)round(pow(2,ceil(logbase2)));
}

#if defined(USE_GL1)
void load_matrix( GLenum mode, const GLfloat* m )
{
    if (MatrixMode != mode)
    {
        glMatrixMode( mode );
        MatrixMode = mode;
    }
    glLoadMatrixf( m );
}
#endif

#if defined(__WIN32__)
#define LOAD_OPENGL_PROC(T,N)                                                           \
{                                                                                       \
    N = (T)wglGetProcAddress(#N);                                                       \
    if (N == NULL) {                                                                    \
        Protokoll.WriteText( false, "ERROR Video: OpenGL function is NULL:" #N "\n" );  \
        return 1;                                                                       \
    }                                                                                   \
}

uint8_t LoadGLFunctions( void )
{
#if defined(USE_GL2)
    LOAD_OPENGL_PROC( PFNGLDELETESHADERPROC,                glDeleteShader );
    LOAD_OPENGL_PROC( PFNGLDELETEPROGRAMPROC,               glDeleteProgram );
    LOAD_OPENGL_PROC( PFNGLUSEPROGRAMPROC,                  glUseProgram );
    LOAD_OPENGL_PROC( PFNGLCREATESHADERPROC,                glCreateShader );
    LOAD_OPENGL_PROC( PFNGLSHADERSOURCEPROC,                glShaderSource );
    LOAD_OPENGL_PROC( PFNGLCOMPILESHADERPROC,               glCompileShader );
    LOAD_OPENGL_PROC( PFNGLGETSHADERIVPROC,                 glGetShaderiv );
    LOAD_OPENGL_PROC( PFNGLCREATEPROGRAMPROC,               glCreateProgram );
    LOAD_OPENGL_PROC( PFNGLATTACHSHADERPROC,                glAttachShader );
    LOAD_OPENGL_PROC( PFNGLLINKPROGRAMPROC,                 glLinkProgram );
    LOAD_OPENGL_PROC( PFNGLGETPROGRAMIVPROC,                glGetProgramiv );
    LOAD_OPENGL_PROC( PFNGLGETACTIVEATTRIBPROC,             glGetActiveAttrib );
    LOAD_OPENGL_PROC( PFNGLGETATTRIBLOCATIONPROC,           glGetAttribLocation );
    LOAD_OPENGL_PROC( PFNGLGETACTIVEUNIFORMPROC,            glGetActiveUniform );
    LOAD_OPENGL_PROC( PFNGLGETUNIFORMLOCATIONPROC,          glGetUniformLocation );
    LOAD_OPENGL_PROC( PFNGLGETSHADERINFOLOGPROC,            glGetShaderInfoLog );
    LOAD_OPENGL_PROC( PFNGLGETPROGRAMINFOLOGPROC,           glGetProgramInfoLog );
    LOAD_OPENGL_PROC( PFNGLDISABLEVERTEXATTRIBARRAYPROC,    glDisableVertexAttribArray );
    LOAD_OPENGL_PROC( PFNGLENABLEVERTEXATTRIBARRAYPROC,     glEnableVertexAttribArray );
    LOAD_OPENGL_PROC( PFNGLVERTEXATTRIBPOINTERPROC,         glVertexAttribPointer );
    LOAD_OPENGL_PROC( PFNGLUNIFORM1IPROC,                   glUniform1i );
    LOAD_OPENGL_PROC( PFNGLUNIFORM1FVPROC,                  glUniform1fv );
    LOAD_OPENGL_PROC( PFNGLUNIFORM3FVPROC,                  glUniform3fv );
    LOAD_OPENGL_PROC( PFNGLUNIFORM4FVPROC,                  glUniform4fv );
    LOAD_OPENGL_PROC( PFNGLUNIFORMMATRIX3FVPROC,            glUniformMatrix3fv );
    LOAD_OPENGL_PROC( PFNGLUNIFORMMATRIX4FVPROC,            glUniformMatrix4fv );
    /* FBO API */
#if defined(USE_FBO)
    LOAD_OPENGL_PROC( PFNGLBINDFRAMEBUFFEREXTPROC,          glBindFramebuffer );
    LOAD_OPENGL_PROC( PFNGLDELETEFRAMEBUFFERSEXTPROC,       glDeleteFramebuffers );
    LOAD_OPENGL_PROC( PFNGLGENFRAMEBUFFERSEXTPROC,          glGenFramebuffers );
    LOAD_OPENGL_PROC( PFNGLCHECKFRAMEBUFFERSTATUSEXTPROC,   glCheckFramebufferStatus );
    LOAD_OPENGL_PROC( PFNGLFRAMEBUFFERTEXTURE2DEXTPROC,     glFramebufferTexture2D );
    LOAD_OPENGL_PROC( PFNGLGENRENDERBUFFERSEXTPROC,         glGenRenderbuffers );
    LOAD_OPENGL_PROC( PFNGLBINDRENDERBUFFEREXTPROC,         glBindRenderbuffer );
    LOAD_OPENGL_PROC( PFNGLRENDERBUFFERSTORAGEEXTPROC,      glRenderbufferStorage );
    LOAD_OPENGL_PROC( PFNGLDELETERENDERBUFFERSEXTPROC,      glDeleteRenderbuffers );
#endif /* defined(USE_FBO) */
#endif

#if defined(USE_GL1) || defined(USE_GL2)
    LOAD_OPENGL_PROC( PFNGLCOMPRESSEDTEXIMAGE2DARBPROC,     glCompressedTexImage2D );
    LOAD_OPENGL_PROC( PFNGLGETCOMPRESSEDTEXIMAGEARBPROC,    glGetCompressedTexImageARB );
#endif

    return 0;
}

/* OpenGL Version 2.0 API */
#if defined(USE_GL2)
PFNGLDELETESHADERPROC               glDeleteShader              = NULL;
PFNGLDELETEPROGRAMPROC              glDeleteProgram             = NULL;
PFNGLUSEPROGRAMPROC                 glUseProgram                = NULL;
PFNGLCREATESHADERPROC               glCreateShader              = NULL;
PFNGLSHADERSOURCEPROC               glShaderSource              = NULL;
PFNGLCOMPILESHADERPROC              glCompileShader             = NULL;
PFNGLGETSHADERIVPROC                glGetShaderiv               = NULL;
PFNGLCREATEPROGRAMPROC              glCreateProgram             = NULL;
PFNGLATTACHSHADERPROC               glAttachShader              = NULL;
PFNGLLINKPROGRAMPROC                glLinkProgram               = NULL;
PFNGLGETPROGRAMIVPROC               glGetProgramiv              = NULL;
PFNGLGETACTIVEATTRIBPROC            glGetActiveAttrib           = NULL;
PFNGLGETATTRIBLOCATIONPROC          glGetAttribLocation         = NULL;
PFNGLGETACTIVEUNIFORMPROC           glGetActiveUniform          = NULL;
PFNGLGETUNIFORMLOCATIONPROC         glGetUniformLocation        = NULL;
PFNGLGETSHADERINFOLOGPROC           glGetShaderInfoLog          = NULL;
PFNGLGETPROGRAMINFOLOGPROC          glGetProgramInfoLog         = NULL;
PFNGLDISABLEVERTEXATTRIBARRAYPROC   glDisableVertexAttribArray  = NULL;
PFNGLENABLEVERTEXATTRIBARRAYPROC    glEnableVertexAttribArray   = NULL;
PFNGLVERTEXATTRIBPOINTERPROC        glVertexAttribPointer       = NULL;
PFNGLUNIFORM1IPROC                  glUniform1i                 = NULL;
PFNGLUNIFORM1FVPROC                 glUniform1fv                = NULL;
PFNGLUNIFORM3FVPROC                 glUniform3fv                = NULL;
PFNGLUNIFORM4FVPROC                 glUniform4fv                = NULL;
PFNGLUNIFORMMATRIX3FVPROC           glUniformMatrix3fv          = NULL;
PFNGLUNIFORMMATRIX4FVPROC           glUniformMatrix4fv          = NULL;
/* FBO API */
#if defined(USE_FBO)
PFNGLBINDFRAMEBUFFEREXTPROC         glBindFramebuffer           = NULL;
PFNGLDELETEFRAMEBUFFERSEXTPROC      glDeleteFramebuffers        = NULL;
PFNGLGENFRAMEBUFFERSEXTPROC         glGenFramebuffers           = NULL;
PFNGLCHECKFRAMEBUFFERSTATUSEXTPROC  glCheckFramebufferStatus    = NULL;
PFNGLFRAMEBUFFERTEXTURE2DEXTPROC    glFramebufferTexture2D      = NULL;
PFNGLGENRENDERBUFFERSEXTPROC        glGenRenderbuffers          = NULL;
PFNGLBINDRENDERBUFFEREXTPROC        glBindRenderbuffer          = NULL;
PFNGLRENDERBUFFERSTORAGEEXTPROC     glRenderbufferStorage       = NULL;
PFNGLDELETERENDERBUFFERSEXTPROC     glDeleteRenderbuffers       = NULL;
#endif /* defined(USE_FBO) */
#endif

#if defined(USE_GL1) || defined(USE_GL2)
/* GL_ARB_texture_compression */
PFNGLCOMPRESSEDTEXIMAGE2DARBPROC    glCompressedTexImage2D      = NULL;
PFNGLGETCOMPRESSEDTEXIMAGEARBPROC   glGetCompressedTexImageARB  = NULL;
#endif

#endif
