#pragma once

#include "Core/UICore.h"
#include <glm/glm.hpp>
#include <string>
#include <memory>

namespace VGE {
namespace UI {

struct Style {
    glm::vec4 backgroundColor;
    glm::vec4 borderColor;
    glm::vec4 textColor;
    glm::vec4 titleBarColor;
    glm::vec4 titleBarTextColor;
    glm::vec4 hoverColor;
    glm::vec4 activeColor;
    glm::vec4 disabledColor;

    float cornerRadius;
    float borderThickness;
    float padding;
    float titleBarHeight;
    float minWidth;
    float minHeight;
    float resizeHandleSize;
    float borderWidth;

    Style()
        : backgroundColor(0.2f, 0.2f, 0.2f, 1.0f)
        , borderColor(0.4f, 0.4f, 0.4f, 1.0f)
        , textColor(1.0f, 1.0f, 1.0f, 1.0f)
        , titleBarColor(0.3f, 0.3f, 0.3f, 1.0f)
        , titleBarTextColor(1.0f, 1.0f, 1.0f, 1.0f)
        , hoverColor(0.4f, 0.4f, 0.4f, 1.0f)
        , activeColor(0.5f, 0.5f, 0.5f, 1.0f)
        , disabledColor(0.3f, 0.3f, 0.3f, 0.5f)
        , cornerRadius(4.0f)
        , borderThickness(1.0f)
        , padding(8.0f)
        , titleBarHeight(24.0f)
        , minWidth(100.0f)
        , minHeight(50.0f)
        , resizeHandleSize(8.0f)
        , borderWidth(1.0f)
    {}
};

struct ThemeColors {
    glm::vec4 primary;
    glm::vec4 secondary;
    glm::vec4 accent;
    glm::vec4 background;
    glm::vec4 text;
    glm::vec4 border;

    ThemeColors()
        : primary(0.2f, 0.6f, 1.0f, 1.0f)
        , secondary(0.4f, 0.4f, 0.4f, 1.0f)
        , accent(1.0f, 0.5f, 0.0f, 1.0f)
        , background(0.2f, 0.2f, 0.2f, 1.0f)
        , text(1.0f, 1.0f, 1.0f, 1.0f)
        , border(0.4f, 0.4f, 0.4f, 1.0f)
    {}
};

struct ThemeStyle {
    float spacing;
    float padding;
    float borderRadius;
    float borderWidth;
    float fontSize;
    float iconSize;

    ThemeStyle()
        : spacing(8.0f)
        , padding(8.0f)
        , borderRadius(4.0f)
        , borderWidth(1.0f)
        , fontSize(14.0f)
        , iconSize(16.0f)
    {}
};

class UI_API UIStyle {
public:
    UIStyle();
    virtual ~UIStyle();

    // Colors
    void SetBackgroundColor(const glm::vec4& color) { m_Style.backgroundColor = color; }
    void SetBorderColor(const glm::vec4& color) { m_Style.borderColor = color; }
    void SetTextColor(const glm::vec4& color) { m_Style.textColor = color; }
    void SetTitleBarColor(const glm::vec4& color) { m_Style.titleBarColor = color; }
    void SetTitleBarTextColor(const glm::vec4& color) { m_Style.titleBarTextColor = color; }
    void SetHoverColor(const glm::vec4& color) { m_Style.hoverColor = color; }
    void SetActiveColor(const glm::vec4& color) { m_Style.activeColor = color; }
    void SetDisabledColor(const glm::vec4& color) { m_Style.disabledColor = color; }

    const glm::vec4& GetBackgroundColor() const { return m_Style.backgroundColor; }
    const glm::vec4& GetBorderColor() const { return m_Style.borderColor; }
    const glm::vec4& GetTextColor() const { return m_Style.textColor; }
    const glm::vec4& GetTitleBarColor() const { return m_Style.titleBarColor; }
    const glm::vec4& GetTitleBarTextColor() const { return m_Style.titleBarTextColor; }
    const glm::vec4& GetHoverColor() const { return m_Style.hoverColor; }
    const glm::vec4& GetActiveColor() const { return m_Style.activeColor; }
    const glm::vec4& GetDisabledColor() const { return m_Style.disabledColor; }

    // Dimensions
    void SetCornerRadius(float radius) { m_Style.cornerRadius = radius; }
    void SetBorderThickness(float thickness) { m_Style.borderThickness = thickness; }
    void SetPadding(float padding) { m_Style.padding = padding; }
    void SetTitleBarHeight(float height) { m_Style.titleBarHeight = height; }
    void SetMinWidth(float width) { m_Style.minWidth = width; }
    void SetMinHeight(float height) { m_Style.minHeight = height; }
    void SetResizeHandleSize(float size) { m_Style.resizeHandleSize = size; }
    void SetBorderWidth(float width) { m_Style.borderWidth = width; }

    float GetCornerRadius() const { return m_Style.cornerRadius; }
    float GetBorderThickness() const { return m_Style.borderThickness; }
    float GetPadding() const { return m_Style.padding; }
    float GetTitleBarHeight() const { return m_Style.titleBarHeight; }
    float GetMinWidth() const { return m_Style.minWidth; }
    float GetMinHeight() const { return m_Style.minHeight; }
    float GetResizeHandleSize() const { return m_Style.resizeHandleSize; }
    float GetBorderWidth() const { return m_Style.borderWidth; }

    // Theme
    void SetTheme(const ThemeColors& colors, const ThemeStyle& style);
    const ThemeColors& GetThemeColors() const { return m_ThemeColors; }
    const ThemeStyle& GetThemeStyle() const { return m_ThemeStyle; }

private:
    Style m_Style;
    ThemeColors m_ThemeColors;
    ThemeStyle m_ThemeStyle;
};

}} // namespace VGE::UI 