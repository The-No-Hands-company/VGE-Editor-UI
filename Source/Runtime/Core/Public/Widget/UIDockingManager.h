#pragma once

#include "Core/UICore.h"
#include "Widget/UIWidget.h"
#include "Widget/UIWindow.h"
#include <memory>
#include <vector>
#include <unordered_map>

namespace VGE {
namespace UI {

enum class DockPosition {
    None,
    Left,
    Right,
    Top,
    Bottom,
    Center
};

struct DockPreview {
    DockPosition position;
    glm::vec2 position2D;
    glm::vec2 size;
    bool visible;

    DockPreview()
        : position(DockPosition::None)
        , position2D(0.0f)
        , size(0.0f)
        , visible(false)
    {}
};

class UI_API UIDockingManager : public UIWidget {
public:
    UIDockingManager();
    virtual ~UIDockingManager();

    void AddDockableWindow(std::shared_ptr<UIWindow> window);
    void RemoveDockableWindow(std::shared_ptr<UIWindow> window);
    void DockWindow(std::shared_ptr<UIWindow> window, std::shared_ptr<UIWindow> target, DockPosition position);
    void UndockWindow(std::shared_ptr<UIWindow> window);
    void Clear();

    virtual void OnRender() override;
    virtual void OnUpdate(float deltaTime) override;
    virtual bool OnMouseMove(const glm::vec2& position) override;
    virtual bool OnMouseButton(int button, bool pressed, const glm::vec2& position) override;

    const std::vector<std::shared_ptr<UIWindow>>& GetDockableWindows() const { return m_DockableWindows; }
    DockPosition GetDockPreview(std::shared_ptr<UIWindow> window, const glm::vec2& position) const;

private:
    struct DockNode {
        std::shared_ptr<UIWindow> window;
        DockPosition position;
        glm::vec2 size;
        float splitRatio;
        std::vector<DockNode> children;

        DockNode()
            : position(DockPosition::None)
            , size(0.0f)
            , splitRatio(0.5f)
        {}
    };

    void UpdateDockPreview(const glm::vec2& position);
    void UpdateLayout();
    void RenderDockPreview();
    DockNode* FindDockTarget(const glm::vec2& position);
    void ArrangeDockNodes(DockNode& node, const glm::vec2& position, const glm::vec2& size);

private:
    std::vector<std::shared_ptr<UIWindow>> m_DockableWindows;
    DockNode m_RootNode;
    std::shared_ptr<UIWindow> m_DraggedWindow;
    DockPreview m_Preview;
    bool m_IsDragging;
    float m_DockPreviewAlpha;
    float m_MinDockSize;
    float m_DockPadding;
};

}} // namespace VGE::UI 