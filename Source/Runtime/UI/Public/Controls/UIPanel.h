#pragma once

#include "Runtime/Core/Public/UIWidget.h"
#include "Runtime/Core/Public/Core/UITypes.h"
#include "Runtime/Core/Public/Core/UICore.h"
#include <string>
#include <vector>
#include <memory>

namespace VGE {
namespace UI {

class UI_API UIPanel : public UIWidget {
public:
    UIPanel(const std::string& name = "Panel");
    virtual ~UIPanel() = default;

protected:
    // UIElement interface implementation
    virtual void OnDraw(UIRenderer& renderer) override;
    virtual void OnUpdate(float deltaTime) override;
    virtual void OnPositionChanged() override;
    virtual void OnSizeChanged() override;
    virtual void OnChildAdded(std::shared_ptr<UIElement> child) override;
    virtual void OnChildRemoved(std::shared_ptr<UIElement> child) override;
    virtual void OnParentPositionChanged() override;

public:
    // Panel specific functionality
    void SetCollapsed(bool collapsed);
    bool IsCollapsed() const { return m_Collapsed; }
    void SetResizable(bool resizable) { m_Style.showResizeHandle = resizable; }
    bool IsResizable() const { return m_Style.showResizeHandle; }
    void SetHasTitleBar(bool hasTitleBar) { m_Style.showTitleBar = hasTitleBar; }
    bool HasTitleBar() const { return m_Style.showTitleBar; }
    void SetDockable(bool dockable) { m_Dockable = dockable; }
    bool IsDockable() const { return m_Dockable; }

    // Style
    void SetStyle(const Style& style) { m_Style = style; }
    const Style& GetStyle() const { return m_Style; }

private:
    void RenderTitleBar(UIRenderer& renderer);
    void RenderContent(UIRenderer& renderer);
    bool IsMouseOverRect(const glm::vec2& pos, const glm::vec2& size) const;

    Style m_Style;
    bool m_Dockable;
    bool m_Collapsed;
    bool m_IsDragging;
    glm::vec2 m_PanelPadding;
    glm::vec2 m_DragOffset;
    glm::vec2 m_ExpandedSize;
};

} // namespace UI
} // namespace VGE


