#pragma once

#include "Runtime/Core/Public/Core/UICore.h"
#include "Runtime/Core/Public/Renderer/UIShader.h"
#include <string>
#include <memory>
#include <unordered_map>

namespace VGE {
namespace UI {

class UI_API UIShaderManager {
public:
    static UIShaderManager& Get();
    static void Create();
    static void Destroy();

    // Shader management
    std::shared_ptr<UIShader> GetShader(const std::string& name);
    std::shared_ptr<UIShader> CreateShader(const std::string& name);
    std::shared_ptr<UIShader> CreateShader(const std::string& name, const std::string& vertexSource, const std::string& fragmentSource);
    void RemoveShader(const std::string& name);
    void ClearShaders();

    // Built-in shaders
    std::shared_ptr<UIShader> GetDefaultShader();
    std::shared_ptr<UIShader> GetTextShader();
    std::shared_ptr<UIShader> GetColorShader();

    // Destructor
    ~UIShaderManager();

private:
    UIShaderManager();

    UIShaderManager(const UIShaderManager&) = delete;
    UIShaderManager& operator=(const UIShaderManager&) = delete;

    void InitializeBuiltInShaders();
    std::string GetBuiltInShaderSource(const std::string& name, bool isVertex);

private:
    static UIShaderManager* s_Instance;

    std::unordered_map<std::string, std::shared_ptr<UIShader>> m_Shaders;
    std::shared_ptr<UIShader> m_DefaultShader;
    std::shared_ptr<UIShader> m_TextShader;
    std::shared_ptr<UIShader> m_ColorShader;
    bool m_Initialized = false;
};

}} // namespace VGE::UI 


