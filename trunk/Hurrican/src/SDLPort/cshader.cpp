/**
 *
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

#include "cshader.h"

CShader::CShader() :
    NamePos (GL_INVALID_VALUE),
    NameClr (GL_INVALID_VALUE),
    NameTex (GL_INVALID_VALUE),
    NameMvp (GL_INVALID_VALUE),
    Program (GL_INVALID_VALUE),
    Shaders (),
    Uniforms(),
    Attributes()
{
}

CShader::~CShader()
{
}

void CShader::Close( void )
{
    uint16_t i;

    for(i=0; i<Shaders.size(); i++)
    {
        if ( Shaders.at(i).name != GL_INVALID_VALUE) {
            glDeleteShader( Shaders.at(i).name );
        }
    }

    if (Program != GL_INVALID_VALUE) {
        glDeleteProgram( Program );
    }

    Program = GL_INVALID_VALUE;
    Shaders.clear();
}

int8_t CShader::Load( const string& path_vertex, const string& path_frag )
{
    if (LoadShader( GL_VERTEX_SHADER, path_vertex ))
        return 1;

    if (LoadShader( GL_FRAGMENT_SHADER, path_frag ))
        return 1;

    if (CreateProgram())
        return 1;

    return 0;
}

int8_t CShader::LoadShader( GLenum type, const string& path )
{
    shader_t shader;

    printf( "Shader: Compiling %s\n", path.c_str() );

    shader.type = type;
    shader.path = path;
    shader.name = CompileShader( shader.type, shader.path );

    if (shader.name == GL_INVALID_VALUE) {
        return 1;
    }
    else {
        printf( "Shader: Compiled %s\n", path.c_str() );
    }

    Shaders.push_back( shader );

    return 0;
}

void CShader::Use( void )
{
    glUseProgram( Program );
}

GLuint CShader::CompileShader( GLenum type, const string& path )
{
    GLint       status;
    GLuint      shader;
    string      shadertype;
    uint8_t*    source;
    uint32_t    size = 0;

    source = LoadFileToMemory( path, size );

#if defined(USE_GLES2)
    uint8_t     length;
    uint8_t*    temp;
    const char* precision = "precision mediump float;\n";

    length = strlen(precision);
    temp = new uint8_t[size+length];

    memcpy( temp, precision, length );
    memcpy( temp+length, source, size );

    delete [] source;
    source = temp;
#endif

    if (source != NULL)
    {
#if defined(DEBUG)
        printf( "Shader Source Begin\n%s\nShader Source End\n", source );
#endif

        shader = glCreateShader( type );
        glShaderSource( shader, 1, (const char**)&source, NULL );
        glCompileShader( shader );

        delete [] source;

        glGetShaderiv( shader, GL_COMPILE_STATUS, &status );
        if (status == GL_FALSE)
        {
            switch(type)
            {
                case GL_VERTEX_SHADER:      shadertype = "vertex"; break;
                case GL_FRAGMENT_SHADER:    shadertype = "fragment"; break;
                default :                   shadertype = "unknown"; break;
            }

            printf( "ERROR Shader: Compile failure in %s shader: %s\n", shadertype.c_str(), path.c_str() );
            PrintLog( SHADER, shader );
            return GL_INVALID_VALUE;
        }
    }
    else
    {
        printf( "ERROR Shader: File read failure in %s shader: %s\n", shadertype.c_str(), path.c_str() );
        return GL_INVALID_VALUE;
    }

    return shader;
}

int8_t CShader::CreateProgram( void )
{
    uint16_t i;
    GLint   status;

    Program = glCreateProgram();

    for(i = 0; i < Shaders.size(); i++) {
        glAttachShader( Program, Shaders[i].name );
    }

    glLinkProgram( Program );

    glGetProgramiv( Program, GL_LINK_STATUS, &status );
    if (status == GL_FALSE)
    {
        printf( "ERROR Shader: Linker failure\n" );
        PrintLog( PROGRAM );
        Program = GL_INVALID_VALUE;
        return 1 ;
    }

    FindAttributes();
    FindUniforms();

    return 0;
}

void CShader::FindAttributes( void )
{
    GLint numAttributes;
    GLint maxAttributeLen;
    char* attributeName;

    glGetProgramiv( Program, GL_ACTIVE_ATTRIBUTES, &numAttributes );
    glGetProgramiv( Program, GL_ACTIVE_ATTRIBUTE_MAX_LENGTH, &maxAttributeLen );
    attributeName = new char[maxAttributeLen];

    for (GLint index(0); index < numAttributes; ++index)
    {
        GLint size;
        GLenum type;
        GLint location;

        glGetActiveAttrib( Program, index, maxAttributeLen, NULL, &size, &type, attributeName );
        location = glGetAttribLocation( Program, attributeName );

        pair<string, GLint> parameter;
        parameter.first = string(attributeName);
        parameter.second = location;
        Attributes.push_back(parameter);
    }

    delete [] attributeName;
}

void CShader::FindUniforms( void )
{
    GLint numUniforms;
    GLint maxUniformLen;
    char* uniformName;

    glGetProgramiv( Program, GL_ACTIVE_UNIFORMS, &numUniforms );
    glGetProgramiv( Program, GL_ACTIVE_UNIFORM_MAX_LENGTH, &maxUniformLen );
    uniformName = new char[maxUniformLen];

    for (GLint index(0); index < numUniforms; ++index)
    {
        GLint size;
        GLenum type;
        GLint location;

        glGetActiveUniform( Program, index, maxUniformLen, NULL, &size, &type, uniformName );
        location = glGetUniformLocation( Program, uniformName );

        pair<string, GLint> parameter;
        parameter.first = string(uniformName);
        parameter.second = location;
        Uniforms.push_back(parameter);
    }

    delete [] uniformName;
}

GLint CShader::GetAttribute( const string& attribute )
{
    for (vector<pair<string, GLint> >::const_iterator itr(Attributes.begin()); itr < Attributes.end(); ++itr) {
        if (attribute == itr->first)
            return itr->second;
    }

    printf( "ERROR Shader: Could not locate attribute: %s\n", attribute.c_str() );

    return GL_INVALID_VALUE;
}

GLint CShader::GetUniform( const string& uniform )
{
    for (vector<pair<string, GLint> >::const_iterator itr(Uniforms.begin()); itr < Uniforms.end(); ++itr) {
        if (uniform == itr->first)
            return itr->second;
    }

    printf( "ERROR Shader: Could not locate uniform: %s\n", uniform.c_str() );

    return GL_INVALID_VALUE;
}

void CShader::PrintLog( uint8_t type, GLuint shader )
{
    GLint   loglength;
    char* log;

    if (CHECK_FLAG(type, SHADER))
    {
        glGetShaderiv( shader, GL_INFO_LOG_LENGTH, &loglength );

        log = new char[loglength+1];
        glGetShaderInfoLog( shader, loglength, NULL, log );
        printf( "Shader: Shader log:\n %s", log );
        delete [] log;
    }

    if (CHECK_FLAG(type, PROGRAM))
    {
        glGetProgramiv( Program, GL_INFO_LOG_LENGTH, &loglength );

        log = new char[loglength + 1];
        glGetProgramInfoLog( Program, loglength, NULL, log );
        printf( "Shader: Program log:\n %s", log );
        delete [] log;
    }
}
