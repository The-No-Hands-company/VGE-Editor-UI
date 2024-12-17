#pragma once

#include "Core/UICore.h"
#include <string>

namespace VGE {
namespace UI {

struct UI_API UIWindowDesc {
    std::string title = "VersaUI Window";
    uint32_t width = 1280;
    uint32_t height = 720;
    bool resizable = true;
    bool decorated = true;
    bool floating = false;
    bool maximized = false;
    bool minimized = false;
    bool visible = true;
    bool focused = true;
    bool focusOnShow = true;
    bool scaleToMonitor = true;
    bool centerCursor = true;
    bool transparentFramebuffer = false;
    bool fullscreen = false;
    int samples = 0; // MSAA samples (0 = disabled)

    UIWindowDesc() = default;
    UIWindowDesc(const std::string& title) : title(title) {}
    UIWindowDesc(const std::string& title, uint32_t width, uint32_t height) 
        : title(title), width(width), height(height) {}
};

}} // namespace VGE::UI
