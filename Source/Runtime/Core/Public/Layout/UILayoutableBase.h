#pragma once

#include "Runtime/Core/Public/Core/UICore.h"

#include "IUILayoutable.h"
#include "Runtime/Core/Public/Core/UICore.h"
#include "Runtime/Core/Public/Core/UITypes.h"
#include <glm/glm.hpp>

namespace VGE {
namespace UI {

class UI_API UILayoutableBase : public IUILayoutable {
public:
    virtual ~UILayoutableBase() = default;

    // IUILayoutable implementation
    virtual void SetPosition(const glm::vec2& position) override {
        m_Position = position;
        OnPositionChanged();
    }

    virtual void SetSize(const glm::vec2& size) override {
        m_Size = size;
        OnSizeChanged();
    }

    virtual const glm::vec2& GetPosition() const override {
        return m_Position;
    }

    virtual const glm::vec2& GetSize() const override {
        return m_Size;
    }

    virtual Rect GetRect() const override {
        return Rect(m_Position.x, m_Position.y, m_Size.x, m_Size.y);
    }

    virtual void UpdateLayout() override {
        OnLayoutUpdate();
    }

protected:
    virtual void OnPositionChanged() {}
    virtual void OnSizeChanged() {}
    virtual void OnLayoutUpdate() {}

private:
    glm::vec2 m_Position{0.0f, 0.0f};
    glm::vec2 m_Size{0.0f, 0.0f};
};

}} // namespace VGE::UI



