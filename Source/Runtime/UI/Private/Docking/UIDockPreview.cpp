#include "Runtime/UI/Public/Docking/UIDockSpace.h"
#include "Runtime/UI/Public/Docking/UIDockZone.h"
#include "Runtime/Core/Public/Renderer/UIRenderer.h"

namespace VGE {
namespace Editor {

void UIDockPreview::Update(const glm::vec2& mousePos, const UIDockSpace& dockSpace) {
    if (!m_IsActive) return;

    // Update the current zone based on mouse position
    m_CurrentZone = UIDockZoneDetector::DetectZone(mousePos, dockSpace);

    // Calculate preview geometry
    auto [pos, size] = CalculatePreviewGeometry(m_CurrentZone, dockSpace);
    m_Position = pos;
    m_Size = size;
}

void UIDockPreview::Render() {
    if (!m_IsActive || m_CurrentZone == DockZone::None) return;

    // Get the renderer
    auto renderer = UIRenderer::Get();
    if (!renderer) return;

    // Set up preview color (semi-transparent blue)
    glm::vec4 previewColor(0.2f, 0.4f, 0.8f, m_Opacity);

    // Draw preview rectangle
    renderer->DrawRect(m_Position, m_Size, previewColor);

    // Draw preview border
    glm::vec4 borderColor(0.3f, 0.5f, 0.9f, m_Opacity + 0.2f);
    renderer->DrawRectOutline(m_Position, m_Size, borderColor, 2.0f);
}

std::pair<glm::vec2, glm::vec2> UIDockPreview::CalculatePreviewGeometry(
    DockZone zone, const UIDockSpace& dockSpace) const {
    
    const auto& dockSpacePos = dockSpace.GetPosition();
    const auto& dockSpaceSize = dockSpace.GetSize();

    switch (zone) {
        case DockZone::Left:
            return {
                dockSpacePos,
                glm::vec2(dockSpaceSize.x * 0.5f, dockSpaceSize.y)
            };

        case DockZone::Right:
            return {
                dockSpacePos + glm::vec2(dockSpaceSize.x * 0.5f, 0),
                glm::vec2(dockSpaceSize.x * 0.5f, dockSpaceSize.y)
            };

        case DockZone::Top:
            return {
                dockSpacePos,
                glm::vec2(dockSpaceSize.x, dockSpaceSize.y * 0.5f)
            };

        case DockZone::Bottom:
            return {
                dockSpacePos + glm::vec2(0, dockSpaceSize.y * 0.5f),
                glm::vec2(dockSpaceSize.x, dockSpaceSize.y * 0.5f)
            };

        case DockZone::Center:
            return {
                dockSpacePos + glm::vec2(dockSpaceSize.x * 0.1f, dockSpaceSize.y * 0.1f),
                dockSpaceSize * 0.8f
            };

        case DockZone::TopLeft:
            return {
                dockSpacePos,
                dockSpaceSize * 0.5f
            };

        case DockZone::TopRight:
            return {
                dockSpacePos + glm::vec2(dockSpaceSize.x * 0.5f, 0),
                dockSpaceSize * 0.5f
            };

        case DockZone::BottomLeft:
            return {
                dockSpacePos + glm::vec2(0, dockSpaceSize.y * 0.5f),
                dockSpaceSize * 0.5f
            };

        case DockZone::BottomRight:
            return {
                dockSpacePos + dockSpaceSize * 0.5f,
                dockSpaceSize * 0.5f
            };

        default:
            return {glm::vec2(0), glm::vec2(0)};
    }
}

DockZone UIDockZoneDetector::DetectZone(const glm::vec2& mousePos, const UIDockSpace& dockSpace) {
    const auto& dockSpacePos = dockSpace.GetPosition();
    const auto& dockSpaceSize = dockSpace.GetSize();

    // Convert mouse position to normalized coordinates within the dock space
    glm::vec2 normalizedPos = (mousePos - dockSpacePos) / dockSpaceSize;

    // Check if mouse is outside dock space
    if (normalizedPos.x < 0 || normalizedPos.x > 1 ||
        normalizedPos.y < 0 || normalizedPos.y > 1) {
        return DockZone::None;
    }

    // Check corners first
    if (normalizedPos.x < ZONE_CORNER_THRESHOLD && normalizedPos.y < ZONE_CORNER_THRESHOLD)
        return DockZone::TopLeft;
    if (normalizedPos.x > (1 - ZONE_CORNER_THRESHOLD) && normalizedPos.y < ZONE_CORNER_THRESHOLD)
        return DockZone::TopRight;
    if (normalizedPos.x < ZONE_CORNER_THRESHOLD && normalizedPos.y > (1 - ZONE_CORNER_THRESHOLD))
        return DockZone::BottomLeft;
    if (normalizedPos.x > (1 - ZONE_CORNER_THRESHOLD) && normalizedPos.y > (1 - ZONE_CORNER_THRESHOLD))
        return DockZone::BottomRight;

    // Then check edges
    if (normalizedPos.x < ZONE_EDGE_THRESHOLD)
        return DockZone::Left;
    if (normalizedPos.x > (1 - ZONE_EDGE_THRESHOLD))
        return DockZone::Right;
    if (normalizedPos.y < ZONE_EDGE_THRESHOLD)
        return DockZone::Top;
    if (normalizedPos.y > (1 - ZONE_EDGE_THRESHOLD))
        return DockZone::Bottom;

    // If not on any edge, must be in center
    return DockZone::Center;
}

}} // namespace VGE::Editor 