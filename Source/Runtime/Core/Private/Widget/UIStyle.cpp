#include "Runtime/Core/Public/Widget/UIStyle.h"
#include <algorithm>
#include <fstream>
#include <json/json.h>

namespace VGE {
namespace UI {

// Enum serialization helpers
std::string BorderStyleToString(UIBorderStyle style) {
    switch (style) {
        case UIBorderStyle::None: return "none";
        case UIBorderStyle::Solid: return "solid";
        case UIBorderStyle::Dashed: return "dashed";
        case UIBorderStyle::Dotted: return "dotted";
        case UIBorderStyle::Double: return "double";
        default: return "none";
    }
}

UIBorderStyle BorderStyleFromString(const std::string& str) {
    if (str == "solid") return UIBorderStyle::Solid;
    if (str == "dashed") return UIBorderStyle::Dashed;
    if (str == "dotted") return UIBorderStyle::Dotted;
    if (str == "double") return UIBorderStyle::Double;
    return UIBorderStyle::None;
}

std::string BackgroundFillToString(UIBackgroundFill fill) {
    switch (fill) {
        case UIBackgroundFill::None: return "none";
        case UIBackgroundFill::Solid: return "solid";
        case UIBackgroundFill::Gradient: return "gradient";
        case UIBackgroundFill::Texture: return "texture";
        default: return "none";
    }
}

UIBackgroundFill BackgroundFillFromString(const std::string& str) {
    if (str == "solid") return UIBackgroundFill::Solid;
    if (str == "gradient") return UIBackgroundFill::Gradient;
    if (str == "texture") return UIBackgroundFill::Texture;
    return UIBackgroundFill::None;
}

std::string GradientTypeToString(UIGradientType type) {
    switch (type) {
        case UIGradientType::Linear: return "linear";
        case UIGradientType::Radial: return "radial";
        case UIGradientType::Angular: return "angular";
        default: return "linear";
    }
}

UIGradientType GradientTypeFromString(const std::string& str) {
    if (str == "radial") return UIGradientType::Radial;
    if (str == "angular") return UIGradientType::Angular;
    return UIGradientType::Linear;
}

// UIStyle serialization helpers
glm::vec4 UIStyle::ColorFromJson(const Json::Value& value) {
    return glm::vec4(
        value.get("r", 0.0f).asFloat(),
        value.get("g", 0.0f).asFloat(),
        value.get("b", 0.0f).asFloat(),
        value.get("a", 1.0f).asFloat()
    );
}

Json::Value UIStyle::ColorToJson(const glm::vec4& color) {
    Json::Value value;
    value["r"] = color.r;
    value["g"] = color.g;
    value["b"] = color.b;
    value["a"] = color.a;
    return value;
}

glm::vec2 UIStyle::Vec2FromJson(const Json::Value& value) {
    return glm::vec2(
        value.get("x", 0.0f).asFloat(),
        value.get("y", 0.0f).asFloat()
    );
}

Json::Value UIStyle::Vec2ToJson(const glm::vec2& vec) {
    Json::Value value;
    value["x"] = vec.x;
    value["y"] = vec.y;
    return value;
}

void UIStyle::SerializeInheritanceFlags(Json::Value& value) const {
    Json::Value inheritance;
    inheritance["backgroundColor"] = m_InheritBackgroundColor;
    inheritance["borderColor"] = m_InheritBorderColor;
    inheritance["textColor"] = m_InheritTextColor;
    inheritance["font"] = m_InheritFont;
    inheritance["effects"] = m_InheritEffects;
    inheritance["transitions"] = m_InheritTransitions;
    value["inheritance"] = inheritance;
}

void UIStyle::DeserializeInheritanceFlags(const Json::Value& value) {
    const Json::Value& inheritance = value["inheritance"];
    m_InheritBackgroundColor = inheritance.get("backgroundColor", true).asBool();
    m_InheritBorderColor = inheritance.get("borderColor", true).asBool();
    m_InheritTextColor = inheritance.get("textColor", true).asBool();
    m_InheritFont = inheritance.get("font", true).asBool();
    m_InheritEffects = inheritance.get("effects", true).asBool();
    m_InheritTransitions = inheritance.get("transitions", true).asBool();
}

Json::Value UIStyle::Serialize() const {
    Json::Value value;

    // Colors
    value["backgroundColor"] = ColorToJson(BackgroundColor);
    value["borderColor"] = ColorToJson(BorderColor);
    value["textColor"] = ColorToJson(TextColor);

    // Background
    value["backgroundFill"] = BackgroundFillToString(BackgroundFill);
    // TODO: Serialize BackgroundTexture
    value["gradientStartColor"] = ColorToJson(GradientStartColor);
    value["gradientEndColor"] = ColorToJson(GradientEndColor);
    value["gradientType"] = GradientTypeToString(GradientType);
    value["gradientAngle"] = GradientAngle;

    // Border
    value["borderStyle"] = BorderStyleToString(BorderStyle);
    value["borderWidth"] = BorderWidth;
    value["borderRadius"] = BorderRadius;

    // Text
    value["fontFamily"] = FontFamily;
    value["fontSize"] = FontSize;
    value["fontBold"] = FontBold;
    value["fontItalic"] = FontItalic;
    value["fontUnderline"] = FontUnderline;

    // Effects
    value["opacity"] = Opacity;
    value["blurRadius"] = BlurRadius;
    value["shadowOffset"] = Vec2ToJson(ShadowOffset);
    value["shadowColor"] = ColorToJson(ShadowColor);

    // State colors
    value["hoverBackgroundColor"] = ColorToJson(HoverBackgroundColor);
    value["pressedBackgroundColor"] = ColorToJson(PressedBackgroundColor);
    value["disabledBackgroundColor"] = ColorToJson(DisabledBackgroundColor);
    value["hoverBorderColor"] = ColorToJson(HoverBorderColor);
    value["pressedBorderColor"] = ColorToJson(PressedBorderColor);
    value["disabledBorderColor"] = ColorToJson(DisabledBorderColor);
    value["hoverTextColor"] = ColorToJson(HoverTextColor);
    value["pressedTextColor"] = ColorToJson(PressedTextColor);
    value["disabledTextColor"] = ColorToJson(DisabledTextColor);

    // Transitions
    value["transitionDuration"] = TransitionDuration;
    value["enableTransitions"] = EnableTransitions;

    // Inheritance flags
    SerializeInheritanceFlags(value);

    return value;
}

bool UIStyle::Deserialize(const Json::Value& value) {
    // Colors
    BackgroundColor = ColorFromJson(value["backgroundColor"]);
    BorderColor = ColorFromJson(value["borderColor"]);
    TextColor = ColorFromJson(value["textColor"]);

    // Background
    BackgroundFill = BackgroundFillFromString(value["backgroundFill"].asString());
    // TODO: Deserialize BackgroundTexture
    GradientStartColor = ColorFromJson(value["gradientStartColor"]);
    GradientEndColor = ColorFromJson(value["gradientEndColor"]);
    GradientType = GradientTypeFromString(value["gradientType"].asString());
    GradientAngle = value.get("gradientAngle", 0.0f).asFloat();

    // Border
    BorderStyle = BorderStyleFromString(value["borderStyle"].asString());
    BorderWidth = value.get("borderWidth", 1.0f).asFloat();
    BorderRadius = value.get("borderRadius", 0.0f).asFloat();

    // Text
    FontFamily = value.get("fontFamily", "Arial").asString();
    FontSize = value.get("fontSize", 12.0f).asFloat();
    FontBold = value.get("fontBold", false).asBool();
    FontItalic = value.get("fontItalic", false).asBool();
    FontUnderline = value.get("fontUnderline", false).asBool();

    // Effects
    Opacity = value.get("opacity", 1.0f).asFloat();
    BlurRadius = value.get("blurRadius", 0.0f).asFloat();
    ShadowOffset = Vec2FromJson(value["shadowOffset"]);
    ShadowColor = ColorFromJson(value["shadowColor"]);

    // State colors
    HoverBackgroundColor = ColorFromJson(value["hoverBackgroundColor"]);
    PressedBackgroundColor = ColorFromJson(value["pressedBackgroundColor"]);
    DisabledBackgroundColor = ColorFromJson(value["disabledBackgroundColor"]);
    HoverBorderColor = ColorFromJson(value["hoverBorderColor"]);
    PressedBorderColor = ColorFromJson(value["pressedBorderColor"]);
    DisabledBorderColor = ColorFromJson(value["disabledBorderColor"]);
    HoverTextColor = ColorFromJson(value["hoverTextColor"]);
    PressedTextColor = ColorFromJson(value["pressedTextColor"]);
    DisabledTextColor = ColorFromJson(value["disabledTextColor"]);

    // Transitions
    TransitionDuration = value.get("transitionDuration", 0.2f).asFloat();
    EnableTransitions = value.get("enableTransitions", true).asBool();

    // Inheritance flags
    DeserializeInheritanceFlags(value);

    return true;
}

// UITheme implementation
UITheme::UITheme(const std::string& name)
    : m_Name(name)
{
}

void UITheme::RegisterStyle(const std::string& name, UIStylePtr style)
{
    m_Styles[name] = style;
}

UIStylePtr UITheme::GetStyle(const std::string& name) const
{
    auto it = m_Styles.find(name);
    if (it != m_Styles.end())
        return it->second;
    
    if (m_ParentTheme)
        return m_ParentTheme->GetStyle(name);
    
    return nullptr;
}

bool UITheme::HasStyle(const std::string& name) const
{
    return m_Styles.find(name) != m_Styles.end() || 
           (m_ParentTheme && m_ParentTheme->HasStyle(name));
}

void UITheme::RemoveStyle(const std::string& name)
{
    m_Styles.erase(name);
}

void UITheme::SetColor(const std::string& name, const glm::vec4& color)
{
    m_Colors[name] = color;
}

glm::vec4 UITheme::GetColor(const std::string& name) const
{
    auto it = m_Colors.find(name);
    if (it != m_Colors.end())
        return it->second;
    
    if (m_ParentTheme)
        return m_ParentTheme->GetColor(name);
    
    return glm::vec4(0.0f);
}

bool UITheme::SaveToFile(const std::string& filename) const
{
    Json::Value root;
    root["name"] = m_Name;

    // Save colors
    Json::Value colors;
    for (const auto& [name, color] : m_Colors)
    {
        colors[name] = UIStyle::ColorToJson(color);
    }
    root["colors"] = colors;

    // Save styles
    Json::Value styles;
    for (const auto& [name, style] : m_Styles)
    {
        styles[name] = style->Serialize();
    }
    root["styles"] = styles;

    // Write to file
    Json::StreamWriterBuilder builder;
    builder["commentStyle"] = "None";
    builder["indentation"] = "    ";
    std::unique_ptr<Json::StreamWriter> writer(builder.newStreamWriter());
    std::ofstream file(filename);
    if (!file.is_open())
        return false;
    
    return writer->write(root, &file) == 0;
}

bool UITheme::LoadFromFile(const std::string& filename)
{
    std::ifstream file(filename);
    if (!file.is_open())
        return false;

    Json::Value root;
    Json::CharReaderBuilder builder;
    std::string errs;
    if (!Json::parseFromStream(builder, file, &root, &errs))
        return false;

    m_Name = root["name"].asString();

    // Load colors
    const Json::Value& colors = root["colors"];
    for (const auto& name : colors.getMemberNames())
    {
        m_Colors[name] = UIStyle::ColorFromJson(colors[name]);
    }

    // Load styles
    const Json::Value& styles = root["styles"];
    for (const auto& name : styles.getMemberNames())
    {
        auto style = std::make_shared<UIStyle>();
        if (style->Deserialize(styles[name]))
        {
            m_Styles[name] = style;
        }
    }

    return true;
}

UIThemePtr UITheme::CreateDefault()
{
    auto theme = std::make_shared<UITheme>("Default");

    // Set default colors
    theme->SetColor("background", glm::vec4(0.2f, 0.2f, 0.2f, 1.0f));
    theme->SetColor("text", glm::vec4(1.0f));
    theme->SetColor("border", glm::vec4(0.3f, 0.3f, 0.3f, 1.0f));
    theme->SetColor("accent", glm::vec4(0.0f, 0.5f, 1.0f, 1.0f));
    theme->SetColor("warning", glm::vec4(1.0f, 0.5f, 0.0f, 1.0f));
    theme->SetColor("error", glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));
    theme->SetColor("success", glm::vec4(0.0f, 1.0f, 0.0f, 1.0f));

