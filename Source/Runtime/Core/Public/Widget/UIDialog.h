#pragma once

#include "Runtime/Core/Public/Core/UICore.h"
#include "Runtime/Core/Public/Widget/UIWidget.h"
#include "Runtime/Core/Public/Core/UITypes.h"
#include <string>

namespace VGE {
namespace Editor {

class UIDialog : public UIWidget {
public:
    UIDialog(const std::string& title = "Dialog");
    virtual ~UIDialog() override = default;

    // Core functionality
    virtual void OnUpdate(float deltaTime) override;
    virtual void OnPaint(UIRenderer& renderer) override;

protected:
    // UIElement interface implementation
    virtual void OnPositionChanged() override;
    virtual void OnSizeChanged() override;
    virtual void OnChildAdded(std::shared_ptr<UIElement> child) override;
    virtual void OnChildRemoved(std::shared_ptr<UIElement> child) override;
    virtual void OnParentPositionChanged() override;

    // Mouse event handlers
    virtual bool OnMouseMove(const glm::vec2& mousePos) override;
    virtual bool OnMouseDown(const glm::vec2& mousePos) override;
    virtual bool OnMouseUp(const glm::vec2& mousePos) override;
    virtual void OnMouseEnter() override;
    virtual void OnMouseLeave() override;
    virtual void OnPressed() override;
    virtual void OnReleased() override;

public:
    // Dialog specific functionality
    void SetTitle(const std::string& title) { m_Title = title; }
    const std::string& GetTitle() const { return m_Title; }
    void SetDraggable(bool draggable) { m_Draggable = draggable; }
    bool IsDraggable() const { return m_Draggable; }
    void SetResizable(bool resizable) { m_Resizable = resizable; }
    bool IsResizable() const { return m_Resizable; }
    void SetModal(bool modal) { m_Modal = modal; }
    bool IsModal() const { return m_Modal; }

protected:
    // Helper functions
    void RenderTitleBar(UIRenderer& renderer);
    void RenderContent(UIRenderer& renderer);
    void RenderResizeHandle(UIRenderer& renderer);
    void CenterInParent();

    // Dialog properties
    std::string m_Title;
    bool m_Draggable = true;
    bool m_Resizable = true;
    bool m_Modal = false;
    bool m_IsDragging = false;
    bool m_IsResizing = false;
    glm::vec2 m_DragOffset{0.0f, 0.0f};
    float m_TitleBarHeight = 25.0f;
    float m_ResizeHandleSize = 10.0f;
};

}} // namespace VGE::Editor


