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

#include "cshader.hpp"
#include "SDL_port.hpp"

CShader::CShader()
    : NamePos(GL_INVALID_VALUE),
      NameClr(GL_INVALID_VALUE),
      NameTex(GL_INVALID_VALUE),
      NameMvp(GL_INVALID_VALUE),
      Program(GL_INVALID_VALUE),
      Shaders(),
      Uniforms(),
      Attributes() {}

void CShader::Close() {
    for (const auto& shader: Shaders) {
        if (shader.name != GL_INVALID_VALUE) {
            glDeleteShader(shader.name);
        }
    }

    if (Program != GL_INVALID_VALUE) {
        glDeleteProgram(Program);
    }

    Program = GL_INVALID_VALUE;
    Shaders.clear();
}

bool CShader::Load(const std::string &path_vertex, const std::string &path_frag) {
    if (!LoadShader(GL_VERTEX_SHADER, path_vertex))
        return false;

    if (!LoadShader(GL_FRAGMENT_SHADER, path_frag))
        return false;

    if (!CreateProgram())
        return false;

    return true;
}

bool CShader::LoadShader(GLenum type, const std::string &path) {
    Protokoll << "Shader: Compiling " << path << std::endl;

    shader_t shader;
    shader.type = type;
    shader.path = path;
    shader.name = CompileShader(shader.type, shader.path);

    if (shader.name == GL_INVALID_VALUE) {
        return false;
    } else {
        Protokoll << "Shader: Compiled " << path << std::endl;
    }

    Shaders.push_back(shader);

    return true;
}

void CShader::Use() {
    glUseProgram(Program);
}

GLuint CShader::CompileShader(GLenum type, const std::string &path) {
    std::vector<char> source = LoadFileToMemory(path);

#if defined(USE_GLES2)
    const std::string version = "#version 100\n";
    const std::string precision = "precision mediump float;\n";
    source.insert(source.begin(), precision.begin(), precision.end());
    source.insert(source.begin(), version.begin(), version.end());
#elif defined(USE_GLES3)
    const std::string version = "#version 320 es\n";
    const std::string precision = "#ifdef GL_FRAGMENT_PRECISION_HIGH\nprecision highp float;\n#else\nprecision mediump float;\n#endif\n";
    source.insert(source.begin(), precision.begin(), precision.end());
    source.insert(source.begin(), version.begin(), version.end());
#elif defined(USE_GL2)
    const std::string version = "#version 110\n";
    source.insert(source.begin(), version.begin(), version.end());
#elif defined(USE_GL3)
    const std::string version = "#version 130\n";
    source.insert(source.begin(), version.begin(), version.end());
#endif

    if (!source.empty()) {
#if defined(DEBUG)
        Protokoll << "Shader Source Begin\n" << source << "\nShader Source End" << std::endl;
#endif

        GLuint shader = glCreateShader(type);
        const GLchar *shader_src = source.data();
        glShaderSource(shader, 1, &shader_src, nullptr);
        glCompileShader(shader);

        GLint status;
        glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
        std::string shadertype;
        if (status == GL_FALSE) {
            switch (type) {
                case GL_VERTEX_SHADER:
                    shadertype = "vertex";
                    break;
                case GL_FRAGMENT_SHADER:
                    shadertype = "fragment";
                    break;
                default:
                    shadertype = "unknown";
                    break;
            }

            Protokoll << "ERROR Shader: Compile failure in " << shadertype << " shader: " << path << std::endl;
            PrintLog(SHADER, shader);
            return GL_INVALID_VALUE;
        }

        return shader;
    } else {
        Protokoll << "ERROR Shader: File read failure in shader: " << path << std::endl;
        return GL_INVALID_VALUE;
    }
}

bool CShader::CreateProgram() {
    Program = glCreateProgram();

    for (const auto& shader: Shaders) {
        glAttachShader(Program, shader.name);
    }

    glLinkProgram(Program);

    GLint status;
    glGetProgramiv(Program, GL_LINK_STATUS, &status);
    if (status == GL_FALSE) {
        Protokoll << "ERROR Shader: Linker failure" << std::endl;
        PrintLog(PROGRAM);
        Program = GL_INVALID_VALUE;
        return false;
    }

    FindAttributes();
    FindUniforms();

    return true;
}

void CShader::FindAttributes() {
    GLint numAttributes;
    GLint maxAttributeLen;

    glGetProgramiv(Program, GL_ACTIVE_ATTRIBUTES, &numAttributes);
    glGetProgramiv(Program, GL_ACTIVE_ATTRIBUTE_MAX_LENGTH, &maxAttributeLen);
    char *attributeName = new char[maxAttributeLen];

    for (GLint index(0); index < numAttributes; ++index) {
        GLint size;
        GLenum type;

        glGetActiveAttrib(Program, index, maxAttributeLen, nullptr, &size, &type, attributeName);
        GLint location = glGetAttribLocation(Program, attributeName);

        std::pair<std::string, GLint> parameter;
        parameter.first = std::string(attributeName);
        parameter.second = location;
        Attributes.push_back(parameter);
    }

    delete[] attributeName;
}

void CShader::FindUniforms() {
    GLint numUniforms;
    GLint maxUniformLen;

    glGetProgramiv(Program, GL_ACTIVE_UNIFORMS, &numUniforms);
    glGetProgramiv(Program, GL_ACTIVE_UNIFORM_MAX_LENGTH, &maxUniformLen);
    char *uniformName = new char[maxUniformLen];

    for (GLint index(0); index < numUniforms; ++index) {
        GLint size;
        GLenum type;

        glGetActiveUniform(Program, index, maxUniformLen, nullptr, &size, &type, uniformName);
        GLint location = glGetUniformLocation(Program, uniformName);

        std::pair<std::string, GLint> parameter;
        parameter.first = std::string(uniformName);
        parameter.second = location;
        Uniforms.push_back(parameter);
    }

    delete[] uniformName;
}

GLint CShader::GetAttribute(const std::string &attribute) {
    for (const auto& a: Attributes) {
        if (attribute == a.first)
            return a.second;
    }

    Protokoll << "ERROR Shader: Could not locate attribute: " << attribute << std::endl;

    return GL_INVALID_VALUE;
}

GLint CShader::GetUniform(const std::string &uniform) {
    for (const auto& u: Uniforms) {
        if (uniform == u.first)
            return u.second;
    }

    Protokoll << "ERROR Shader: Could not locate uniform: " << uniform << std::endl;

    return GL_INVALID_VALUE;
}

void CShader::PrintLog(uint8_t type, GLuint shader) {
    GLint loglength;

    if (CHECK_FLAG(type, SHADER)) {
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &loglength);

        char *log = new char[loglength + 1];
        glGetShaderInfoLog(shader, loglength, nullptr, log);
        Protokoll << "Shader: Shader log:\n " << log << std::endl;
        delete[] log;
    }

    if (CHECK_FLAG(type, PROGRAM)) {
        glGetProgramiv(Program, GL_INFO_LOG_LENGTH, &loglength);

        char *log = new char[loglength + 1];
        glGetProgramInfoLog(Program, loglength, nullptr, log);
        Protokoll << "Shader: Program log:\n " << log << std::endl;
        delete[] log;
    }
}
