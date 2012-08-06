/**
 *
 *  EGLPORT.H
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

#ifndef EGLPORT_H
#define EGLPORT_H

#if defined(USE_EGL_SDL)
#define SUPPORT_X11 1
#endif

#include <stdint.h>
#include "egl.h"

#ifdef __cplusplus
extern "C" {
#endif

/** Defines (in every case choose only one) */
/**     DEBUG : enable additional error monitoring per EGL function call */
/**   Native display and window system for use with EGL */
/**     USE_EGL_RAW : used for direct access to the framebuffer */
/**     USE_EGL_SDL : used for access to a X window */
/**     USE_EGL_RPI : used for raspberry pi window */
/**   GLES Version */
/**     USE_GLES1 : EGL for use with OpenGL-ES 1.X contexts */
/**     USE_GLES2 : EGL for use with OpenGL-ES 2.0 contexts */
/**   Platform: settings that are specific to that device */
/**     PANDORA (use USE_EGL_SDL and USE_GLES1 or USE_GLES2)*/
/**     WIZ     (use USE_EGL_RAW and USE_GLES1)*/
/**     CAANOO  (use USE_EGL_RAW and USE_GLES1)*/
/**     RPI     (use USE_EGL_RAW and USE_GLES1 or USE_GLES2)*/

/** Public API */
void    EGL_Close                   ( void );
int8_t  EGL_Open                    ( void );
int8_t  EGL_Init                    ( void );
void    EGL_SwapBuffers             ( void );

/**   Simple Examples  */
/**     Raw mode:
          EGL_Open();
          EGL_Init();
          do while(!quit) {
            ... run app
            EGL_SwapBuffers();
          }
          EGL_Close();
*/
/**     X/SDL mode:
          SDL_Init( SDL_INIT_VIDEO );
          EGL_Open();
          SDL_Surface* screen = SDL_SetVideoMode(640, 480, 16, SDL_SWSURFACE|SDL_FULLSCREEN);
          EGL_Init();
          do while(!quit) {
            ... run app
            EGL_SwapBuffers();
          }
          EGL_Close();
          SDL_Quit();
*/

#if defined(DEBUG)
#define GET_EGLERROR(FUNCTION)               \
    FUNCTION;                                \
    {                                        \
        CheckEGLErrors(__FILE__, __LINE__);  \
    }
#else
#define GET_EGLERROR(FUNCTION) FUNCTION;
#endif

#define peglQueryString(A,B)                    GET_EGLERROR(eglQueryString(A,B))
#define peglDestroyContext(A,B)                 GET_EGLERROR(eglDestroyContext(A,B))
#define peglDestroySurface(A,B)                 GET_EGLERROR(eglDestroySurface(A,B))
#define peglTerminate(A)                        GET_EGLERROR(eglTerminate(A))
#define peglSwapBuffers(A,B)                    GET_EGLERROR(eglSwapBuffers(A,B))
#define peglGetDisplay(A)                       GET_EGLERROR(eglGetDisplay(A))
#define peglBindAPI(A)                          GET_EGLERROR(eglBindAPI(A))
#define peglCreateContext(A,B,C,D)              GET_EGLERROR(eglCreateContext(A,B,C,D))
#define peglCreateWindowSurface(A,B,C,D)        GET_EGLERROR(eglCreateWindowSurface(A,B,C,D))
#define peglInitialize(A,B,C)                   GET_EGLERROR(eglInitialize(A,B,C))
#define peglMakeCurrent(A,B,C,D)                GET_EGLERROR(eglMakeCurrent(A,B,C,D))
#define peglChooseConfig(A,B,C,D,E)             GET_EGLERROR(eglChooseConfig(A,B,C,D,E))

#ifdef __cplusplus
}
#endif

#endif // EGLPORT_H
