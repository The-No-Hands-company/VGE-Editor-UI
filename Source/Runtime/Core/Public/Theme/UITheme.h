#pragma once

#include "Runtime/Core/Public/Core/UICore.h"

#include "Runtime/Core/Public/Core/UITypes.h"
#include "Runtime/Core/Public/Core/UICore.h"

namespace VGE {
namespace UI {

class UI_API UITheme {
public:
    UITheme() = default;
    virtual ~UITheme() = default;

    // Colors
    const ThemeColors& GetColors() const { return m_Colors; }
    void SetColors(const ThemeColors& colors) { m_Colors = colors; }

    // Style
    const ThemeStyle& GetStyle() const { return m_Style; }
    void SetStyle(const ThemeStyle& style) { m_Style = style; }

    // Presets
    static UITheme CreateDarkTheme();
    static UITheme CreateLightTheme();

protected:
    ThemeColors m_Colors;
    ThemeStyle m_Style;
};

}} // namespace VGE::UI



