#pragma once

#include "Runtime/Core/Public/Core/UICore.h"
#include <string>
#include <memory>
#include <vector>
#include <any>
#include <unordered_map>
#include <functional>
#include <glm/glm.hpp>

namespace VGE {
namespace UI {

/**
 * @class UIObject
 * @brief Base class for all UI objects in the Versa Engine UI framework.
 * 
 * Provides core functionality that all UI objects should have:
 * - Unique identification
 * - Name management
 * - Enable/disable state
 * - Visibility control
 * - Property system
 * - Event handling
 * - Hierarchy management
 */
class UIObject {
public:
    explicit UIObject(const std::string& name = "");
    virtual ~UIObject() = default;

    // Identification
    const std::string& GetID() const { return m_ID; }
    const std::string& GetName() const { return m_Name; }
    void SetName(const std::string& name) { m_Name = name; }

    // State Management
    bool IsEnabled() const { return m_Enabled; }
    virtual void SetEnabled(bool enabled) { m_Enabled = enabled; }
    
    bool IsVisible() const { return m_Visible; }
    virtual void SetVisible(bool visible) { m_Visible = visible; }

    // Position and Size
    const glm::vec2& GetPosition() const { return m_Position; }
    void SetPosition(const glm::vec2& position) { m_Position = position; }

    const glm::vec2& GetSize() const { return m_Size; }
    void SetSize(const glm::vec2& size) { m_Size = size; }

    // Property System
    template<typename T>
    void SetProperty(const std::string& name, const T& value) {
        m_Properties[name] = value;
    }

    template<typename T>
    T GetProperty(const std::string& name) const {
        auto it = m_Properties.find(name);
        if (it != m_Properties.end()) {
            return std::any_cast<T>(it->second);
        }
        return T();
    }

    bool HasProperty(const std::string& name) const {
        return m_Properties.find(name) != m_Properties.end();
    }

    void RemoveProperty(const std::string& name) {
        m_Properties.erase(name);
    }

    // Event Handling
    using EventCallback = std::function<void()>;
    void AddEventListener(const std::string& eventName, EventCallback callback);
    void RemoveEventListener(const std::string& eventName);
    void DispatchEvent(const std::string& eventName);

    // Hierarchy
    void AddChild(std::shared_ptr<UIObject> child);
    void RemoveChild(std::shared_ptr<UIObject> child);
    const std::vector<std::shared_ptr<UIObject>>& GetChildren() const { return m_Children; }

    // Virtual methods
    virtual void Initialize() {}
    virtual void Shutdown() {}
    virtual void Update(float deltaTime) {}
    virtual void Draw() {}

protected:
    // Generate a unique ID for this object
    static std::string GenerateUniqueID();

private:
    std::string m_ID;
    std::string m_Name;
    bool m_Enabled;
    bool m_Visible{true};
    glm::vec2 m_Position{0.0f, 0.0f};
    glm::vec2 m_Size{100.0f, 100.0f};
    std::unordered_map<std::string, std::any> m_Properties;
    std::vector<std::shared_ptr<UIObject>> m_Children;
    std::unordered_map<std::string, std::vector<EventCallback>> m_EventCallbacks;
};

}} // namespace VGE::UI



