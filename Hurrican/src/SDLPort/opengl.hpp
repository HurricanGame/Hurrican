#ifndef _OPENGL_H_
#define _OPENGL_H_

#include <epoxy/gl.h>

// Do some sanity checks
#if !defined(USE_GL1) && !defined(USE_GL2) && !defined(USE_GL3)
#error "ERROR USE_GL1/2/3 are not defined. At least one must be defined."
#elif defined(USE_GL1) && (defined(USE_GL2) || defined(USE_GL3))
#error "ERROR USE_GL1 and USE_GL2/3 are defined. Only one must be defined."
#elif defined(USE_GL2) && defined(USE_GL3)
#error "ERROR USE_GL2 and USE_GL3 are defined. Only one must be defined."
#elif defined(USE_GL1) && (defined(USE_GLES2) || defined(USE_GLES3))
#error "ERROR USE_GL1 and USE_GLES2 are defined. Replace USE_GLES2 or 3 for USE_GLES1"
#elif defined(USE_GL2) && defined(USE_GLES1)
#error "ERROR USE_GL2 and USE_GLES1 are defined. Replace USE_GLES1 for USE_GLES2"
#endif

#endif /* _OPENGL_H_ */
