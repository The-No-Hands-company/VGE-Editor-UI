#include "U

#include "Runtime/Core/Public/Core/UICore.h"IColorPicker.h"
#include "Runtime/Core/Public/Core/Application.h"
#include "Renderer/RenderCommand.h"
#include <glm/gtx/compatibility.hpp>
#include <algorithm>
#include <cmath>

namespace VGE {
namespace Editor {

UIColorPicker::UIColorPicker(const std::string& label)
    : UIWidget(label) {
    m_Size = { 300.0f, 400.0f }; // Default size

    // Initialize enabled color spaces
    m_EnabledColorSpaces = { ColorSpace::RGB, ColorSpace::HSV, ColorSpace::HSL };

    // Initialize default palette
    ColorPalette defaultPalette;
    defaultPalette.Name = "Default";
    defaultPalette.Colors = {
        glm::vec4(1.0f, 0.0f, 0.0f, 1.0f),  // Red
        glm::vec4(0.0f, 1.0f, 0.0f, 1.0f),  // Green
        glm::vec4(0.0f, 0.0f, 1.0f, 1.0f),  // Blue
        glm::vec4(1.0f, 1.0f, 0.0f, 1.0f),  // Yellow
        glm::vec4(1.0f, 0.0f, 1.0f, 1.0f),  // Magenta
        glm::vec4(0.0f, 1.0f, 1.0f, 1.0f)   // Cyan
    };
    m_Palettes.push_back(defaultPalette);
    m_ActivePalette = 0;
}

void UIColorPicker::Update() {
    if (!m_Enabled) return;

    float deltaTime = Application::Get().GetDeltaTime();

    // Update wheel rotation animation
    if (m_IsDraggingWheel) {
        m_WheelRotation += deltaTime * 30.0f;
    } else {
        m_WheelRotation = glm::mix(m_WheelRotation, 0.0f, deltaTime * 5.0f);
    }

    // Update preview pulse animation
    m_PreviewPulse = std::sin(Application::Get().GetTime() * 2.0f) * 0.1f + 0.9f;

    // Update harmony colors if enabled
    if (m_ColorHarmonyEnabled) {
        UpdateHarmonyColors();
    }

    // Update color blindness simulation if enabled
    if (m_ColorBlindSimulationEnabled) {
        UpdateColorBlindnessPreview();
    }
}

void UIColorPicker::Draw() {
    if (!m_Visible || !m_Theme) return;

    auto& colors = m_Theme->GetColors();
    auto& style = m_Theme->GetStyle();

    // Calculate layout
    float wheelSize = m_CompactMode ? 150.0f : 200.0f;
    float sliderWidth = m_CompactMode ? 20.0f : 30.0f;
    float spacing = style.WidgetSpacing;

    // Main container background
    RenderCommand::SetColor(colors.WidgetBackground);
    RenderCommand::DrawRoundedRect(m_Position, m_Size, style.WidgetRounding);

    // Color wheel section
    glm::vec2 wheelPos = m_Position + glm::vec2(spacing);
    RenderColorWheel();

    // Color sliders section
    glm::vec2 slidersPos = wheelPos + glm::vec2(wheelSize + spacing, 0.0f);
    RenderColorSliders();

    if (m_ShowAlphaChannel) {
        RenderAlphaSlider();
    }

    // Preview section
    if (m_ShowColorPreview) {
        RenderColorPreview();
    }

    // Color values section
    if (m_ShowColorValues) {
        RenderColorValues();
    }

    // Palette bar section
    if (m_ShowPaletteBar) {
        RenderPaletteBar();
    }

    // Gradient editor section
    if (m_GradientEditorEnabled) {
        RenderGradientEditor();
    }

    // Harmony preview section
    if (m_ColorHarmonyEnabled) {
        RenderHarmonyPreview();
    }

    // Color blindness preview section
    if (m_ColorBlindSimulationEnabled) {
        RenderColorBlindnessPreview();
    }
}

void UIColorPicker::RenderColorWheel() {
    auto& style = m_Theme->GetStyle();
    float wheelSize = m_CompactMode ? 150.0f : 200.0f;
    glm::vec2 wheelCenter = m_Position + glm::vec2(wheelSize * 0.5f);

    // Draw color wheel background
    RenderCommand::DrawColorWheel(
        wheelCenter,
        wheelSize * 0.5f,
        m_WheelRotation,
        m_Color.a
    );

    // Draw saturation/value square
    float squareSize = wheelSize * 0.7f;
    glm::vec2 squarePos = wheelCenter - glm::vec2(squareSize * 0.5f);
    
    RenderCommand::DrawSaturationValueSquare(
        squarePos,
        glm::vec2(squareSize),
        m_Color
    );

    // Draw current color indicator
    glm::vec2 colorPos = GetWheelPositionFromColor(m_Color);
    float indicatorSize = 10.0f * (m_IsDraggingWheel ? 1.2f : 1.0f);
    
    RenderCommand::SetColor(glm::vec4(1.0f));
    RenderCommand::DrawCircle(colorPos, indicatorSize);
    RenderCommand::SetColor(m_Color);
    RenderCommand::DrawCircle(colorPos, indicatorSize - 2.0f);
}

void UIColorPicker::RenderColorSliders() {
    auto& colors = m_Theme->GetColors();
    auto& style = m_Theme->GetStyle();
    float sliderWidth = m_CompactMode ? 20.0f : 30.0f;
    float sliderHeight = 150.0f;
    float spacing = style.WidgetSpacing;

    glm::vec2 sliderPos = m_Position + glm::vec2(m_Size.x - sliderWidth - spacing, spacing);

    // Render sliders based on current color space
    switch (m_ColorSpace) {
        case ColorSpace::RGB: {
            // Red slider
            RenderColorSlider(
                sliderPos,
                glm::vec2(sliderWidth, sliderHeight),
                m_Color.r,
                glm::vec4(0.0f, m_Color.g, m_Color.b, 1.0f),
                glm::vec4(1.0f, m_Color.g, m_Color.b, 1.0f),
                "R"
            );
            sliderPos.x -= sliderWidth + spacing;

            // Green slider
            RenderColorSlider(
                sliderPos,
                glm::vec2(sliderWidth, sliderHeight),
                m_Color.g,
                glm::vec4(m_Color.r, 0.0f, m_Color.b, 1.0f),
                glm::vec4(m_Color.r, 1.0f, m_Color.b, 1.0f),
                "G"
            );
            sliderPos.x -= sliderWidth + spacing;

            // Blue slider
            RenderColorSlider(
                sliderPos,
                glm::vec2(sliderWidth, sliderHeight),
                m_Color.b,
                glm::vec4(m_Color.r, m_Color.g, 0.0f, 1.0f),
                glm::vec4(m_Color.r, m_Color.g, 1.0f, 1.0f),
                "B"
            );
            break;
        }
        case ColorSpace::HSV: {
            // Similar implementation for HSV sliders
            break;
        }
        // Add other color space implementations
    }
}

void UIColorPicker::RenderColorSlider(
    const glm::vec2& position,
    const glm::vec2& size,
    float value,
    const glm::vec4& colorStart,
    const glm::vec4& colorEnd,
    const std::string& label
) {
    auto& colors = m_Theme->GetColors();
    auto& style = m_Theme->GetStyle();

    // Draw slider background gradient
    RenderCommand::DrawColorGradient(
        position,
        size,
        colorStart,
        colorEnd,
        true // vertical
    );

    // Draw slider handle
    float handleY = position.y + (1.0f - value) * (size.y - 4.0f);
    float handleWidth = size.x + 4.0f;
    float handleHeight = 4.0f;

    if (m_ActiveSlider == std::stoi(label)) {
        handleWidth += 4.0f;
        handleHeight += 2.0f;
    }

    RenderCommand::SetColor(colors.SliderHandle);
    RenderCommand::DrawRoundedRect(
        glm::vec2(position.x - 2.0f, handleY - handleHeight * 0.5f),
        glm::vec2(handleWidth, handleHeight),
        handleHeight * 0.5f
    );

    // Draw label
    if (!m_CompactMode) {
        RenderCommand::SetColor(colors.Text);
        RenderCommand::DrawText(
            label,
            position + glm::vec2(-20.0f, size.y * 0.5f),
            12.0f,
            UIAlignment::Right
        );
    }
}

void UIColorPicker::RenderAlphaSlider() {
    auto& style = m_Theme->GetStyle();
    float sliderWidth = m_CompactMode ? 20.0f : 30.0f;
    float sliderHeight = 150.0f;
    float spacing = style.WidgetSpacing;

    glm::vec2 sliderPos = m_Position + glm::vec2(
        m_Size.x - sliderWidth * 4.0f - spacing * 4.0f,
        spacing
    );

    // Draw checkerboard background
    RenderCommand::DrawCheckerboard(
        sliderPos,
        glm::vec2(sliderWidth, sliderHeight),
        8.0f // checker size
    );

    // Draw alpha gradient
    RenderCommand::DrawColorGradient(
        sliderPos,
        glm::vec2(sliderWidth, sliderHeight),
        glm::vec4(m_Color.r, m_Color.g, m_Color.b, 0.0f),
        glm::vec4(m_Color.r, m_Color.g, m_Color.b, 1.0f),
        true // vertical
    );

    // Draw alpha handle
    float handleY = sliderPos.y + (1.0f - m_Color.a) * (sliderHeight - 4.0f);
    RenderCommand::SetColor(glm::vec4(1.0f));
    RenderCommand::DrawRoundedRect(
        glm::vec2(sliderPos.x - 2.0f, handleY - 2.0f),
        glm::vec2(sliderWidth + 4.0f, 4.0f),
        2.0f
    );
}

bool UIColorPicker::HandleEvent(Event& event) {
    if (!m_Enabled) return false;

    EventDispatcher dispatcher(event);

    // Handle color wheel interaction
    if (HandleColorWheelInput(event)) return true;

    // Handle slider interaction
    if (HandleSliderInput(event)) return true;

    // Handle palette interaction
    if (m_ShowPaletteBar && HandlePaletteInput(event)) return true;

    // Handle gradient editor interaction
    if (m_GradientEditorEnabled && HandleGradientInput(event)) return true;

    return false;
}

void UIColorPicker::SetColor(const glm::vec4& color) {
    if (m_Color != color) {
        m_Color = color;
        NotifyColorChanged();
        
        if (m_ColorHarmonyEnabled) {
            UpdateHarmonyColors();
        }
        
        if (m_ColorBlindSimulationEnabled) {
            UpdateColorBlindnessPreview();
        }
    }
}

void UIColorPicker::UpdateHarmonyColors() {
    m_HarmonyColors.clear();
    
    // Calculate harmony colors based on the current rule
    if (m_HarmonyRule == "Complementary") {
        // Add complementary color
        glm::vec3 hsv = RGBtoHSV(glm::vec3(m_Color));
        hsv.x = fmod(hsv.x + 0.5f, 1.0f);
        m_HarmonyColors.push_back(glm::vec4(HSVtoRGB(hsv), m_Color.a));
    }
    else if (m_HarmonyRule == "Triadic") {
        // Add two colors at 120-degree intervals
        glm::vec3 hsv = RGBtoHSV(glm::vec3(m_Color));
        for (int i = 1; i <= 2; ++i) {
            float hue = fmod(hsv.x + (i * 0.333f), 1.0f);
            glm::vec3 harmonyHsv = glm::vec3(hue, hsv.y, hsv.z);
            m_HarmonyColors.push_back(glm::vec4(HSVtoRGB(harmonyHsv), m_Color.a));
        }
    }
    // Add other harmony rules as needed
}

void UIColorPicker::UpdateColorBlindnessPreview() {
    // Simulate color blindness based on the selected type
    if (m_ColorBlindnessType == "Protanopia") {
        m_SimulatedColor = SimulateProtanopia(m_Color);
    }
    else if (m_ColorBlindnessType == "Deuteranopia") {
        m_SimulatedColor = SimulateDeuteranopia(m_Color);
    }
    else if (m_ColorBlindnessType == "Tritanopia") {
        m_SimulatedColor = SimulateTritanopia(m_Color);
    }
}

void UIColorPicker::NotifyColorChanged() {
    if (m_ColorChangedCallback) {
        m_ColorChangedCallback(m_Color);
    }
}

} // namespace Editor
} // namespace VGE



