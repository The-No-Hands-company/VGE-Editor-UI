#pragma once

#include "Runtime/Core/Public/Core/UICore.h"

#include <memory>
#include <string>
#include <vector>
#include "UI/Framework/Core/UIElement.h"

namespace VGE::Editor::UI {

struct DragDropData {
    std::string type;
    void* data;
    size_t size;
};

class DragDropManager {
public:
    // Core functionality
    void Initialize();
    void Shutdown();
    void Update(float deltaTime);
    
    // Drag operations
    void BeginDrag(const std::string& type, void* data, size_t size);
    void UpdateDrag(float x, float y);
    void EndDrag();
    
    // Drop operations
    bool CanDrop(const std::string& acceptedType);
    bool ProcessDrop(void* outData, size_t* outSize);
    
    // Visual feedback
    void RenderDragPreview();
    void SetDragVisual(std::unique_ptr<UIElement> visual);
    
    // State queries
    bool IsDragging() const { return m_IsDragging; }
    const std::string& GetDragType() const { return m_CurrentDrag.type; }

private:
    bool m_IsDragging;
    DragDropData m_CurrentDrag;
    float m_DragX, m_DragY;
    std::unique_ptr<UIElement> m_DragVisual;
    
    // Helper functions
    void UpdateVisualPosition(float x, float y);
    void CleanupDragData();
};

} // namespace VGE::Editor::UI



