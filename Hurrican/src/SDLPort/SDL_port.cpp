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

#include "SDL_port.hpp"

#include <filesystem>
namespace fs = std::filesystem;

#if defined(USE_GL1)
GLenum MatrixMode = 0;
#endif
glm::mat4x4 g_matView;
glm::mat4x4 g_matModelView;
#if !defined( __WIN32__) || defined(__MINGW32__)
void strcat_s(char *dst, const char *src) {
    strcat(dst, src);
}

void strcat_s(char *dst, uint32_t size, const char *src) {
    strncat(dst, src, size);
}

void strncat_s(char *dst, const char *src, uint32_t size) {
    strncat(dst, src, size);
}

void strcpy_s(char *dst, int size, const char *src) {
    strncpy(dst, src, size);
}

void strcpy_s(char *dst, const char *src) {
    strcpy(dst, src);
}
#endif

std::vector<char> LoadFileToMemory(const std::string &name) {
    std::ifstream file(name.c_str(), std::ifstream::binary);

    if (!file) {
        Protokoll << "Error file operation: File: " << name << std::endl;
        // FIXME: An exception would be far more useful...
        return std::vector<char>();
    }

    auto filesize = fs::file_size(name);
    std::vector<char> buffer(filesize + 1);
    file.read(buffer.data(), filesize);
    buffer[filesize] = '\0';

    return buffer;
}

bool isPowerOfTwo(int x) {
    return ((x != 0) && !(x & (x - 1)));
}

int nextPowerOfTwo(int x) {
    double logbase2 = log(x) / log(2);
    return static_cast<int>(round(pow(2, ceil(logbase2))));
}

#if defined(USE_GL1)
void load_matrix(GLenum mode, const GLfloat *m) {
    if (MatrixMode != mode) {
        glMatrixMode(mode);
        MatrixMode = mode;
    }
    glLoadMatrixf(m);
}
#endif
