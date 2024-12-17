#pragma once

#include "Runtime/Core/Public/Core/UICore.h"
#include "Runtime/Core/Public/Renderer/ShaderSources.h"
#include <glad/gl.h>
#include <glm/glm.hpp>
#include <string>
#include <unordered_map>

namespace VGE {
namespace UI {

class UI_API Shader {
public:
    Shader();
    ~Shader();

    bool LoadFromFile(const std::string& vertexPath, const std::string& fragmentPath);
    bool LoadFromSource(const std::string& vertexSource, const std::string& fragmentSource);
    bool LoadFromType(ShaderSources::Type type);
    bool LoadFromCustom(const std::string& name);

    void Bind() const;
    void Unbind() const;

    // Uniform setters
    void SetUniform(const std::string& name, bool value);
    void SetUniform(const std::string& name, int value);
    void SetUniform(const std::string& name, float value);
    void SetUniform(const std::string& name, const glm::vec2& value);
    void SetUniform(const std::string& name, const glm::vec3& value);
    void SetUniform(const std::string& name, const glm::vec4& value);
    void SetUniform(const std::string& name, const glm::mat3& value);
    void SetUniform(const std::string& name, const glm::mat4& value);

private:
    bool CompileShader(const std::string& source, GLenum type, GLuint& shader);
    bool LinkProgram(GLuint vertexShader, GLuint fragmentShader);
    void DeleteShaders(GLuint vertexShader, GLuint fragmentShader);
    GLint GetUniformLocation(const std::string& name);

    GLuint m_Program;
    std::unordered_map<std::string, GLint> m_UniformLocations;
};

} // namespace UI
} // namespace VGE



