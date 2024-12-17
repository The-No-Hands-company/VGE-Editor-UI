#pragma once

#include "Core/UICore.h"
#include <iostream>
#include <string>

namespace VGE {
namespace UI {

enum class LogLevel {
    Info,
    Warning,
    Error
};

class UI_API Logger {
public:
    static void Log(const std::string& message, LogLevel level = LogLevel::Info) {
        switch (level) {
            case LogLevel::Info:
                std::cout << "[INFO] " << message << std::endl;
                break;
            case LogLevel::Warning:
                std::cerr << "[WARNING] " << message << std::endl;
                break;
            case LogLevel::Error:
                std::cerr << "[ERROR] " << message << std::endl;
                break;
        }
    }

    static void Warning(const std::string& message) {
        Log(message, LogLevel::Warning);
    }

    static void Error(const std::string& message) {
        Log(message, LogLevel::Error);
    }

    template<typename... Args>
    static void Log(const std::string& message, LogLevel level, Args&&... args) {
        char buffer[512];
        snprintf(buffer, sizeof(buffer), message.c_str(), std::forward<Args>(args)...);
        Log(buffer, level);
    }
};

}} // namespace VGE::UI 