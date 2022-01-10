/**
 *
 *  Copyright (C) 2014 Scott R. Smith
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

#include "cfbo.hpp"
#include "opengl.hpp"

#if defined(USE_FBO)

CFbo::CFbo()
    : Enabled(false),
      texture(GL_INVALID_VALUE),
      framebuffer(GL_INVALID_VALUE),
      width(0),
      height(0) {}

bool CFbo::Open(uint16_t w, uint16_t h) {
    Enabled = false;

#if defined(__WIN32__)
    if (glBindFramebuffer && glDeleteFramebuffers && glGenFramebuffers && glCheckFramebufferStatus &&
        glFramebufferTexture2D && glGenRenderbuffers && glBindRenderbuffer && glRenderbufferStorage &&
        glDeleteRenderbuffers) {
        Protokoll << "FBO supported" << std::endl;
    } else {
        Protokoll << "FBO is NOT supported" << std::endl;
        return Enabled;
    }
#endif

    /* Create a texture to be the color buffer */
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, w, h, 0, GL_RGB, GL_UNSIGNED_SHORT_5_6_5, nullptr);
    /* Create the framebuffer reference */
    glGenFramebuffers(1, &framebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
    /* Attach the texture to the framebuffer */
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture, 0);

    /* Verify the framebuffer is valid */
    GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    switch (status) {
        case GL_FRAMEBUFFER_COMPLETE:
            Enabled = true;
            Protokoll << "GL_FRAMEBUFFER_COMPLETE: created successfully resolution: " << std::dec << w << "x" << h
                      << std::endl;
            break;
        case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT:
            Protokoll << "GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT: Not all framebuffer attachment points are framebuffer "
                         "attachment complete.\n"
                      << std::endl;
            break;
        case GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS:
            Protokoll
                << "GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS: Not all attached images have the same width and height.\n"
                << std::endl;
            break;
        case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT:
            Protokoll << "GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT: No images are attached to the framebuffer.\n"
                      << std::endl;
            break;
        case GL_FRAMEBUFFER_UNSUPPORTED:
            Protokoll << "GL_FRAMEBUFFER_UNSUPPORTED: The combination of internal formats of the attached images "
                         "violates an implementation-dependent set of restrictions.\n"
                      << std::endl;
            break;
        default:
            Protokoll << "FBO failed\n" << std::endl;
            break;
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    return Enabled;
}

void CFbo::BindTexture(bool active) {
    if (active) {
        glBindTexture(GL_TEXTURE_2D, texture);
#if defined(USE_GL1)
        glEnable(GL_TEXTURE_2D);
#endif
    } else {
        glBindTexture(GL_TEXTURE_2D, 0);
#if defined(USE_GL1)
        glDisable(GL_TEXTURE_2D);
#endif
    }
}

void CFbo::Close() {
    if (Enabled == true) {
        glDeleteTextures(1, &texture);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glDeleteFramebuffers(1, &framebuffer);
    }
}

#endif /* USE_FBO */