    // Register default styles
    theme->RegisterStyle("default", UIStyle::CreateDefault());
    theme->RegisterStyle("button", UIStyle::CreateButton());
    theme->RegisterStyle("panel", UIStyle::CreatePanel());
    theme->RegisterStyle("text", UIStyle::CreateText());

    return theme;
}

// UIStyle implementation
void UIStyle::SetParentStyle(UIStylePtr parent)
{
    m_ParentStyle = parent;
}

UIStylePtr UIStyle::GetEffectiveStyle() const
{
    if (!m_ParentStyle)
        return Clone();

    auto effective = m_ParentStyle->GetEffectiveStyle();

    // Inherit properties based on flags
    if (!m_InheritBackgroundColor)
    {
        effective->BackgroundColor = BackgroundColor;
        effective->HoverBackgroundColor = HoverBackgroundColor;
        effective->PressedBackgroundColor = PressedBackgroundColor;
        effective->DisabledBackgroundColor = DisabledBackgroundColor;
    }

    if (!m_InheritBorderColor)
    {
        effective->BorderColor = BorderColor;
        effective->HoverBorderColor = HoverBorderColor;
        effective->PressedBorderColor = PressedBorderColor;
        effective->DisabledBorderColor = DisabledBorderColor;
    }

    if (!m_InheritTextColor)
    {
        effective->TextColor = TextColor;
        effective->HoverTextColor = HoverTextColor;
        effective->PressedTextColor = PressedTextColor;
        effective->DisabledTextColor = DisabledTextColor;
    }

    if (!m_InheritFont)
    {
        effective->FontFamily = FontFamily;
        effective->FontSize = FontSize;
        effective->FontBold = FontBold;
        effective->FontItalic = FontItalic;
        effective->FontUnderline = FontUnderline;
    }

    if (!m_InheritEffects)
    {
        effective->Opacity = Opacity;
        effective->BlurRadius = BlurRadius;
        effective->ShadowOffset = ShadowOffset;
        effective->ShadowColor = ShadowColor;
    }

    if (!m_InheritTransitions)
    {
        effective->TransitionDuration = TransitionDuration;
        effective->EnableTransitions = EnableTransitions;
    }

    return effective;
}

// UIStyleManager implementation
UIStyleManager& UIStyleManager::Get()
{
    static UIStyleManager instance;
    return instance;
}

void UIStyleManager::RegisterTheme(UIThemePtr theme)
{
    if (theme)
        m_Themes[theme->GetName()] = theme;
}

UIThemePtr UIStyleManager::GetTheme(const std::string& name) const
{
    auto it = m_Themes.find(name);
    return it != m_Themes.end() ? it->second : nullptr;
}

void UIStyleManager::SetActiveTheme(const std::string& name)
{
    auto theme = GetTheme(name);
    if (theme)
        m_ActiveTheme = theme;
}

UIStylePtr UIStyleManager::GetStyle(const std::string& name) const
{
    if (m_ActiveTheme)
        return m_ActiveTheme->GetStyle(name);
    return nullptr;
}

bool UIStyleManager::LoadThemeFromFile(const std::string& filename)
{
    auto theme = std::make_shared<UITheme>();
    if (!theme->LoadFromFile(filename))
        return false;

    RegisterTheme(theme);
    return true;
}

bool UIStyleManager::SaveThemeToFile(const std::string& name, const std::string& filename) const
{
    auto theme = GetTheme(name);
    if (!theme)
        return false;

    return theme->SaveToFile(filename);
}

// Existing UIStyle implementation...
// (Keep the existing UIStyle::Clone, Lerp, CreateDefault, CreateButton, CreatePanel, and CreateText methods)

}} // namespace VGE::UI


