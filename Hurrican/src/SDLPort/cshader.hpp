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
#ifndef CSHADER_H
#define CSHADER_H

#include <string>
#include <vector>
#include "SDL_port.hpp"
#include "opengl.hpp"

#define CHECK_FLAG(X, Y) (((X) & (Y)) == (Y))

enum { SHADER = 0x01, PROGRAM = 0x02 };

struct shader_t {
    std::string path;
    GLenum type;
    GLuint name;

    shader_t() : path("none"), type(GL_INVALID_VALUE), name(GL_INVALID_VALUE){};
};

class CShader {
  public:
    CShader();

    void Close();
    bool Load(const std::string &path_vertex, const std::string &path_frag);
    void Use();
    GLint GetAttribute(const std::string &attribute);
    GLint GetUniform(const std::string &attribute);
    void AddConstant(std::string name, GLint value);

    GLuint NamePos;
    GLuint NameClr;
    GLuint NameTex;
    GLuint NameMvp;
#if defined(USE_ETC1)
    GLuint texUnit0;
    GLuint texUnit1;
#endif

  private:
    bool LoadShader(GLenum type, const std::string &path);
    bool CreateProgram();
    GLuint CompileShader(GLenum type, const std::string &path);
    void FindAttributes();
    void FindUniforms();
    void PrintLog(uint8_t type, GLuint shader = 0);

    GLuint Program;
    std::vector<shader_t> Shaders;
    std::vector<std::pair<std::string, GLint> > Uniforms;
    std::vector<std::pair<std::string, GLint> > Attributes;
    std::vector<std::pair<std::string, GLint> > Constants;
};

extern glm::mat4x4 g_matView;
extern glm::mat4x4 g_matModelView;

#endif  // CSHADER_H
