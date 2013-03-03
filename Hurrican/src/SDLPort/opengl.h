#ifndef _OPENGL_H_
#define _OPENGL_H_

// Do some sanity checks
#if !defined(USE_GL1) && !defined(USE_GL2)
    #error "ERROR USE_GL1 nor USE_GL2 are defined. Only one must be defined."
#elif defined(USE_GL1) && defined(USE_GL2)
    #error "ERROR USE_GL1 and USE_GL2 are defined. Only one must be defined."
#elif defined(USE_GL1) && defined(USE_GLES2)
    #error "ERROR USE_GL1 and USE_GLES2 are defined. Replace USE_GLES2 for USE_GLES1"
#elif defined(USE_GL2) && defined(USE_GLES1)
    #error "ERROR USE_GL2 and USE_GLES1 are defined. Replace USE_GLES1 for USE_GLES2"
#endif

#if !defined(__WIN32)
#define GL_GLEXT_PROTOTYPES 1
#endif
#if defined(USE_GLES1) || defined(USE_GLES2)
/* OpenGL-ES Profiles */
#if defined(USE_GLES2)
#include "gl2.h"
#include "gl2ext.h"
#else
#include "gl.h"
#include "glext.h"
#endif
#define glClearDepth glClearDepthf

#else /* Full OpenGL Profile */
#include "SDL_opengl.h"
#endif

#if defined(__WIN32__)
#if defined(USE_GL2)
/* OpenGL Version 2.0 API */
extern PFNGLDELETESHADERPROC                glDeleteShader;
extern PFNGLDELETEPROGRAMPROC               glDeleteProgram;
extern PFNGLUSEPROGRAMPROC                  glUseProgram;
extern PFNGLCREATESHADERPROC                glCreateShader;
extern PFNGLSHADERSOURCEPROC                glShaderSource;
extern PFNGLCOMPILESHADERPROC               glCompileShader;
extern PFNGLGETSHADERIVPROC                 glGetShaderiv;
extern PFNGLCREATEPROGRAMPROC               glCreateProgram;
extern PFNGLATTACHSHADERPROC                glAttachShader;
extern PFNGLLINKPROGRAMPROC                 glLinkProgram;
extern PFNGLGETPROGRAMIVPROC                glGetProgramiv;
extern PFNGLGETACTIVEATTRIBPROC             glGetActiveAttrib;
extern PFNGLGETATTRIBLOCATIONPROC           glGetAttribLocation;
extern PFNGLGETACTIVEUNIFORMPROC            glGetActiveUniform;
extern PFNGLGETUNIFORMLOCATIONPROC          glGetUniformLocation;
extern PFNGLGETSHADERINFOLOGPROC            glGetShaderInfoLog;
extern PFNGLGETPROGRAMINFOLOGPROC           glGetProgramInfoLog;
extern PFNGLDISABLEVERTEXATTRIBARRAYPROC    glDisableVertexAttribArray;
extern PFNGLENABLEVERTEXATTRIBARRAYPROC     glEnableVertexAttribArray;
extern PFNGLVERTEXATTRIBPOINTERPROC         glVertexAttribPointer;
extern PFNGLUNIFORM1IPROC                   glUniform1i;
extern PFNGLUNIFORM1FVPROC                  glUniform1fv;
extern PFNGLUNIFORM3FVPROC                  glUniform3fv;
extern PFNGLUNIFORM4FVPROC                  glUniform4fv;
extern PFNGLUNIFORMMATRIX3FVPROC            glUniformMatrix3fv;
extern PFNGLUNIFORMMATRIX4FVPROC            glUniformMatrix4fv;
#endif

#if defined(USE_GL1) || defined(USE_GL2)
/* GL_ARB_texture_compression */
extern PFNGLCOMPRESSEDTEXIMAGE2DARBPROC     glCompressedTexImage2D;
extern PFNGLGETCOMPRESSEDTEXIMAGEARBPROC    glGetCompressedTexImageARB;
#endif
#endif

#endif /* _OPENGL_H_ */
