#pragma once

// Engine includes
#include "Runtime/Core/Public/Core/UICore.h"
#include "Runtime/Core/Public/Widget/UIWidget.h"

// Standard library includes
#include <functional>
#include <vector>
#include <string>
#include <glm/glm.hpp>

namespace VGE {
namespace UI {

enum class ColorSpace {
    RGB,    // Red, Green, Blue
    HSV,    // Hue, Saturation, Value
    HSL,    // Hue, Saturation, Lightness
    CMYK,   // Cyan, Magenta, Yellow, Key (Black)
    LAB,    // CIELAB color space
    XYZ     // CIE XYZ color space
};

struct ColorStop {
    float Position;    // 0-1 range
    glm::vec4 Color;   // RGBA
};

struct ColorPalette {
    std::string Name;
    std::vector<glm::vec4> Colors;
    bool IsEditable = false;
};

class UI_API UIColorPicker : public UIWidget {
public:
    UIColorPicker(const std::string& label = "");
    virtual ~UIColorPicker() = default;

    virtual void Update() override;
    virtual void Draw() override;
    virtual bool HandleEvent(Event& event) override;

    // Color management
    void SetColor(const glm::vec4& color);
    void SetColor(const glm::vec3& color, float alpha = 1.0f);
    const glm::vec4& GetColor() const { return m_Color; }
    void SetAlpha(float alpha);
    float GetAlpha() const { return m_Color.a; }

    // Color space
    void SetColorSpace(ColorSpace space);
    ColorSpace GetColorSpace() const { return m_ColorSpace; }
    void EnableColorSpace(ColorSpace space, bool enabled);
    bool IsColorSpaceEnabled(ColorSpace space) const;

    // Palette management
    void AddPalette(const ColorPalette& palette);
    void RemovePalette(const std::string& name);
    void SetActivePalette(const std::string& name);
    void AddColorToPalette(const std::string& paletteName, const glm::vec4& color);
    void ClearPalette(const std::string& name);
    const std::vector<ColorPalette>& GetPalettes() const { return m_Palettes; }

    // Gradient editor
    void SetGradientStops(const std::vector<ColorStop>& stops);
    const std::vector<ColorStop>& GetGradientStops() const { return m_GradientStops; }
    void AddGradientStop(float position, const glm::vec4& color);
    void RemoveGradientStop(size_t index);
    void EnableGradientEditor(bool enable) { m_GradientEditorEnabled = enable; }

    // Visual customization
    void SetShowColorPreview(bool show) { m_ShowColorPreview = show; }
    void SetShowColorValues(bool show) { m_ShowColorValues = show; }
    void SetShowAlphaChannel(bool show) { m_ShowAlphaChannel = show; }
    void SetShowPaletteBar(bool show) { m_ShowPaletteBar = show; }
    void SetCompactMode(bool compact) { m_CompactMode = compact; }
    void SetPreviewShape(bool circular) { m_CircularPreview = circular; }

    // Color harmony
    void EnableColorHarmony(bool enable) { m_ColorHarmonyEnabled = enable; }
    void SetHarmonyRule(const std::string& rule) { m_HarmonyRule = rule; }
    const std::vector<glm::vec4>& GetHarmonyColors() const { return m_HarmonyColors; }

    // Color blindness simulation
    void EnableColorBlindnessSimulation(bool enable) { m_ColorBlindSimulationEnabled = enable; }
    void SetColorBlindnessType(const std::string& type) { m_ColorBlindnessType = type; }

    // Event callbacks
    void SetColorChangedCallback(std::function<void(const glm::vec4&)> callback) {
        m_ColorChangedCallback = callback;
    }
    void SetGradientChangedCallback(std::function<void(const std::vector<ColorStop>&)> callback) {
        m_GradientChangedCallback = callback;
    }
    void SetPaletteChangedCallback(std::function<void(const std::string&)> callback) {
        m_PaletteChangedCallback = callback;
    }

private:
    // Rendering methods
    void RenderColorWheel();
    void RenderColorSliders();
    void RenderAlphaSlider();
    void RenderColorPreview();
    void RenderColorValues();
    void RenderPaletteBar();
    void RenderGradientEditor();
    void RenderHarmonyPreview();
    void RenderColorBlindnessPreview();

    // Input handling
    bool HandleColorWheelInput(Event& event);
    bool HandleSliderInput(Event& event);
    bool HandlePaletteInput(Event& event);
    bool HandleGradientInput(Event& event);

    // Color conversion
    glm::vec4 ConvertColor(const glm::vec4& color, ColorSpace from, ColorSpace to) const;
    glm::vec4 GetColorFromWheel(const glm::vec2& position) const;
    glm::vec2 GetWheelPositionFromColor(const glm::vec4& color) const;

    // Utility methods
    void UpdateHarmonyColors();
    void UpdateColorBlindnessPreview();
    void NotifyColorChanged();
    float GetSliderValue(int component) const;
    void SetSliderValue(int component, float value);

    // Core properties
    glm::vec4 m_Color = { 1.0f, 1.0f, 1.0f, 1.0f };
    ColorSpace m_ColorSpace = ColorSpace::RGB;
    std::vector<ColorSpace> m_EnabledColorSpaces;
    
    // Visual state
    bool m_ShowColorPreview = true;
    bool m_ShowColorValues = true;
    bool m_ShowAlphaChannel = true;
    bool m_ShowPaletteBar = true;
    bool m_CompactMode = false;
    bool m_CircularPreview = false;
    
    // Interaction state
    bool m_IsDraggingWheel = false;
    bool m_IsDraggingSlider = false;
    bool m_IsDraggingGradient = false;
    int m_ActiveSlider = -1;
    size_t m_ActiveGradientStop = (size_t)-1;
    
    // Color harmony
    bool m_ColorHarmonyEnabled = false;
    std::string m_HarmonyRule = "Complementary";
    std::vector<glm::vec4> m_HarmonyColors;
    
    // Color blindness simulation
    bool m_ColorBlindSimulationEnabled = false;
    std::string m_ColorBlindnessType = "Protanopia";
    glm::vec4 m_SimulatedColor;
    
    // Palettes
    std::vector<ColorPalette> m_Palettes;
    size_t m_ActivePalette = (size_t)-1;
    
    // Gradient editor
    bool m_GradientEditorEnabled = false;
    std::vector<ColorStop> m_GradientStops;
    
    // Animation
    float m_WheelRotation = 0.0f;
    float m_PreviewPulse = 0.0f;
    
    // Callbacks
    std::function<void(const glm::vec4&)> m_ColorChangedCallback;
    std::function<void(const std::vector<ColorStop>&)> m_GradientChangedCallback;
    std::function<void(const std::string&)> m_PaletteChangedCallback;
};

} // namespace UI
} // namespace VGE



