#pragma once

#include "Runtime/Core/Public/Core/UICore.h"
#include <glm/glm.hpp>
#include <memory>

namespace VGE {
namespace UI {

// Forward declarations
class UIWidget;

// Alignment enums
enum class HAlign {
    Left,
    Center,
    Right,
    Fill
};

enum class VAlign {
    Top,
    Center,
    Bottom,
    Fill
};

class UI_API UISlot {
public:
    UISlot();
    virtual ~UISlot() = default;

    // Widget management
    void SetWidget(std::shared_ptr<UIWidget> widget);
    std::shared_ptr<UIWidget> GetWidget() const { return m_Widget; }

    // Layout properties
    UISlot& Padding(float left, float top, float right, float bottom);
    UISlot& SetHAlign(HAlign align);
    UISlot& SetVAlign(VAlign align);
    UISlot& Fill(float horizontal = 1.0f, float vertical = 1.0f);
    UISlot& MaxSize(float width = 0.0f, float height = 0.0f);

    // Getters
    const glm::vec4& GetPadding() const { return m_Padding; }
    HAlign GetHAlign() const { return m_HAlign; }
    VAlign GetVAlign() const { return m_VAlign; }
    const glm::vec2& GetFill() const { return m_Fill; }
    const glm::vec2& GetMaxSize() const { return m_MaxSize; }

protected:
    std::shared_ptr<UIWidget> m_Widget;
    glm::vec4 m_Padding;
    glm::vec2 m_Fill;
    glm::vec2 m_MaxSize;
    HAlign m_HAlign;
    VAlign m_VAlign;
};

}} // namespace VGE::UI
