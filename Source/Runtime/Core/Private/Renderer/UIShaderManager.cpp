#include "Runtime/Core/Public/Renderer/UIShaderManager.h"

namespace VGE {
namespace UI {

// Static member initialization
UIShaderManager* UIShaderManager::s_Instance = nullptr;

void UIShaderManager::Create() {
    if (!s_Instance) {
        s_Instance = new UIShaderManager();
    }
}

void UIShaderManager::Destroy() {
    if (s_Instance) {
        delete s_Instance;
        s_Instance = nullptr;
    }
}

UIShaderManager& UIShaderManager::Get() {
    if (!s_Instance) {
        Create();
    }
    return *s_Instance;
}

UIShaderManager::UIShaderManager() {
    InitializeBuiltInShaders();
}

UIShaderManager::~UIShaderManager() {
    ClearShaders();
}

std::shared_ptr<UIShader> UIShaderManager::GetShader(const std::string& name) {
    auto it = m_Shaders.find(name);
    if (it != m_Shaders.end()) {
        return it->second;
    }
    return nullptr;
}

std::shared_ptr<UIShader> UIShaderManager::CreateShader(const std::string& name) {
    auto shader = std::make_shared<UIShader>();
    m_Shaders[name] = shader;
    return shader;
}

std::shared_ptr<UIShader> UIShaderManager::CreateShader(const std::string& name, const std::string& vertexSource, const std::string& fragmentSource) {
    auto shader = std::make_shared<UIShader>();
    if (shader->Initialize(vertexSource, fragmentSource)) {
        m_Shaders[name] = shader;
        return shader;
    }
    return nullptr;
}

void UIShaderManager::RemoveShader(const std::string& name) {
    auto it = m_Shaders.find(name);
    if (it != m_Shaders.end()) {
        m_Shaders.erase(it);
    }
}

void UIShaderManager::ClearShaders() {
    m_Shaders.clear();
    m_DefaultShader.reset();
    m_TextShader.reset();
    m_ColorShader.reset();
}

std::shared_ptr<UIShader> UIShaderManager::GetDefaultShader() {
    if (!m_DefaultShader) {
        m_DefaultShader = CreateShader("Default", GetBuiltInShaderSource("Default", true), GetBuiltInShaderSource("Default", false));
    }
    return m_DefaultShader;
}

std::shared_ptr<UIShader> UIShaderManager::GetTextShader() {
    if (!m_TextShader) {
        m_TextShader = CreateShader("Text", GetBuiltInShaderSource("Text", true), GetBuiltInShaderSource("Text", false));
    }
    return m_TextShader;
}

std::shared_ptr<UIShader> UIShaderManager::GetColorShader() {
    if (!m_ColorShader) {
        m_ColorShader = CreateShader("Color", GetBuiltInShaderSource("Color", true), GetBuiltInShaderSource("Color", false));
    }
    return m_ColorShader;
}

void UIShaderManager::InitializeBuiltInShaders() {
    if (!m_Initialized) {
        GetDefaultShader();
        GetTextShader();
        GetColorShader();
        m_Initialized = true;
    }
}

std::string UIShaderManager::GetBuiltInShaderSource(const std::string& name, bool isVertex) {
    if (name == "Default") {
        if (isVertex) {
            return R"(
                #version 330 core
                layout(location = 0) in vec3 a_Position;
                layout(location = 1) in vec4 a_Color;
                
                uniform mat4 u_ViewProjection;
                uniform mat4 u_Transform;
                
                out vec4 v_Color;
                
                void main() {
                    v_Color = a_Color;
                    gl_Position = u_ViewProjection * u_Transform * vec4(a_Position, 1.0);
                }
            )";
        } else {
            return R"(
                #version 330 core
                layout(location = 0) out vec4 color;
                
                in vec4 v_Color;
                
                void main() {
                    color = v_Color;
                }
            )";
        }
    } else if (name == "Text") {
        if (isVertex) {
            return R"(
                #version 330 core
                layout(location = 0) in vec3 a_Position;
                layout(location = 1) in vec2 a_TexCoord;
                
                uniform mat4 u_ViewProjection;
                uniform mat4 u_Transform;
                
                out vec2 v_TexCoord;
                
                void main() {
                    v_TexCoord = a_TexCoord;
                    gl_Position = u_ViewProjection * u_Transform * vec4(a_Position, 1.0);
                }
            )";
        } else {
            return R"(
                #version 330 core
                layout(location = 0) out vec4 color;
                
                in vec2 v_TexCoord;
                
                uniform sampler2D u_Texture;
                uniform vec4 u_Color;
                
                void main() {
                    color = texture(u_Texture, v_TexCoord) * u_Color;
                }
            )";
        }
    } else if (name == "Color") {
        if (isVertex) {
            return R"(
                #version 330 core
                layout(location = 0) in vec3 a_Position;
                
                uniform mat4 u_ViewProjection;
                uniform mat4 u_Transform;
                
                void main() {
                    gl_Position = u_ViewProjection * u_Transform * vec4(a_Position, 1.0);
                }
            )";
        } else {
            return R"(
                #version 330 core
                layout(location = 0) out vec4 color;
                
                uniform vec4 u_Color;
                
                void main() {
                    color = u_Color;
                }
            )";
        }
    }
    return "";
}

}} // namespace VGE::UI


