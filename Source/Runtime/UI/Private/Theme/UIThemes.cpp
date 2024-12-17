#include "Runtime/Core/Public/Widget/UIStyle.h"
#include "Runtime/Core/Public/Widget/UIStyleManager.h"
#include <fstream>
#include <nlohmann/json.hpp>

namespace VGE {
namespace UI {

// Helper functions for JSON serialization
namespace {
    glm::vec4 LoadColorFromJson(const nlohmann::json& j) {
        return glm::vec4(j[0].get<float>(), j[1].get<float>(), j[2].get<float>(), j[3].get<float>());
    }

    nlohmann::json SaveColorToJson(const glm::vec4& color) {
        return nlohmann::json::array({ color.r, color.g, color.b, color.a });
    }
}

UIThemePtr CreateDarkTheme()
{
    auto theme = std::make_shared<UITheme>("Dark");

    // Base colors
    theme->SetColor("background", glm::vec4(0.12f, 0.12f, 0.12f, 1.0f));
    theme->SetColor("background.alt", glm::vec4(0.16f, 0.16f, 0.16f, 1.0f));
    theme->SetColor("foreground", glm::vec4(0.90f, 0.90f, 0.90f, 1.0f));
    theme->SetColor("border", glm::vec4(0.25f, 0.25f, 0.25f, 1.0f));
    theme->SetColor("accent", glm::vec4(0.20f, 0.60f, 1.00f, 1.0f));
    theme->SetColor("accent.hover", glm::vec4(0.30f, 0.70f, 1.00f, 1.0f));
    theme->SetColor("accent.active", glm::vec4(0.15f, 0.55f, 0.95f, 1.0f));
    
    // Semantic colors
    theme->SetColor("success", glm::vec4(0.20f, 0.80f, 0.20f, 1.0f));
    theme->SetColor("warning", glm::vec4(0.90f, 0.70f, 0.10f, 1.0f));
    theme->SetColor("error", glm::vec4(0.90f, 0.20f, 0.20f, 1.0f));
    theme->SetColor("info", glm::vec4(0.20f, 0.60f, 0.90f, 1.0f));

    // Create and register styles
    auto defaultStyle = UIStyle::CreateDefault();
    defaultStyle->BackgroundColor = theme->GetColor("background");
    defaultStyle->TextColor = theme->GetColor("foreground");
    defaultStyle->BorderColor = theme->GetColor("border");
    theme->RegisterStyle("default", defaultStyle);

    auto buttonStyle = UIStyle::CreateButton();
    buttonStyle->BackgroundColor = theme->GetColor("background.alt");
    buttonStyle->TextColor = theme->GetColor("foreground");
    buttonStyle->BorderColor = theme->GetColor("border");
    buttonStyle->HoverBackgroundColor = theme->GetColor("accent.hover");
    buttonStyle->PressedBackgroundColor = theme->GetColor("accent.active");
    buttonStyle->BorderRadius = 4.0f;
    theme->RegisterStyle("button", buttonStyle);

    auto panelStyle = UIStyle::CreatePanel();
    panelStyle->BackgroundColor = theme->GetColor("background");
    panelStyle->BorderColor = theme->GetColor("border");
    panelStyle->BorderRadius = 2.0f;
    theme->RegisterStyle("panel", panelStyle);

    auto textStyle = UIStyle::CreateText();
    textStyle->TextColor = theme->GetColor("foreground");
    theme->RegisterStyle("text", textStyle);

    return theme;
}

UIThemePtr CreateLightTheme()
{
    auto theme = std::make_shared<UITheme>("Light");

    // Base colors
    theme->SetColor("background", glm::vec4(0.98f, 0.98f, 0.98f, 1.0f));
    theme->SetColor("background.alt", glm::vec4(0.94f, 0.94f, 0.94f, 1.0f));
    theme->SetColor("foreground", glm::vec4(0.10f, 0.10f, 0.10f, 1.0f));
    theme->SetColor("border", glm::vec4(0.75f, 0.75f, 0.75f, 1.0f));
    theme->SetColor("accent", glm::vec4(0.00f, 0.50f, 1.00f, 1.0f));
    theme->SetColor("accent.hover", glm::vec4(0.10f, 0.60f, 1.00f, 1.0f));
    theme->SetColor("accent.active", glm::vec4(0.00f, 0.45f, 0.95f, 1.0f));
    
    // Semantic colors
    theme->SetColor("success", glm::vec4(0.20f, 0.70f, 0.20f, 1.0f));
    theme->SetColor("warning", glm::vec4(0.80f, 0.60f, 0.00f, 1.0f));
    theme->SetColor("error", glm::vec4(0.80f, 0.10f, 0.10f, 1.0f));
    theme->SetColor("info", glm::vec4(0.10f, 0.50f, 0.80f, 1.0f));

    // Create and register styles
    auto defaultStyle = UIStyle::CreateDefault();
    defaultStyle->BackgroundColor = theme->GetColor("background");
    defaultStyle->TextColor = theme->GetColor("foreground");
    defaultStyle->BorderColor = theme->GetColor("border");
    theme->RegisterStyle("default", defaultStyle);

    auto buttonStyle = UIStyle::CreateButton();
    buttonStyle->BackgroundColor = theme->GetColor("background.alt");
    buttonStyle->TextColor = theme->GetColor("foreground");
    buttonStyle->BorderColor = theme->GetColor("border");
    buttonStyle->HoverBackgroundColor = theme->GetColor("accent.hover");
    buttonStyle->PressedBackgroundColor = theme->GetColor("accent.active");
    buttonStyle->BorderRadius = 4.0f;
    theme->RegisterStyle("button", buttonStyle);

    auto panelStyle = UIStyle::CreatePanel();
    panelStyle->BackgroundColor = theme->GetColor("background");
    panelStyle->BorderColor = theme->GetColor("border");
    panelStyle->BorderRadius = 2.0f;
    theme->RegisterStyle("panel", panelStyle);

    auto textStyle = UIStyle::CreateText();
    textStyle->TextColor = theme->GetColor("foreground");
    theme->RegisterStyle("text", textStyle);

    return theme;
}

// Register themes with the style manager
void InitializeThemes()
{
    auto& styleManager = UIStyleManager::Get();
    
    // Register themes
    styleManager.RegisterTheme(CreateDarkTheme());
    styleManager.RegisterTheme(CreateLightTheme());
    
    // Set dark theme as default
    styleManager.SetActiveTheme("Dark");
}

void UITheme::SaveToFile(const std::string& filepath) const {
    try {
        nlohmann::json j;

        // Save colors
        for (const auto& [name, color] : m_Colors) {
            j["colors"][name] = SaveColorToJson(color);
        }

        // Save styles
        for (const auto& [name, style] : m_Styles) {
            auto& styleJson = j["styles"][name];
            styleJson["backgroundColor"] = SaveColorToJson(style->BackgroundColor);
            styleJson["textColor"] = SaveColorToJson(style->TextColor);
            styleJson["borderColor"] = SaveColorToJson(style->BorderColor);
            styleJson["hoverBackgroundColor"] = SaveColorToJson(style->HoverBackgroundColor);
            styleJson["pressedBackgroundColor"] = SaveColorToJson(style->PressedBackgroundColor);
            styleJson["borderRadius"] = style->BorderRadius;
            styleJson["fontSize"] = style->FontSize;
            styleJson["padding"] = style->Padding;
        }

        std::ofstream file(filepath);
        if (!file.is_open()) {
            throw std::runtime_error("Failed to open theme file for writing: " + filepath);
        }

        file << std::setw(4) << j << std::endl;
    }
    catch (const std::exception& e) {
        throw std::runtime_error("Failed to save theme: " + std::string(e.what()));
    }
}

void UITheme::LoadFromFile(const std::string& filepath) {
    try {
        std::ifstream file(filepath);
        if (!file.is_open()) {
            throw std::runtime_error("Failed to open theme file: " + filepath);
        }

        nlohmann::json j;
        file >> j;

        // Load colors
        for (const auto& [name, value] : j["colors"].items()) {
            SetColor(name, LoadColorFromJson(value));
        }

        // Load styles
        for (const auto& [name, value] : j["styles"].items()) {
            auto style = std::make_shared<UIStyle>();
            style->BackgroundColor = LoadColorFromJson(value["backgroundColor"]);
            style->TextColor = LoadColorFromJson(value["textColor"]);
            style->BorderColor = LoadColorFromJson(value["borderColor"]);
            style->HoverBackgroundColor = LoadColorFromJson(value["hoverBackgroundColor"]);
            style->PressedBackgroundColor = LoadColorFromJson(value["pressedBackgroundColor"]);
            style->BorderRadius = value["borderRadius"].get<float>();
            style->FontSize = value["fontSize"].get<float>();
            style->Padding = value["padding"].get<float>();
            RegisterStyle(name, style);
        }
    }
    catch (const std::exception& e) {
        throw std::runtime_error("Failed to load theme: " + std::string(e.what()));
    }
}

}} // namespace VGE::UI 