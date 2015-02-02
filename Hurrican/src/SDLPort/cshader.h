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
#include <string.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include "opengl.h"
#include "Logdatei.h"
#include "cml/cml.h"

using namespace std;

#define CHECK_FLAG(X,Y) ((X & Y) == Y)

enum
{
    SHADER  =0x01,
    PROGRAM =0x02
};

typedef struct SHADER_T
{
    string  path;
    GLenum  type;
    GLuint  name;

    SHADER_T() :
        path("none"),
        type(GL_INVALID_VALUE),
        name(GL_INVALID_VALUE)
    {};
} shader_t;

class CShader
{
public:
    CShader();
    virtual ~CShader();

    void        Close           ( void );
    int8_t      Load            ( const string& path_vertex, const string& path_frag );
    void        Use             ( void );
    GLint       GetAttribute    ( const string& attribute );
    GLint       GetUniform      ( const string& attribute );

    GLuint      NamePos;
    GLuint      NameClr;
    GLuint      NameTex;
    GLuint      NameMvp;
#if defined(USE_ETC1)
    GLuint      texUnit0;
    GLuint      texUnit1;
#endif

private:
    int8_t      LoadShader      ( GLenum type, const string& path );
    int8_t      CreateProgram   ( void );
    GLuint      CompileShader   ( GLenum type, const string& path );
    void        FindAttributes  ( void );
    void        FindUniforms    ( void );
    void        PrintLog        ( uint8_t type, GLuint shader=0 );

    GLuint                          Program;
    vector<shader_t>                Shaders;
    vector< pair<string, GLint> >   Uniforms;
    vector< pair<string, GLint> >   Attributes;
};

extern cml::matrix44f_r g_matView;
extern cml::matrix44f_r g_matModelView;

extern uint8_t* LoadFileToMemory( const std::string& name, uint32_t& size );

#endif // CSHADER_H
