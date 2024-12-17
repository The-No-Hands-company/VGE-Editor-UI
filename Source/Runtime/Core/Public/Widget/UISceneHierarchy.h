#pragma once

#include "Runtime/Core/Public/Widget/UITreeView.h"
#include "Runtime/Core/Public/Theme/UIAnimatable.h"
#include <memory>
#include <vector>
#include <string>

namespace VGE {
namespace UI {

// Forward declarations
class UIPropertyPanel;
struct PropertyChangeEvent;

class UI_API SceneNode {
public:
    virtual ~SceneNode() = default;
    
    // Node properties
    virtual std::string GetName() const = 0;
    virtual void SetName(const std::string& name) = 0;
    
    virtual glm::vec3 GetPosition() const = 0;
    virtual void SetPosition(const glm::vec3& position) = 0;
    
    virtual glm::vec3 GetRotation() const = 0;
    virtual void SetRotation(const glm::vec3& rotation) = 0;
    
    virtual glm::vec3 GetScale() const = 0;
    virtual void SetScale(const glm::vec3& scale) = 0;
    
    // Property system integration
    virtual void RegisterProperties(UIPropertyPanel* panel) {
        // Register additional properties specific to this node type
    }
    
    virtual void OnPropertyChanged(const PropertyChangeEvent& event) {
        // Handle property changes specific to this node type
    }
    
    // Tree hierarchy
    virtual SceneNode* GetParent() const = 0;
    virtual void SetParent(SceneNode* parent) = 0;
    virtual const std::vector<SceneNode*>& GetChildren() const = 0;
    virtual void AddChild(SceneNode* child) = 0;
    virtual void RemoveChild(SceneNode* child) = 0;
    
    // Node operations
    virtual void OnSelected() {}
    virtual void OnDeselected() {}
    virtual bool CanAcceptChild(SceneNode* child) const { return true; }
    virtual bool CanBeParent() const { return true; }
    virtual bool CanBeReordered() const { return true; }
};

class UI_API UISceneHierarchy : public UITreeView, public UIAnimatable {
public:
    UISceneHierarchy();
    virtual ~UISceneHierarchy() = default;
    
    // UIWidget interface
    void OnLayoutUpdate() override;
    glm::vec2 CalculateDesiredSize() override;
    void ArrangeChildren() override;
    void OnPaint(UIRenderer* renderer) override;
    
    // UIAnimatable interface
    void OnAnimationUpdate(float deltaTime) override;
    void OnAnimationStateChanged(const std::string& newState) override;
    
    // Scene management
    void SetRootNode(SceneNode* root);
    SceneNode* GetRootNode() const { return m_RootNode; }
    
    void AddNode(SceneNode* node, SceneNode* parent = nullptr);
    void RemoveNode(SceneNode* node);
    void ClearNodes();
    
    // Selection
    const std::vector<SceneNode*>& GetSelectedNodes() const { return m_SelectedNodes; }
    void SelectNode(SceneNode* node, bool addToSelection = false);
    void DeselectNode(SceneNode* node);
    void ClearSelection();
    
    // Node operations
    bool CanDragNode(SceneNode* node) const;
    bool CanDropNode(SceneNode* node, SceneNode* target) const;
    void MoveNode(SceneNode* node, SceneNode* newParent);
    void ReorderNode(SceneNode* node, SceneNode* target, bool before);
    
    // Property panel integration
    UIPropertyPanel* GetPropertyPanel() const { return m_PropertyPanel.get(); }
    
protected:
    // Tree view overrides
    void OnSelectionChanged(const std::vector<SceneNode*>& selection) override;
    void OnNodeMoved(SceneNode* node, SceneNode* newParent) override;
    void OnNodeReordered(SceneNode* node, SceneNode* target, bool before) override;
    
    // Property panel callbacks
    void OnPropertyChanged(const PropertyChangeEvent& event);
    
    // Node refresh
    void RefreshNode(SceneNode* node);
    void RefreshNodeAndChildren(SceneNode* node);
    
private:
    SceneNode* m_RootNode = nullptr;
    std::vector<SceneNode*> m_SelectedNodes;
    std::shared_ptr<UIPropertyPanel> m_PropertyPanel;
    
    // Animation states
    struct {
        float SelectionAlpha = 0.0f;
        float DragAlpha = 0.0f;
        float DropTargetAlpha = 0.0f;
    } m_AnimationState;
};

}} // namespace VGE::UI 