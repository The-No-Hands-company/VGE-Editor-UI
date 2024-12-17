#include "Runtime/Core/Public/Renderer/ShaderSources.h"

namespace VGE {
namespace UI {

namespace {
    const char* coloredRectVertexShader = R"(
        #version 330 core
        layout (location = 0) in vec4 vertex;
        
        uniform mat4 model;
        uniform mat4 projection;
        
        void main() {
            gl_Position = projection * model * vec4(vertex.xy, 0.0, 1.0);
        }
    )";

    const char* coloredRectFragmentShader = R"(
        #version 330 core
        out vec4 FragColor;
        
        uniform vec4 color;
        
        void main() {
            FragColor = color;
        }
    )";

    const char* textureVertexShader = R"(
        #version 330 core
        layout (location = 0) in vec4 vertex;
        
        out vec2 TexCoords;
        
        uniform mat4 model;
        uniform mat4 projection;
        
        void main() {
            gl_Position = projection * model * vec4(vertex.xy, 0.0, 1.0);
            TexCoords = vertex.zw;
        }
    )";

    const char* textureFragmentShader = R"(
        #version 330 core
        in vec2 TexCoords;
        out vec4 FragColor;
        
        uniform sampler2D texture0;
        uniform vec4 color;
        
        void main() {
            FragColor = texture(texture0, TexCoords) * color;
        }
    )";
}

ShaderSources& ShaderSources::Get() {
    static ShaderSources instance;
    return instance;
}

ShaderSources::ShaderSources() {
    InitializeBuiltInShaders();
}

void ShaderSources::InitializeBuiltInShaders() {
    // Colored Rectangle Shader
    m_BuiltInShaders[Type::UI_ColoredRect] = {
        coloredRectVertexShader,
        coloredRectFragmentShader
    };

    // Texture Shader
    m_BuiltInShaders[Type::UI_Texture] = {
        textureVertexShader,
        textureFragmentShader
    };
}

const ShaderSources::ShaderSource& ShaderSources::GetShaderSource(Type type) const {
    auto it = m_BuiltInShaders.find(type);
    if (it != m_BuiltInShaders.end()) {
        return it->second;
    }
    static ShaderSource emptySource;
    return emptySource;
}

void ShaderSources::RegisterCustomShader(const std::string& name, const ShaderSource& source) {
    m_CustomShaders[name] = source;
}

const ShaderSources::ShaderSource& ShaderSources::GetCustomShaderSource(const std::string& name) const {
    auto it = m_CustomShaders.find(name);
    if (it != m_CustomShaders.end()) {
        return it->second;
    }
    static ShaderSource emptySource;
    return emptySource;
}

} // namespace UI
} // namespace VGE 