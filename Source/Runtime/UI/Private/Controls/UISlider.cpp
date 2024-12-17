#include "U

#include "Runtime/Core/Public/Core/UICore.h"ISlider.h"
#include "Runtime/Core/Public/Core/Application.h"
#include "Renderer/RenderCommand.h"
#include <algorithm>
#include <glm/gtx/compatibility.hpp>

namespace VGE {
namespace Editor {

UISlider::UISlider(const std::string& label, SliderStyle style)
    : UIWidget(label)
    , m_Style(style) {
    m_Size = { 200.0f, 20.0f }; // Default size
}

void UISlider::Update() {
    if (!m_Enabled) return;

    auto& input = Application::Get().GetInput();
    float deltaTime = Application::Get().GetDeltaTime();

    // Update animations
    if (m_IsAnimating) {
        m_AnimationTime += deltaTime;
        float t = std::min(m_AnimationTime / m_AnimationDuration, 1.0f);
        t = glm::smoothstep(0.0f, 1.0f, t); // Smooth animation curve
        m_Value = glm::lerp(m_AnimationStartValue, m_AnimationTargetValue, t);
        
        if (t >= 1.0f) {
            m_IsAnimating = false;
            m_Value = m_AnimationTargetValue;
        }
    }

    // Handle keyboard input if enabled
    if (m_KeyboardInputEnabled && m_State == UIWidgetState::Focused) {
        float step = (m_Steps > 0) ? (m_MaxValue - m_MinValue) / m_Steps : 0.01f;
        
        if (input.IsKeyPressed(KeyCode::Left) || input.IsKeyPressed(KeyCode::Down)) {
            SetValue(m_Value - step);
        }
        if (input.IsKeyPressed(KeyCode::Right) || input.IsKeyPressed(KeyCode::Up)) {
            SetValue(m_Value + step);
        }
    }

    // Handle mouse wheel if enabled
    if (m_MouseWheelEnabled && IsPointInside(input.GetMousePosition())) {
        float wheelDelta = input.GetMouseWheelDelta();
        if (wheelDelta != 0.0f) {
            float step = (m_Steps > 0) ? (m_MaxValue - m_MinValue) / m_Steps : 0.01f;
            SetValue(m_Value + wheelDelta * step);
        }
    }

    // Visual feedback animations
    if (m_VisualFeedback) {
        float targetScale = (m_IsDragging || m_State == UIWidgetState::Hovered) ? 1.2f : 1.0f;
        m_HandleScale = glm::lerp(m_HandleScale, targetScale, deltaTime * 10.0f);

        float targetHighlight = m_IsDragging ? 1.0f : 0.0f;
        m_TrackHighlight = glm::lerp(m_TrackHighlight, targetHighlight, deltaTime * 8.0f);
    }
}

void UISlider::Draw() {
    if (!m_Visible || !m_Theme) return;

    // Render different components based on style
    switch (m_Style) {
        case SliderStyle::Standard:
            RenderTrack();
            RenderMarkers();
            RenderHandle();
            break;

        case SliderStyle::Radial: {
            // Render circular track
            float radius = std::min(m_Size.x, m_Size.y) * 0.5f;
            glm::vec2 center = m_Position + m_Size * 0.5f;
            
            // Background circle
            RenderCommand::SetColor(m_Theme->GetColors().SliderTrack);
            RenderCommand::DrawCircle(center, radius, 32);
            
            // Filled arc
            float angleStart = -90.0f; // Start from top
            float angleEnd = angleStart + (m_Value - m_MinValue) / (m_MaxValue - m_MinValue) * 360.0f;
            RenderCommand::SetColor(m_Theme->GetColors().SliderFill);
            RenderCommand::DrawArc(center, radius, angleStart, angleEnd, 32);
            
            // Handle
            float angle = glm::radians(angleEnd);
            glm::vec2 handlePos = center + glm::vec2(
                std::cos(angle) * radius,
                std::sin(angle) * radius
            );
            RenderCommand::SetColor(m_Theme->GetColors().SliderHandle);
            RenderCommand::DrawCircle(handlePos, 8.0f * m_HandleScale);
            break;
        }

        case SliderStyle::Curve: {
            // Render curved track with bezier curve
            // Implementation for curved slider visualization
            break;
        }

        case SliderStyle::MultiPoint: {
            // Render track with multiple control points
            RenderTrack();
            for (size_t i = 0; i < m_Values.size(); i++) {
                float t = (m_Values[i] - m_MinValue) / (m_MaxValue - m_MinValue);
                glm::vec2 handlePos = m_Position + glm::vec2(
                    t * m_Size.x,
                    m_Size.y * 0.5f
                );
                RenderCommand::SetColor(m_Theme->GetColors().SliderHandle);
                RenderCommand::DrawCircle(handlePos, 6.0f * m_HandleScale);
            }
            break;
        }

        case SliderStyle::Stepped: {
            // Render stepped slider with distinct segments
            RenderTrack();
            if (m_Steps > 0) {
                for (int i = 0; i <= m_Steps; i++) {
                    float t = static_cast<float>(i) / m_Steps;
                    glm::vec2 stepPos = m_Position + glm::vec2(
                        t * m_Size.x,
                        m_Size.y * 0.5f
                    );
                    RenderCommand::SetColor(m_Theme->GetColors().SliderTrack);
                    RenderCommand::DrawCircle(stepPos, 3.0f);
                }
            }
            RenderHandle();
            break;
        }
    }

    // Render value tooltip if enabled
    if (m_ShowValue && (m_IsDragging || m_State == UIWidgetState::Hovered)) {
        RenderValueTooltip();
    }
}

void UISlider::RenderTrack() {
    auto& colors = m_Theme->GetColors();
    float trackHeight = 4.0f;
    glm::vec2 trackPos = m_Position + glm::vec2(0.0f, (m_Size.y - trackHeight) * 0.5f);
    glm::vec2 trackSize = { m_Size.x, trackHeight };

    // Background track
    RenderCommand::SetColor(colors.SliderTrack);
    RenderCommand::DrawRoundedRect(trackPos, trackSize, trackHeight * 0.5f);

    // Filled portion
    float fillWidth = (m_Value - m_MinValue) / (m_MaxValue - m_MinValue) * m_Size.x;
    RenderCommand::SetColor(glm::mix(colors.SliderFill, colors.Accent, m_TrackHighlight));
    RenderCommand::DrawRoundedRect(trackPos, glm::vec2(fillWidth, trackHeight), trackHeight * 0.5f);
}

void UISlider::RenderHandle() {
    auto& colors = m_Theme->GetColors();
    glm::vec2 handlePos = GetHandlePosition();
    float handleRadius = 8.0f * m_HandleScale;

    // Handle shadow
    if (m_IsDragging) {
        RenderCommand::SetColor(glm::vec4(0.0f, 0.0f, 0.0f, 0.2f));
        RenderCommand::DrawCircle(handlePos + glm::vec2(0.0f, 1.0f), handleRadius + 1.0f);
    }

    // Handle
    RenderCommand::SetColor(m_IsDragging ? colors.SliderHandleActive : colors.SliderHandle);
    RenderCommand::DrawCircle(handlePos, handleRadius);

    // Handle highlight
    if (m_State == UIWidgetState::Hovered || m_IsDragging) {
        RenderCommand::SetColor(glm::vec4(1.0f, 1.0f, 1.0f, 0.1f));
        RenderCommand::DrawCircle(handlePos, handleRadius * 1.5f);
    }
}

void UISlider::RenderMarkers() {
    if (m_Markers.empty()) return;

    auto& colors = m_Theme->GetColors();
    float trackHeight = 4.0f;
    float markerHeight = 10.0f;

    for (const auto& marker : m_Markers) {
        float x = m_Position.x + marker.Position * m_Size.x;
        glm::vec2 markerPos = { x, m_Position.y + (m_Size.y - markerHeight) * 0.5f };

        // Marker line
        RenderCommand::SetColor(marker.Color);
        RenderCommand::DrawRect(markerPos, glm::vec2(1.0f, markerHeight));

        // Marker label
        if (!marker.Label.empty()) {
            RenderCommand::DrawText(
                marker.Label,
                markerPos + glm::vec2(-20.0f, -15.0f),
                glm::vec2(40.0f, 12.0f),
                UIAlignment::Center
            );
        }
    }
}

void UISlider::RenderValueTooltip() {
    char valueText[32];
    snprintf(valueText, sizeof(valueText), m_ValueFormat.c_str(), m_Value);

    glm::vec2 handlePos = GetHandlePosition();
    glm::vec2 tooltipSize = { 60.0f, 25.0f };
    glm::vec2 tooltipPos = handlePos - glm::vec2(tooltipSize.x * 0.5f, tooltipSize.y + 10.0f);

    // Tooltip background
    RenderCommand::SetColor(m_Theme->GetColors().TooltipBackground);
    RenderCommand::DrawRoundedRect(tooltipPos, tooltipSize, 4.0f);

    // Tooltip text
    RenderCommand::SetColor(m_Theme->GetColors().TooltipText);
    RenderCommand::DrawText(
        valueText,
        tooltipPos + glm::vec2(0.0f, 4.0f),
        tooltipSize,
        UIAlignment::Center
    );
}

bool UISlider::HandleEvent(Event& event) {
    if (!m_Enabled) return false;

    EventDispatcher dispatcher(event);
    
    dispatcher.Dispatch<MouseButtonPressedEvent>([this](MouseButtonPressedEvent& e) {
        if (e.GetMouseButton() == MouseButton::Left && IsPointInside(e.GetPosition())) {
            StartDrag(e.GetPosition());
            return true;
        }
        return false;
    });

    dispatcher.Dispatch<MouseButtonReleasedEvent>([this](MouseButtonReleasedEvent& e) {
        if (e.GetMouseButton() == MouseButton::Left && m_IsDragging) {
            EndDrag();
            return true;
        }
        return false;
    });

    dispatcher.Dispatch<MouseMovedEvent>([this](MouseMovedEvent& e) {
        if (m_IsDragging) {
            UpdateDrag(e.GetPosition());
            return true;
        }
        return false;
    });

    dispatcher.Dispatch<TouchBeganEvent>([this](TouchBeganEvent& e) {
        if (m_TouchInputEnabled && IsPointInside(e.GetPosition())) {
            StartDrag(e.GetPosition());
            return true;
        }
        return false;
    });

    return false;
}

void UISlider::SetValue(float value, bool notify) {
    float newValue = std::clamp(value, m_MinValue, m_MaxValue);
    
    if (m_Steps > 0) {
        newValue = SnapToNearestStep(newValue);
    }

    if (m_Value != newValue) {
        m_Value = newValue;
        if (notify && m_ValueChangedCallback) {
            m_ValueChangedCallback(m_Value);
        }
    }
}

void UISlider::SetRange(float min, float max) {
    m_MinValue = min;
    m_MaxValue = max;
    SetValue(m_Value, false); // Clamp current value to new range
}

void UISlider::SetSteps(int steps) {
    m_Steps = steps;
    if (steps > 0) {
        SetValue(m_Value, false); // Snap to nearest step
    }
}

float UISlider::SnapToNearestStep(float value) const {
    if (m_Steps <= 0) return value;

    float stepSize = (m_MaxValue - m_MinValue) / m_Steps;
    float steps = std::round((value - m_MinValue) / stepSize);
    return m_MinValue + steps * stepSize;
}

glm::vec2 UISlider::GetHandlePosition() const {
    float t = (m_Value - m_MinValue) / (m_MaxValue - m_MinValue);
    return m_Position + glm::vec2(
        t * m_Size.x,
        m_Size.y * 0.5f
    );
}

void UISlider::StartDrag(const glm::vec2& position) {
    m_IsDragging = true;
    m_DragStart = position;
    m_DragStartValue = m_Value;

    if (m_DragStartCallback) {
        m_DragStartCallback();
    }

    if (m_HapticFeedback) {
        // Trigger haptic feedback if available
        Application::Get().TriggerHapticFeedback(0.1f);
    }
}

void UISlider::UpdateDrag(const glm::vec2& position) {
    if (!m_IsDragging) return;

    float deltaX = (position.x - m_DragStart.x) * m_GestureScale;
    float valueRange = m_MaxValue - m_MinValue;
    float valueDelta = (deltaX / m_Size.x) * valueRange;
    
    SetValue(m_DragStartValue + valueDelta);
}

void UISlider::EndDrag() {
    m_IsDragging = false;

    if (m_DragEndCallback) {
        m_DragEndCallback();
    }

    if (m_HapticFeedback) {
        Application::Get().TriggerHapticFeedback(0.05f);
    }
}

void UISlider::AnimateToValue(float targetValue) {
    m_AnimationStartValue = m_Value;
    m_AnimationTargetValue = std::clamp(targetValue, m_MinValue, m_MaxValue);
    m_AnimationTime = 0.0f;
    m_IsAnimating = true;
}

} // namespace Editor
} // namespace VGE



