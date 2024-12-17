#include "Runtime/UI/Public/Controls/UICommandPalette.h"
#include "Runtime/Core/Public/Core/UICore.h"
#include "Runtime/Core/Public/Event/UIEventManager.h"

namespace VGE::UI {

UICommandPalette::UICommandPalette()
    : Widget("Command Palette")
{
    SetSize(glm::vec2(400.0f, 300.0f));
    SetVisible(false);
}

void UICommandPalette::AddCommand(const std::string& name, const std::string& description, CommandCallback callback) {
    Command cmd;
    cmd.Name = name;
    cmd.Description = description;
    cmd.Callback = callback;
    m_Commands.push_back(cmd);
}

void UICommandPalette::OnDraw() {
    // Draw command palette background
    auto renderer = GetRenderer();
    if (!renderer) return;

    const glm::vec4 bgColor(0.2f, 0.2f, 0.2f, 0.9f);
    const glm::vec4 textColor(1.0f, 1.0f, 1.0f, 1.0f);
    const glm::vec4 selectedColor(0.3f, 0.3f, 0.3f, 1.0f);

    // Draw background
    renderer->DrawRect(GetPosition(), GetSize(), bgColor);

    // Draw search box
    const float searchBoxHeight = 30.0f;
    const glm::vec2 searchBoxPos = GetPosition() + glm::vec2(5.0f, 5.0f);
    const glm::vec2 searchBoxSize(GetSize().x - 10.0f, searchBoxHeight);
    renderer->DrawRect(searchBoxPos, searchBoxSize, glm::vec4(0.3f, 0.3f, 0.3f, 1.0f));
    renderer->DrawText(searchBoxPos + glm::vec2(5.0f, 5.0f), m_SearchText, textColor);

    // Draw commands
    const float commandHeight = 25.0f;
    glm::vec2 commandPos = GetPosition() + glm::vec2(5.0f, searchBoxHeight + 10.0f);
    
    for (size_t i = 0; i < m_Commands.size(); ++i) {
        const auto& cmd = m_Commands[i];
        if (i == m_SelectedIndex) {
            renderer->DrawRect(commandPos, glm::vec2(GetSize().x - 10.0f, commandHeight), selectedColor);
        }
        renderer->DrawText(commandPos + glm::vec2(5.0f, 5.0f), cmd.Name, textColor);
        commandPos.y += commandHeight;
    }
}

void UICommandPalette::HandleInput(const Event& event) {
    if (event.Type == EventType::KeyPress) {
        if (event.KeyCode == GLFW_KEY_UP) {
            if (m_SelectedIndex > 0) {
                m_SelectedIndex--;
            }
        }
        else if (event.KeyCode == GLFW_KEY_DOWN) {
            if (m_SelectedIndex < m_Commands.size() - 1) {
                m_SelectedIndex++;
            }
        }
        else if (event.KeyCode == GLFW_KEY_ENTER) {
            if (m_SelectedIndex < m_Commands.size()) {
                m_Commands[m_SelectedIndex].Callback();
                SetVisible(false);
            }
        }
        else if (event.KeyCode == GLFW_KEY_ESCAPE) {
            SetVisible(false);
        }
    }
}

} // namespace VGE::UI



