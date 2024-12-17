#pragma once

#include "Runtime/Core/Public/Core/UICore.h"
#include <glad/gl.h>
#include <string>
#include <unordered_map>
#include <glm/glm.hpp>

namespace VGE {
namespace UI {

class UI_API UIShader {
public:
    UIShader();
    UIShader(const std::string& path);
    ~UIShader();

    bool Initialize(const std::string& vertexSource, const std::string& fragmentSource);
    bool LoadFromFile(const std::string& path);
    void Shutdown();

    bool Compile(const std::string& source, GLenum type, GLuint& shader);
    void Destroy();

    void Bind() const;
    void Unbind() const;

    // Uniform setters
    void SetBool(const std::string& name, bool value);
    void SetInt(const std::string& name, int value);
    void SetFloat(const std::string& name, float value);
    void SetVec2(const std::string& name, const glm::vec2& value);
    void SetVec3(const std::string& name, const glm::vec3& value);
    void SetVec4(const std::string& name, const glm::vec4& value);
    void SetMat3(const std::string& name, const glm::mat3& value);
    void SetMat4(const std::string& name, const glm::mat4& value);
    void SetIntArray(const std::string& name, const int* values, uint32_t count);
    void SetFloatArray(const std::string& name, const float* values, uint32_t count);

    // State management
    bool IsBound() const { return s_BoundShader == m_Program; }
    uint32_t GetProgram() const { return m_Program; }

private:
    bool CompileShader(const std::string& source, uint32_t type, uint32_t& shader);
    bool LinkProgram(uint32_t vertexShader, uint32_t fragmentShader);
    int GetUniformLocation(const std::string& name);
    void CheckCompileErrors(uint32_t shader, const std::string& type);

private:
    uint32_t m_Program;
    std::unordered_map<std::string, int> m_UniformLocationCache;

    // Static state tracking
    static uint32_t s_BoundShader;
};

}} // namespace VGE::UI
