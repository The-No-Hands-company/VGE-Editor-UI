#pragma once

#include "Runtime/Core/Public/Core/UICore.h"

#include <glad/gl.h>
#include <iostream>

namespace VGE {
namespace UI {

namespace ShaderUtils {

inline bool CheckShaderCompilation(GLuint shader, const char* type) {
    GLint success;
    GLchar infoLog[512];
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(shader, sizeof(infoLog), nullptr, infoLog);
        std::cerr << "Shader compilation error (" << type << "): " << infoLog << std::endl;
        return false;
    }
    return true;
}

inline bool CheckProgramLinking(GLuint program) {
    GLint success;
    GLchar infoLog[512];
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(program, sizeof(infoLog), nullptr, infoLog);
        std::cerr << "Shader program linking error: " << infoLog << std::endl;
        return false;
    }
    return true;
}

inline GLuint CreateShader(GLenum type, const char* source, const char* name) {
    GLuint shader = glCreateShader(type);
    glShaderSource(shader, 1, &source, nullptr);
    glCompileShader(shader);
    
    if (!CheckShaderCompilation(shader, name)) {
        glDeleteShader(shader);
        return 0;
    }
    return shader;
}

inline GLuint CreateShaderProgram(const char* vertexSource, const char* fragmentSource) {
    GLuint vertexShader = CreateShader(GL_VERTEX_SHADER, vertexSource, "vertex");
    if (!vertexShader) return 0;

    GLuint fragmentShader = CreateShader(GL_FRAGMENT_SHADER, fragmentSource, "fragment");
    if (!fragmentShader) {
        glDeleteShader(vertexShader);
        return 0;
    }

    GLuint program = glCreateProgram();
    glAttachShader(program, vertexShader);
    glAttachShader(program, fragmentShader);
    glLinkProgram(program);

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    if (!CheckProgramLinking(program)) {
        glDeleteProgram(program);
        return 0;
    }

    return program;
}

} // namespace ShaderUtils

} // namespace UI
} // namespace VGE 


