#pragma once

#include "Runtime/Core/Public/Core/UICore.h"
#include "UIEvent.h"
#include <functional>
#include <unordered_map>
#include <vector>
#include <memory>
#include <mutex>
#include <type_traits>

namespace VGE {
namespace UI {

using EventCallback = std::function<void(const UIEvent&)>;
using EventCallbackID = uint32_t;

/**
 * @class UIEventManager
 * @brief Manages event subscriptions and dispatching
 *
 * Handles:
 * - Event registration and unregistration
 * - Event dispatching
 * - Type-safe event handling
 * - Thread-safe event operations
 */
class UIEventManager {
public:
    static UIEventManager& Instance() {
        static UIEventManager instance;
        return instance;
    }

    // Event subscription
    template<typename T>
    EventCallbackID Subscribe(std::function<void(const T&)> callback) {
        static_assert(std::is_base_of<UIEvent, T>::value, "T must derive from UIEvent");
        std::lock_guard<std::mutex> lock(m_Mutex);
        
        EventCallbackID id = m_NextCallbackID++;
        m_Callbacks[typeid(T).hash_code()].push_back({id, [callback](const UIEvent& e) {
            callback(static_cast<const T&>(e));
        }});
        return id;
    }

    // Event unsubscription
    void Unsubscribe(EventCallbackID id) {
        std::lock_guard<std::mutex> lock(m_Mutex);
        for (auto& [type, callbacks] : m_Callbacks) {
            callbacks.erase(
                std::remove_if(callbacks.begin(), callbacks.end(),
                    [id](const auto& cb) { return cb.first == id; }),
                callbacks.end()
            );
        }
    }

    // Event dispatch
    template<typename T>
    void Dispatch(const T& event) {
        static_assert(std::is_base_of<UIEvent, T>::value, "T must derive from UIEvent");
        std::lock_guard<std::mutex> lock(m_Mutex);
        
        auto it = m_Callbacks.find(typeid(T).hash_code());
        if (it != m_Callbacks.end()) {
            for (const auto& [id, callback] : it->second) {
                callback(event);
            }
        }
    }

    // Clear all event subscriptions
    void Clear() {
        std::lock_guard<std::mutex> lock(m_Mutex);
        m_Callbacks.clear();
        m_NextCallbackID = 0;
    }

private:
    UIEventManager() = default;
    ~UIEventManager() = default;
    UIEventManager(const UIEventManager&) = delete;
    UIEventManager& operator=(const UIEventManager&) = delete;

    std::mutex m_Mutex;
    std::unordered_map<size_t, std::vector<std::pair<EventCallbackID, EventCallback>>> m_Callbacks;
    EventCallbackID m_NextCallbackID = 0;
};

}} // namespace VGE::UI



