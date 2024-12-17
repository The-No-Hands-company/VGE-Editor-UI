#include "Property/UIPropertyEditors.h"
#include "Theme/UIStyle.h"
#include "Controls/UIColorPicker.h"
#include "Controls/UIDialog.h"
#include <algorithm>

namespace VGE {
namespace UI {

bool UIArrayEditor::OnGUI(const std::string& name, std::any& value) {
    bool modified = false;
    auto& array = std::any_cast<std::vector<std::any>&>(value);
    
    if (CollapsingHeader(name.c_str())) {
        // Array size control
        int size = static_cast<int>(array.size());
        if (DragInt("Size", &size, 0.1f, 0, 1000)) {
            if (size < array.size()) {
                array.resize(size);
                modified = true;
            }
            else if (size > array.size() && m_CreateElement) {
                while (array.size() < size) {
                    auto newElement = m_CreateElement();
                    if (m_CustomizeElement) {
                        m_CustomizeElement(name + "[" + std::to_string(array.size()) + "]", newElement);
                    }
                    array.push_back(newElement);
                }
                modified = true;
            }
        }
        
        // Element editors
        for (size_t i = 0; i < array.size(); ++i) {
            PushID(static_cast<int>(i));
            
            // Element header with remove button
            BeginHorizontal();
            Text(("[" + std::to_string(i) + "]").c_str());
            if (Button("X")) {
                array.erase(array.begin() + i);
                modified = true;
                PopID();
                EndHorizontal();
                break;
            }
            EndHorizontal();
            
            // Element editor
            Indent();
            if (m_ElementEditor && m_ElementEditor->OnGUI("", array[i])) {
                modified = true;
            }
            Unindent();
            
            PopID();
        }
        
        // Add button
        if (m_CreateElement && Button("Add Element")) {
            auto newElement = m_CreateElement();
            if (m_CustomizeElement) {
                m_CustomizeElement(name + "[" + std::to_string(array.size()) + "]", newElement);
            }
            array.push_back(newElement);
            modified = true;
        }
    }
    
    return modified;
}

bool UIEnumEditor::OnGUI(const std::string& name, std::any& value) {
    auto names = m_GetNames();
    int currentIndex = std::any_cast<int>(value);
    
    if (currentIndex >= 0 && currentIndex < names.size()) {
        if (BeginCombo(name.c_str(), names[currentIndex].c_str())) {
            for (size_t i = 0; i < names.size(); ++i) {
                bool selected = (i == currentIndex);
                if (Selectable(names[i].c_str(), selected)) {
                    value = m_GetValue(names[i]);
                    EndCombo();
                    return true;
                }
            }
            EndCombo();
        }
    }
    
    return false;
}

bool UIFlagsEditor::OnGUI(const std::string& name, std::any& value) {
    bool modified = false;
    auto flags = m_GetFlags();
    int flagValue = std::any_cast<int>(value);
    
    if (CollapsingHeader(name.c_str())) {
        for (const auto& [flagName, flagBit] : flags) {
            bool isSet = (flagValue & flagBit) != 0;
            if (Checkbox(flagName.c_str(), &isSet)) {
                if (isSet) {
                    flagValue |= flagBit;
                } else {
                    flagValue &= ~flagBit;
                }
                value = flagValue;
                modified = true;
            }
        }
    }
    
    return modified;
}

bool UIColorEditor::OnGUI(const std::string& name, std::any& value) {
    auto colorValue = std::any_cast<glm::vec4>(value);
    
    if (ColorEdit4(name.c_str(), &colorValue.x)) {
        value = colorValue;
        return true;
    }
    
    return false;
}

bool UICurveEditor::OnGUI(const std::string& name, std::any& value) {
    auto& curve = std::any_cast<std::vector<glm::vec2>&>(value);
    bool modified = false;
    
    if (CollapsingHeader(name.c_str())) {
        // Curve preview
        const float previewHeight = 100.0f;
        auto drawList = GetWindowDrawList();
        auto curveMin = glm::vec2(FLT_MAX);
        auto curveMax = glm::vec2(-FLT_MAX);
        
        // Find curve bounds
        for (const auto& point : curve) {
            curveMin = glm::min(curveMin, point);
            curveMax = glm::max(curveMax, point);
        }
        
        // Draw curve
        auto cursorScreenPos = GetCursorScreenPos();
        auto contentRegionAvail = GetContentRegionAvail();
        auto previewRect = glm::vec4(
            cursorScreenPos.x,
            cursorScreenPos.y,
            contentRegionAvail.x,
            previewHeight
        );
        
        drawList->AddRect(
            glm::vec2(previewRect.x, previewRect.y),
            glm::vec2(previewRect.x + previewRect.z, previewRect.y + previewRect.w),
            GetColorU32(ImGuiCol_Border)
        );
        
        // Draw curve points and segments
        if (curve.size() > 1) {
            for (size_t i = 0; i < curve.size() - 1; ++i) {
                auto p1 = curve[i];
                auto p2 = curve[i + 1];
                
                // Transform points to screen space
                auto s1 = glm::vec2(
                    previewRect.x + (p1.x - curveMin.x) / (curveMax.x - curveMin.x) * previewRect.z,
                    previewRect.y + (1.0f - (p1.y - curveMin.y) / (curveMax.y - curveMin.y)) * previewRect.w
                );
                auto s2 = glm::vec2(
                    previewRect.x + (p2.x - curveMin.x) / (curveMax.x - curveMin.x) * previewRect.z,
                    previewRect.y + (1.0f - (p2.y - curveMin.y) / (curveMax.y - curveMin.y)) * previewRect.w
                );
                
                drawList->AddLine(s1, s2, GetColorU32(ImGuiCol_PlotLines), 2.0f);
            }
        }
        
        // Point editors
        for (size_t i = 0; i < curve.size(); ++i) {
            PushID(static_cast<int>(i));
            if (DragFloat2(("Point " + std::to_string(i)).c_str(), &curve[i].x)) {
                modified = true;
            }
            PopID();
        }
        
        // Add/Remove points
        if (Button("Add Point")) {
            if (curve.empty()) {
                curve.push_back(glm::vec2(0.0f, 0.0f));
            } else {
                curve.push_back(curve.back() + glm::vec2(0.1f, 0.0f));
            }
            modified = true;
        }
        
        SameLine();
        
        if (!curve.empty() && Button("Remove Point")) {
            curve.pop_back();
            modified = true;
        }
    }
    
    return modified;
}

bool UIGradientEditor::OnGUI(const std::string& name, std::any& value) {
    auto& gradient = std::any_cast<std::vector<std::pair<float, glm::vec4>>&>(value);
    bool modified = false;
    
    if (CollapsingHeader(name.c_str())) {
        // Gradient preview
        const float previewHeight = 20.0f;
        auto drawList = GetWindowDrawList();
        auto cursorScreenPos = GetCursorScreenPos();
        auto contentRegionAvail = GetContentRegionAvail();
        
        // Draw gradient
        if (!gradient.empty()) {
            for (float x = 0; x < contentRegionAvail.x; x += 1.0f) {
                float t = x / contentRegionAvail.x;
                glm::vec4 color;
                
                // Find surrounding stops
                size_t i = 0;
                while (i < gradient.size() && gradient[i].first < t) ++i;
                
                if (i == 0) {
                    color = gradient.front().second;
                }
                else if (i == gradient.size()) {
                    color = gradient.back().second;
                }
                else {
                    float t0 = gradient[i-1].first;
                    float t1 = gradient[i].first;
                    float alpha = (t - t0) / (t1 - t0);
                    color = glm::mix(gradient[i-1].second, gradient[i].second, alpha);
                }
                
                drawList->AddRectFilled(
                    glm::vec2(cursorScreenPos.x + x, cursorScreenPos.y),
                    glm::vec2(cursorScreenPos.x + x + 1, cursorScreenPos.y + previewHeight),
                    GetColorU32(color)
                );
            }
        }
        
        Dummy(glm::vec2(0, previewHeight));
        
        // Stop editors
        for (size_t i = 0; i < gradient.size(); ++i) {
            PushID(static_cast<int>(i));
            
            BeginHorizontal();
            
            // Position
            float position = gradient[i].first;
            if (DragFloat("Position", &position, 0.01f, 0.0f, 1.0f)) {
                gradient[i].first = position;
                std::sort(gradient.begin(), gradient.end());
                modified = true;
            }
            
            // Color
            SameLine();
            if (ColorEdit4("Color", &gradient[i].second.x)) {
                modified = true;
            }
            
            // Remove button
            SameLine();
            if (Button("X")) {
                gradient.erase(gradient.begin() + i);
                modified = true;
                PopID();
                EndHorizontal();
                break;
            }
            
            EndHorizontal();
            PopID();
        }
        
        // Add stop button
        if (Button("Add Stop")) {
            float position = gradient.empty() ? 0.0f : gradient.back().first + 0.1f;
            if (position > 1.0f) position = 1.0f;
            gradient.push_back({position, glm::vec4(1.0f)});
            std::sort(gradient.begin(), gradient.end());
            modified = true;
        }
    }
    
    return modified;
}

bool UIObjectReferenceEditor::OnGUI(const std::string& name, std::any& value) {
    auto& objRef = std::any_cast<std::string&>(value);
    
    BeginHorizontal();
    
    // Object name display/edit
    char buffer[256];
    strncpy(buffer, objRef.c_str(), sizeof(buffer));
    if (InputText(name.c_str(), buffer, sizeof(buffer))) {
        objRef = buffer;
        EndHorizontal();
        return true;
    }
    
    // Browse button
    SameLine();
    if (Button("...")) {
        // TODO: Show object picker dialog
        EndHorizontal();
        return true;
    }
    
    EndHorizontal();
    return false;
}

bool UIAssetReferenceEditor::OnGUI(const std::string& name, std::any& value) {
    auto& assetRef = std::any_cast<std::string&>(value);
    
    BeginHorizontal();
    
    // Asset path display/edit
    char buffer[256];
    strncpy(buffer, assetRef.c_str(), sizeof(buffer));
    if (InputText(name.c_str(), buffer, sizeof(buffer))) {
        assetRef = buffer;
        EndHorizontal();
        return true;
    }
    
    // Browse button
    SameLine();
    if (Button("...")) {
        // TODO: Show asset picker dialog
        EndHorizontal();
        return true;
    }
    
    EndHorizontal();
    return false;
}

bool UIPathEditor::OnGUI(const std::string& name, std::any& value) {
    auto& path = std::any_cast<std::string&>(value);
    
    BeginHorizontal();
    
    // Path display/edit
    char buffer[256];
    strncpy(buffer, path.c_str(), sizeof(buffer));
    if (InputText(name.c_str(), buffer, sizeof(buffer))) {
        path = buffer;
        EndHorizontal();
        return true;
    }
    
    // Browse button
    SameLine();
    if (Button("...")) {
        // TODO: Show file/directory picker dialog based on m_PathType
        EndHorizontal();
        return true;
    }
    
    EndHorizontal();
    return false;
}

bool UIKeyBindingEditor::OnGUI(const std::string& name, std::any& value) {
    auto& binding = std::any_cast<int&>(value);
    
    // TODO: Implement key binding editor with proper key capture
    char buffer[32];
    sprintf(buffer, "Key: %d", binding);
    if (Button(buffer)) {
        // TODO: Start key capture mode
        return true;
    }
    
    return false;
}

bool UICodeEditor::OnGUI(const std::string& name, std::any& value) {
    auto& code = std::any_cast<std::string&>(value);
    
    // TODO: Implement syntax highlighting based on m_Language
    return InputTextMultiline(name.c_str(), &code, glm::vec2(0, 200));
}

}} // namespace VGE::UI 