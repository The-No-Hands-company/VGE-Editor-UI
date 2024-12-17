#include "Runtime/Core/Public/Renderer/UIShader.h"
#include <glm/gtc/type_ptr.hpp>
#include <fstream>
#include <sstream>
#include <iostream>

namespace VGE {
namespace UI {

uint32_t UIShader::s_BoundShader = 0;

UIShader::UIShader()
    : m_Program(0)
{
}

UIShader::UIShader(const std::string& path)
    : m_Program(0)
{
    LoadFromFile(path);
}

UIShader::~UIShader() {
    Shutdown();
}

bool UIShader::Initialize(const std::string& vertexSource, const std::string& fragmentSource) {
    GLuint vertexShader = 0, fragmentShader = 0;
    
    // Compile vertex shader
    if (!CompileShader(vertexSource, GL_VERTEX_SHADER, vertexShader)) {
        return false;
    }

    // Compile fragment shader
    if (!CompileShader(fragmentSource, GL_FRAGMENT_SHADER, fragmentShader)) {
        glDeleteShader(vertexShader);
        return false;
    }

    // Link shaders
    if (!LinkProgram(vertexShader, fragmentShader)) {
        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);
        return false;
    }

    // Clean up
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    return true;
}

bool UIShader::LoadFromFile(const std::string& path) {
    std::string vertexPath = path + ".vert";
    std::string fragmentPath = path + ".frag";
    
    std::ifstream vertexFile(vertexPath);
    std::ifstream fragmentFile(fragmentPath);
    
    if (!vertexFile.is_open()) {
        std::cerr << "Failed to open vertex shader file: " << vertexPath << std::endl;
        return false;
    }
    
    if (!fragmentFile.is_open()) {
        std::cerr << "Failed to open fragment shader file: " << fragmentPath << std::endl;
        return false;
    }

    std::stringstream vertexBuffer, fragmentBuffer;
    vertexBuffer << vertexFile.rdbuf();
    fragmentBuffer << fragmentFile.rdbuf();
    
    vertexFile.close();
    fragmentFile.close();

    return Initialize(vertexBuffer.str(), fragmentBuffer.str());
}

void UIShader::Shutdown() {
    if (m_Program) {
        glDeleteProgram(m_Program);
        m_Program = 0;
    }
}

bool UIShader::CompileShader(const std::string& source, GLenum type, GLuint& shader) {
    shader = glCreateShader(type);
    const char* sourceCStr = source.c_str();
    glShaderSource(shader, 1, &sourceCStr, nullptr);
    glCompileShader(shader);

    GLint isCompiled = 0;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &isCompiled);
    if (isCompiled == GL_FALSE) {
        GLint maxLength = 0;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &maxLength);

        std::vector<GLchar> infoLog(maxLength);
        glGetShaderInfoLog(shader, maxLength, &maxLength, &infoLog[0]);

        glDeleteShader(shader);
        std::cerr << "Shader compilation failed: " << std::string(infoLog.begin(), infoLog.end()) << std::endl;
        return false;
    }

    return true;
}

bool UIShader::LinkProgram(GLuint vertexShader, GLuint fragmentShader) {
    m_Program = glCreateProgram();
    glAttachShader(m_Program, vertexShader);
    glAttachShader(m_Program, fragmentShader);
    glLinkProgram(m_Program);

    GLint isLinked = 0;
    glGetProgramiv(m_Program, GL_LINK_STATUS, &isLinked);
    if (isLinked == GL_FALSE) {
        GLint maxLength = 0;
        glGetProgramiv(m_Program, GL_INFO_LOG_LENGTH, &maxLength);

        std::vector<GLchar> infoLog(maxLength);
        glGetProgramInfoLog(m_Program, maxLength, &maxLength, &infoLog[0]);

        glDeleteProgram(m_Program);
        std::cerr << "Shader linking failed: " << std::string(infoLog.begin(), infoLog.end()) << std::endl;
        return false;
    }

    return true;
}

void UIShader::Bind() const {
    if (s_BoundShader != m_Program) {
        glUseProgram(m_Program);
        s_BoundShader = m_Program;
    }
}

void UIShader::Unbind() const {
    glUseProgram(0);
    s_BoundShader = 0;
}

void UIShader::Destroy() {
    Shutdown();
}

int UIShader::GetUniformLocation(const std::string& name) {
    if (m_UniformLocationCache.find(name) != m_UniformLocationCache.end())
        return m_UniformLocationCache[name];

    int location = glGetUniformLocation(m_Program, name.c_str());
    m_UniformLocationCache[name] = location;
    return location;
}

void UIShader::CheckCompileErrors(uint32_t shader, const std::string& type) {
    GLint success;
    GLchar infoLog[1024];
    if (type != "PROGRAM") {
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
        if (!success) {
            glGetShaderInfoLog(shader, 1024, nullptr, infoLog);
            std::cerr << "ERROR::SHADER::" << type << "::COMPILATION_FAILED\n" << infoLog << std::endl;
        }
    } else {
        glGetProgramiv(shader, GL_LINK_STATUS, &success);
        if (!success) {
            glGetProgramInfoLog(shader, 1024, nullptr, infoLog);
            std::cerr << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
        }
    }
}

void UIShader::SetBool(const std::string& name, bool value) {
    glUniform1i(GetUniformLocation(name), static_cast<int>(value));
}

void UIShader::SetInt(const std::string& name, int value) {
    glUniform1i(GetUniformLocation(name), value);
}

void UIShader::SetFloat(const std::string& name, float value) {
    glUniform1f(GetUniformLocation(name), value);
}

void UIShader::SetVec2(const std::string& name, const glm::vec2& value) {
    glUniform2fv(GetUniformLocation(name), 1, glm::value_ptr(value));
}

void UIShader::SetVec3(const std::string& name, const glm::vec3& value) {
    glUniform3fv(GetUniformLocation(name), 1, glm::value_ptr(value));
}

void UIShader::SetVec4(const std::string& name, const glm::vec4& value) {
    glUniform4fv(GetUniformLocation(name), 1, glm::value_ptr(value));
}

void UIShader::SetMat3(const std::string& name, const glm::mat3& value) {
    glUniformMatrix3fv(GetUniformLocation(name), 1, GL_FALSE, glm::value_ptr(value));
}

void UIShader::SetMat4(const std::string& name, const glm::mat4& value) {
    glUniformMatrix4fv(GetUniformLocation(name), 1, GL_FALSE, glm::value_ptr(value));
}

void UIShader::SetIntArray(const std::string& name, const int* values, uint32_t count) {
    glUniform1iv(GetUniformLocation(name), count, values);
}

void UIShader::SetFloatArray(const std::string& name, const float* values, uint32_t count) {
    glUniform1fv(GetUniformLocation(name), count, values);
}

}} // namespace VGE::UI


