#pragma once

#include "Runtime/Core/Public/Core/UICore.h"
#include <string>
#include <any>
#include <memory>
#include <vector>
#include "Runtime/Core/Public/Base/VGE.h"

namespace VGE {
namespace UI {

enum class UIEventType {
    // Window Events
    WindowOpen,
    WindowClose,
    WindowMove,
    WindowResize,
    WindowFocus,
    WindowBlur,
    WindowMinimize,
    WindowMaximize,
    WindowRestore,

    // Panel Events
    PanelOpen,
    PanelClose,
    PanelMove,
    PanelResize,
    PanelDock,
    PanelUndock,

    // Menu Events
    MenuOpen,
    MenuClose,
    MenuItemClick,
    MenuItemHover,

    // Toolbar Events
    ToolbarItemClick,
    ToolbarItemHover,
    ToolbarItemToggle,

    // Layout Events
    LayoutChange,
    LayoutSave,
    LayoutLoad,
    LayoutReset,

    // Property Events
    PropertyChange,
    PropertyBeginEdit,
    PropertyEndEdit,

    // Selection Events
    SelectionChange,
    SelectionClear,

    // Custom Event
    Custom
};

/**
 * @class UIEvent
 * @brief Base class for all UI events
 */
class UIEvent {
public:
    UIEvent(UIEventType type) : m_Type(type) {}
    virtual ~UIEvent() = default;

    UIEventType GetType() const { return m_Type; }
    const std::string& GetName() const { return m_Name; }
    void SetName(const std::string& name) { m_Name = name; }

    template<typename T>
    T* As() { return dynamic_cast<T*>(this); }

    template<typename T>
    const T* As() const { return dynamic_cast<const T*>(this); }

protected:
    UIEventType m_Type;
    std::string m_Name;
};

// Window Events
class WindowEvent : public UIEvent {
public:
    WindowEvent(UIEventType type, const std::string& windowName)
        : UIEvent(type), m_WindowName(windowName) {}

    const std::string& GetWindowName() const { return m_WindowName; }
    const glm::vec2& GetPosition() const { return m_Position; }
    const glm::vec2& GetSize() const { return m_Size; }
    void SetPosition(const glm::vec2& pos) { m_Position = pos; }
    void SetSize(const glm::vec2& size) { m_Size = size; }

private:
    std::string m_WindowName;
    glm::vec2 m_Position;
    glm::vec2 m_Size;
};

// Property Events
class PropertyEvent : public UIEvent {
public:
    PropertyEvent(UIEventType type, const std::string& propertyName, const std::any& oldValue, const std::any& newValue)
        : UIEvent(type), m_PropertyName(propertyName), m_OldValue(oldValue), m_NewValue(newValue) {}

    const std::string& GetPropertyName() const { return m_PropertyName; }
    const std::any& GetOldValue() const { return m_OldValue; }
    const std::any& GetNewValue() const { return m_NewValue; }

private:
    std::string m_PropertyName;
    std::any m_OldValue;
    std::any m_NewValue;
};

// Selection Events
class SelectionEvent : public UIEvent {
public:
    SelectionEvent(UIEventType type, const std::string& selectionContext)
        : UIEvent(type), m_SelectionContext(selectionContext) {}

    const std::string& GetSelectionContext() const { return m_SelectionContext; }
    const std::vector<std::string>& GetSelectedItems() const { return m_SelectedItems; }
    void AddSelectedItem(const std::string& item) { m_SelectedItems.push_back(item); }
    void ClearSelectedItems() { m_SelectedItems.clear(); }

private:
    std::string m_SelectionContext;
    std::vector<std::string> m_SelectedItems;
};

// Custom Event
class CustomEvent : public UIEvent {
public:
    CustomEvent(const std::string& name, const std::any& data)
        : UIEvent(UIEventType::Custom), m_Data(data) {
        SetName(name);
    }

    const std::any& GetData() const { return m_Data; }
    template<typename T>
    T GetDataAs() const { return std::any_cast<T>(m_Data); }

private:
    std::any m_Data;
};

}} // namespace VGE::UI



