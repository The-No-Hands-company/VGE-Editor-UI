#include "Runtime/Core/Public/Renderer/Shader.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <glm/gtc/type_ptr.hpp>

namespace VGE {
namespace UI {

Shader::Shader()
    : m_Program(0)
{
}

Shader::~Shader() {
    if (m_Program) {
        glDeleteProgram(m_Program);
    }
}

bool Shader::LoadFromFile(const std::string& vertexPath, const std::string& fragmentPath) {
    // Read vertex shader
    std::ifstream vertexFile(vertexPath);
    if (!vertexFile.is_open()) {
        std::cerr << "Failed to open vertex shader file: " << vertexPath << std::endl;
        return false;
    }
    std::stringstream vertexStream;
    vertexStream << vertexFile.rdbuf();
    vertexFile.close();
    std::string vertexSource = vertexStream.str();

    // Read fragment shader
    std::ifstream fragmentFile(fragmentPath);
    if (!fragmentFile.is_open()) {
        std::cerr << "Failed to open fragment shader file: " << fragmentPath << std::endl;
        return false;
    }
    std::stringstream fragmentStream;
    fragmentStream << fragmentFile.rdbuf();
    fragmentFile.close();
    std::string fragmentSource = fragmentStream.str();

    return LoadFromSource(vertexSource, fragmentSource);
}

bool Shader::LoadFromSource(const std::string& vertexSource, const std::string& fragmentSource) {
    GLuint vertexShader = 0, fragmentShader = 0;

    // Compile shaders
    if (!CompileShader(vertexSource, GL_VERTEX_SHADER, vertexShader)) {
        return false;
    }
    if (!CompileShader(fragmentSource, GL_FRAGMENT_SHADER, fragmentShader)) {
        glDeleteShader(vertexShader);
        return false;
    }

    // Link program
    if (!LinkProgram(vertexShader, fragmentShader)) {
        DeleteShaders(vertexShader, fragmentShader);
        return false;
    }

    DeleteShaders(vertexShader, fragmentShader);
    return true;
}

bool Shader::LoadFromType(ShaderSources::Type type) {
    try {
        const auto& source = ShaderSources::Get().GetShaderSource(type);
        return LoadFromSource(source.vertexSource, source.fragmentSource);
    } catch (const std::runtime_error& e) {
        std::cerr << "Failed to load shader from type: " << e.what() << std::endl;
        return false;
    }
}

bool Shader::LoadFromCustom(const std::string& name) {
    try {
        const auto& source = ShaderSources::Get().GetCustomShaderSource(name);
        return LoadFromSource(source.vertexSource, source.fragmentSource);
    } catch (const std::runtime_error& e) {
        std::cerr << "Failed to load custom shader: " << e.what() << std::endl;
        return false;
    }
}

void Shader::Bind() const {
    glUseProgram(m_Program);
}

void Shader::Unbind() const {
    glUseProgram(0);
}

void Shader::SetUniform(const std::string& name, bool value) {
    glUniform1i(GetUniformLocation(name), value ? 1 : 0);
}

void Shader::SetUniform(const std::string& name, int value) {
    glUniform1i(GetUniformLocation(name), value);
}

void Shader::SetUniform(const std::string& name, float value) {
    glUniform1f(GetUniformLocation(name), value);
}

void Shader::SetUniform(const std::string& name, const glm::vec2& value) {
    glUniform2f(GetUniformLocation(name), value.x, value.y);
}

void Shader::SetUniform(const std::string& name, const glm::vec3& value) {
    glUniform3f(GetUniformLocation(name), value.x, value.y, value.z);
}

void Shader::SetUniform(const std::string& name, const glm::vec4& value) {
    glUniform4f(GetUniformLocation(name), value.x, value.y, value.z, value.w);
}

void Shader::SetUniform(const std::string& name, const glm::mat3& value) {
    glUniformMatrix3fv(GetUniformLocation(name), 1, GL_FALSE, glm::value_ptr(value));
}

void Shader::SetUniform(const std::string& name, const glm::mat4& value) {
    glUniformMatrix4fv(GetUniformLocation(name), 1, GL_FALSE, glm::value_ptr(value));
}

bool Shader::CompileShader(const std::string& source, GLenum type, GLuint& shader) {
    shader = glCreateShader(type);
    const char* sourceCStr = source.c_str();
    glShaderSource(shader, 1, &sourceCStr, nullptr);
    glCompileShader(shader);

    GLint success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        GLchar infoLog[512];
        glGetShaderInfoLog(shader, sizeof(infoLog), nullptr, infoLog);
        std::cerr << "Shader compilation failed:\n" << infoLog << std::endl;
        glDeleteShader(shader);
        return false;
    }

    return true;
}

bool Shader::LinkProgram(GLuint vertexShader, GLuint fragmentShader) {
    m_Program = glCreateProgram();
    glAttachShader(m_Program, vertexShader);
    glAttachShader(m_Program, fragmentShader);
    glLinkProgram(m_Program);

    GLint success;
    glGetProgramiv(m_Program, GL_LINK_STATUS, &success);
    if (!success) {
        GLchar infoLog[512];
        glGetProgramInfoLog(m_Program, sizeof(infoLog), nullptr, infoLog);
        std::cerr << "Program linking failed:\n" << infoLog << std::endl;
        glDeleteProgram(m_Program);
        m_Program = 0;
        return false;
    }

    return true;
}

void Shader::DeleteShaders(GLuint vertexShader, GLuint fragmentShader) {
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
}

GLint Shader::GetUniformLocation(const std::string& name) {
    auto it = m_UniformLocations.find(name);
    if (it != m_UniformLocations.end()) {
        return it->second;
    }

    GLint location = glGetUniformLocation(m_Program, name.c_str());
    m_UniformLocations[name] = location;
    return location;
}

} // namespace UI
} // namespace VGE 