#pragma once

// Platform detection
#if defined(_WIN32)
    #define UI_PLATFORM_WINDOWS
#elif defined(__APPLE__)
    #define UI_PLATFORM_MACOS
#elif defined(__linux__)
    #define UI_PLATFORM_LINUX
#else
    #error "Unsupported platform"
#endif

// DLL export/import macros
#ifdef UI_PLATFORM_WINDOWS
    #ifdef UI_STATIC_LIB
        // Static library build
        #define UI_API
    #else
        // Dynamic library build
        #ifdef UI_EXPORTS
            #define UI_API __declspec(dllexport)
        #else
            #define UI_API __declspec(dllimport)
        #endif
    #endif
#else
    // For non-Windows platforms, we don't need special export/import declarations
    #define UI_API
#endif

// Warning suppression for STL types in DLLs
#ifdef UI_PLATFORM_WINDOWS
    #pragma warning(disable: 4251)
#endif

// Debug macros
#ifdef UI_DEBUG
    #ifdef UI_PLATFORM_WINDOWS
        #define UI_DEBUGBREAK() __debugbreak()
    #else
        #include <signal.h>
        #define UI_DEBUGBREAK() raise(SIGTRAP)
    #endif
    #define UI_ASSERT(x, ...) { if(!(x)) { UI_ERROR("Assertion Failed: {0}", __VA_ARGS__); UI_DEBUGBREAK(); } }
#else
    #define UI_DEBUGBREAK()
    #define UI_ASSERT(x, ...)
#endif

// Standard includes
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <string>
#include <vector>
#include <memory>
#include <functional>
#include <unordered_map>
#include <iostream>
#include <cstdarg>

// Forward declarations of common types
namespace VGE {
namespace UI {

// Common types
using String = std::string;
using StringView = std::string_view;

// Smart pointer aliases
template<typename T>
using Ref = std::shared_ptr<T>;

template<typename T>
using Scope = std::unique_ptr<T>;

// Utility functions
template<typename T, typename... Args>
constexpr Ref<T> CreateRef(Args&&... args) {
    return std::make_shared<T>(std::forward<Args>(args)...);
}

template<typename T, typename... Args>
constexpr Scope<T> CreateScope(Args&&... args) {
    return std::make_unique<T>(std::forward<Args>(args)...);
}

// Color structure
struct Color {
    float r, g, b, a;

    Color() : r(0), g(0), b(0), a(1) {}
    Color(float r, float g, float b, float a = 1.0f) : r(r), g(g), b(b), a(a) {}
    Color(const glm::vec4& v) : r(v.r), g(v.g), b(v.b), a(v.a) {}

    operator glm::vec4() const { return glm::vec4(r, g, b, a); }

    static const Color White;
    static const Color Black;
    static const Color Red;
    static const Color Green;
    static const Color Blue;
    static const Color Yellow;
    static const Color Magenta;
    static const Color Cyan;
    static const Color Transparent;
};

// Rect structure
struct Rect {
    glm::vec2 min;
    glm::vec2 max;

    Rect() : min(0), max(0) {}
    Rect(const glm::vec2& min, const glm::vec2& max) : min(min), max(max) {}
    Rect(float x, float y, float width, float height) 
        : min(x, y), max(x + width, y + height) {}

    float GetWidth() const { return max.x - min.x; }
    float GetHeight() const { return max.y - min.y; }
    glm::vec2 GetSize() const { return max - min; }
    glm::vec2 GetCenter() const { return (min + max) * 0.5f; }

    bool Contains(const glm::vec2& point) const {
        return point.x >= min.x && point.x <= max.x &&
               point.y >= min.y && point.y <= max.y;
    }

    bool Intersects(const Rect& other) const {
        return !(max.x < other.min.x || other.max.x < min.x ||
                max.y < other.min.y || other.max.y < min.y);
    }

    Rect GetIntersection(const Rect& other) const {
        Rect result;
        result.min.x = std::max(min.x, other.min.x);
        result.min.y = std::max(min.y, other.min.y);
        result.max.x = std::min(max.x, other.max.x);
        result.max.y = std::min(max.y, other.max.y);
        return result;
    }

    void Expand(float amount) {
        min -= glm::vec2(amount);
        max += glm::vec2(amount);
    }

    void Expand(const glm::vec2& amount) {
        min -= amount;
        max += amount;
    }
};

// Text alignment
enum class TextAlignment {
    Left,
    Center,
    Right,
    Top,
    Bottom
};

// Forward declarations
class UIWidget;
class UIStyle;
class UIRenderer;
class UISlot;
class UIFont;
class UITexture;
class UIShader;
class UIBuffer;
class UIVertexBuffer;
class UIIndexBuffer;
class UIVertexArray;
class UIFontRenderer;

// Error handling
inline void UI_API UI_ERROR(const char* format, ...) {
    va_list args;
    va_start(args, format);
    vfprintf(stderr, format, args);
    va_end(args);
    fprintf(stderr, "\n");
}

}} // namespace VGE::UI
