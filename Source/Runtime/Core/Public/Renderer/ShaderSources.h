#pragma once

#include "Runtime/Core/Public/Core/UICore.h"
#include <string>
#include <unordered_map>

namespace VGE {
namespace UI {

class UI_API ShaderSources {
public:
    static ShaderSources& Get();

    // Built-in shader types
    enum class Type {
        UI_Basic,
        UI_Text,
        UI_Texture,
        UI_ColoredRect,
        UI_RoundedRect,
        UI_Circle,
        UI_Line,
        UI_Custom
    };

    struct ShaderSource {
        std::string vertexSource;
        std::string fragmentSource;
    };

    // Get shader source by type
    const ShaderSource& GetShaderSource(Type type) const;

    // Register custom shader source
    void RegisterCustomShader(const std::string& name, const ShaderSource& source);
    const ShaderSource& GetCustomShaderSource(const std::string& name) const;

private:
    ShaderSources();
    ~ShaderSources() = default;
    ShaderSources(const ShaderSources&) = delete;
    ShaderSources& operator=(const ShaderSources&) = delete;

    void InitializeBuiltInShaders();

    std::unordered_map<Type, ShaderSource> m_BuiltInShaders;
    std::unordered_map<std::string, ShaderSource> m_CustomShaders;

    friend ShaderSources& Get();
};

} // namespace UI
} // namespace VGE 