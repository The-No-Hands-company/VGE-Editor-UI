#pragma once

// Engine includes
#include "Runtime/Core/Public/Core/UICore.h"
#include "Runtime/Core/Public/Widget/UIWidget.h"

// Standard library includes
#include <functional>
#include <vector>

namespace VGE {
namespace UI {

enum class SliderStyle {
    Standard,       // Traditional horizontal/vertical slider
    Radial,        // Circular/radial slider for angular values
    Curve,         // Curve-based slider for non-linear input
    MultiPoint,    // Multiple control points on a single slider
    Stepped        // Discrete steps with optional snap points
};

enum class SliderInterpolation {
    Linear,
    Exponential,
    Logarithmic,
    Smooth,
    Custom         // Custom interpolation curve
};

struct SliderMarker {
    float Position;    // 0-1 range
    std::string Label;
    glm::vec4 Color;
    bool IsSnapPoint;
};

class UI_API UISlider : public UIWidget {
public:
    UISlider(const std::string& label = "", SliderStyle style = SliderStyle::Standard);
    virtual ~UISlider() = default;

    virtual void Update() override;
    virtual void Draw() override;
    virtual bool HandleEvent(Event& event) override;

    // Advanced value handling
    void SetValue(float value, bool notify = true);
    void SetValues(const std::vector<float>& values); // For multi-point slider
    float GetValue() const { return m_Value; }
    const std::vector<float>& GetValues() const { return m_Values; }

    // Range and constraints
    void SetRange(float min, float max);
    void SetSteps(int steps);  // For stepped slider
    void SetPrecision(int decimalPlaces);
    void SetInterpolation(SliderInterpolation type);
    void SetCustomInterpolation(std::function<float(float)> curve);

    // Visual customization
    void SetStyle(SliderStyle style);
    void SetOrientation(bool vertical);
    void SetShowValue(bool show) { m_ShowValue = show; }
    void SetValueFormat(const std::string& format) { m_ValueFormat = format; }
    void AddMarker(float position, const std::string& label = "", 
                  const glm::vec4& color = glm::vec4(1.0f), bool isSnapPoint = false);
    void ClearMarkers();

    // Gesture and input handling
    void SetTouchInputEnabled(bool enabled) { m_TouchInputEnabled = enabled; }
    void SetMouseWheelEnabled(bool enabled) { m_MouseWheelEnabled = enabled; }
    void SetKeyboardInputEnabled(bool enabled) { m_KeyboardInputEnabled = enabled; }
    void SetGestureScale(float scale) { m_GestureScale = scale; }

    // Animation and feedback
    void SetAnimationDuration(float seconds) { m_AnimationDuration = seconds; }
    void SetHapticFeedback(bool enabled) { m_HapticFeedback = enabled; }
    void SetVisualFeedback(bool enabled) { m_VisualFeedback = enabled; }

    // Event callbacks
    void SetValueChangedCallback(std::function<void(float)> callback) {
        m_ValueChangedCallback = callback;
    }
    void SetDragStartCallback(std::function<void()> callback) {
        m_DragStartCallback = callback;
    }
    void SetDragEndCallback(std::function<void()> callback) {
        m_DragEndCallback = callback;
    }

private:
    float InterpolateValue(float raw) const;
    void UpdateHandlePosition();
    void StartDrag(const glm::vec2& position);
    void UpdateDrag(const glm::vec2& position);
    void EndDrag();
    void AnimateToValue(float targetValue);
    float SnapToNearestStep(float value) const;
    glm::vec2 GetHandlePosition() const;
    void RenderTrack();
    void RenderHandle();
    void RenderMarkers();
    void RenderValueTooltip();

    // Core properties
    float m_Value = 0.0f;
    std::vector<float> m_Values;  // For multi-point mode
    float m_MinValue = 0.0f;
    float m_MaxValue = 1.0f;
    int m_Steps = 0;
    int m_Precision = 2;
    
    // Visual state
    SliderStyle m_Style;
    SliderInterpolation m_InterpolationType = SliderInterpolation::Linear;
    std::function<float(float)> m_CustomInterpolation;
    bool m_IsVertical = false;
    bool m_ShowValue = true;
    std::string m_ValueFormat = "%.2f";
    std::vector<SliderMarker> m_Markers;
    
    // Input state
    bool m_IsDragging = false;
    glm::vec2 m_DragStart;
    float m_DragStartValue = 0.0f;
    bool m_TouchInputEnabled = true;
    bool m_MouseWheelEnabled = true;
    bool m_KeyboardInputEnabled = true;
    float m_GestureScale = 1.0f;

    // Animation
    float m_AnimationDuration = 0.2f;
    float m_AnimationTime = 0.0f;
    float m_AnimationStartValue = 0.0f;
    float m_AnimationTargetValue = 0.0f;
    bool m_IsAnimating = false;

    // Feedback
    bool m_HapticFeedback = true;
    bool m_VisualFeedback = true;
    float m_HandleScale = 1.0f;
    float m_TrackHighlight = 0.0f;

    // Callbacks
    std::function<void(float)> m_ValueChangedCallback;
    std::function<void()> m_DragStartCallback;
    std::function<void()> m_DragEndCallback;
};

} // namespace UI
} // namespace VGE



