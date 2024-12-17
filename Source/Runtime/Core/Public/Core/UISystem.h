#pragma once

#include "Runtime/Core/Public/Core/UICore.h"

#include "Runtime/Core/Public/Math/Rect.h"
#include "Graphics/SpriteBatch.h"
#include "Graphics/Texture.h"
#include "Runtime/Core/Public/Base/VGE.h"
#include <memory>
#include <vector>
#include <functional>

namespace VGE {
namespace UI {

enum class Anchor {
    TopLeft,
    TopCenter,
    TopRight,
    MiddleLeft,
    MiddleCenter,
    MiddleRight,
    BottomLeft,
    BottomCenter,
    BottomRight
};

enum class LayoutType {
    None,
    Vertical,
    Horizontal,
    Grid
};

enum class GridCellAlignment {
    Fill,
    Center,
    TopLeft,
    TopCenter,
    TopRight,
    MiddleLeft,
    MiddleRight,
    BottomLeft,
    BottomCenter,
    BottomRight
};

struct UIStyle {
    glm::vec4 BackgroundColor = glm::vec4(0.2f, 0.2f, 0.2f, 1.0f);
    glm::vec4 TextColor = glm::vec4(1.0f);
    glm::vec4 BorderColor = glm::vec4(0.3f, 0.3f, 0.3f, 1.0f);
    float BorderThickness = 1.0f;
    float Padding = 5.0f;
    Graphics::TexturePtr BackgroundTexture;
    Graphics::TexturePtr BorderTexture;
};

class UIElement;
using UIElementPtr = std::shared_ptr<UIElement>;

class UIElement : public std::enable_shared_from_this<UIElement> {
public:
    UIElement();
    virtual ~UIElement() = default;

    virtual void Update(float deltaTime);
    virtual void Render(Graphics::SpriteBatch& spriteBatch);
    virtual void CalculateLayout(const Math::Rect& parentBounds);

    // Child management
    void AddChild(const UIElementPtr& child);
    void RemoveChild(const UIElementPtr& child);
    void ClearChildren();

    // Getters/Setters
    void SetPosition(const glm::vec2& position);
    void SetSize(const glm::vec2& size);
    void SetAnchor(Anchor anchor) { m_Anchor = anchor; }
    void SetLayoutType(LayoutType type) { m_LayoutType = type; }
    void SetSpacing(float spacing) { m_Spacing = spacing; }
    void SetStyle(const UIStyle& style) { m_Style = style; }
    void SetVisible(bool visible) { m_Visible = visible; }
    void SetEnabled(bool enabled) { m_Enabled = enabled; }

    // Grid layout properties
    void SetGridColumns(size_t columns) { m_GridColumns = columns; }
    void SetGridCellSize(const glm::vec2& size) { m_GridCellSize = size; }
    void SetGridCellAlignment(GridCellAlignment alignment) { m_GridCellAlignment = alignment; }

    const glm::vec2& GetPosition() const { return m_Position; }
    const glm::vec2& GetSize() const { return m_Size; }
    Anchor GetAnchor() const { return m_Anchor; }
    LayoutType GetLayoutType() const { return m_LayoutType; }
    float GetSpacing() const { return m_Spacing; }
    const UIStyle& GetStyle() const { return m_Style; }
    bool IsVisible() const { return m_Visible; }
    bool IsEnabled() const { return m_Enabled; }
    bool IsHovered() const { return m_Hovered; }
    bool IsPressed() const { return m_Pressed; }

    // Grid layout getters
    size_t GetGridColumns() const { return m_GridColumns; }
    const glm::vec2& GetGridCellSize() const { return m_GridCellSize; }
    GridCellAlignment GetGridCellAlignment() const { return m_GridCellAlignment; }

    // Event callbacks
    using EventCallback = std::function<void()>;
    void SetOnHover(EventCallback callback) { m_OnHover = callback; }
    void SetOnClick(EventCallback callback) { m_OnClick = callback; }

protected:
    virtual void UpdateSelf(float deltaTime);
    virtual void UpdateChildren(float deltaTime);
    virtual void RenderSelf(Graphics::SpriteBatch& spriteBatch);
    virtual void RenderChildren(Graphics::SpriteBatch& spriteBatch);
    virtual void CalculateLayoutSelf(const Math::Rect& parentBounds);
    virtual void CalculateChildrenLayout();
    Math::Rect CalculateAnchoredPosition(const Math::Rect& parentBounds) const;

    UIElement* m_Parent;
    std::vector<UIElementPtr> m_Children;
    glm::vec2 m_Position;
    glm::vec2 m_Size;
    Math::Rect m_Bounds;
    Anchor m_Anchor;
    LayoutType m_LayoutType;
    float m_Spacing;
    UIStyle m_Style;
    bool m_Visible;
    bool m_Enabled;
    bool m_Hovered;
    bool m_Pressed;

    // Grid layout properties
    size_t m_GridColumns;
    glm::vec2 m_GridCellSize;
    GridCellAlignment m_GridCellAlignment;

    // Event callbacks
    EventCallback m_OnHover;
    EventCallback m_OnClick;
};

class Panel : public UIElement {
protected:
    void RenderSelf(Graphics::SpriteBatch& spriteBatch) override;
};

class Label : public UIElement {
public:
    Label();

    void SetText(const std::string& text);
    void SetTextAlignment(Anchor alignment) { m_TextAlignment = alignment; }
    const std::string& GetText() const { return m_Text; }
    Anchor GetTextAlignment() const { return m_TextAlignment; }

protected:
    void RenderSelf(Graphics::SpriteBatch& spriteBatch) override;

private:
    std::string m_Text;
    Anchor m_TextAlignment;
};

class UISystem {
public:
    static void Initialize();
    static void Shutdown();
    static void Update(float deltaTime);
    static void Render();

    static UIElementPtr GetRoot() { return s_Root; }
    static const UIStyle& GetDefaultStyle() { return s_DefaultStyle; }

private:
    static UIElementPtr s_Root;
    static UIStyle s_DefaultStyle;
    static Graphics::SpriteBatch s_SpriteBatch;
};

} // namespace UI
} // namespace VGE



