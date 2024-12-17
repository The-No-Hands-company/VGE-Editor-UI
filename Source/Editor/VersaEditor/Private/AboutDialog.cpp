// AboutDia

#include "Runtime/Core/Public/Core/UICore.h"log implementation
// Force rebuild: 2024-02-20 14:30:00
#include "Dialogs/AboutDialog.h"
#include "Runtime/UI/Public/Controls/UIText.h"
#include "Runtime/UI/Public/Controls/UIButton.h"
#include "Runtime/UI/Public/Controls/UIPanel.h"
#include "Runtime/Core/Public/Core/UITypes.h"
#include <GLFW/glfw3.h>
#include <iostream>

namespace VGE {
namespace UI {

AboutDialog::AboutDialog()
    : UIDialog("About")
{
    SetSize(glm::vec2(400.0f, 300.0f));
    
    // Create title text
    auto titleText = std::make_shared<UIText>("Versa Engine");
    titleText->SetFontSize(24.0f);
    titleText->SetColor(Color(1.0f, 1.0f, 1.0f, 1.0f));
    titleText->SetAlignment(TextAlignment::Center, TextAlignment::Top);
    titleText->SetPosition(glm::vec2(0.0f, 20.0f));
    titleText->SetSize(glm::vec2(GetSize().x, 30.0f));
    AddChild(titleText);

    // Create version text
    auto versionText = std::make_shared<UIText>("Version 1.0.0");
    versionText->SetFontSize(14.0f);
    versionText->SetColor(Color(0.8f, 0.8f, 0.8f, 1.0f));
    versionText->SetAlignment(TextAlignment::Center, TextAlignment::Top);
    versionText->SetPosition(glm::vec2(0.0f, 60.0f));
    versionText->SetSize(glm::vec2(GetSize().x, 20.0f));
    AddChild(versionText);

    // Create description text
    auto descriptionText = std::make_shared<UIText>(
        "Versa Engine is a modern, flexible real-time engine.\n"
        "Built with performance and ease of use in mind. Created by The No Hands Company"
    );
    descriptionText->SetFontSize(12.0f);
    descriptionText->SetColor(Color(0.7f, 0.7f, 0.7f, 1.0f));
    descriptionText->SetAlignment(TextAlignment::Center, TextAlignment::Top);
    descriptionText->SetPosition(glm::vec2(20.0f, 100.0f));
    descriptionText->SetSize(glm::vec2(GetSize().x - 40.0f, 60.0f));
    AddChild(descriptionText);

    // Create OK button
    auto okButton = std::make_shared<UIButton>("OK");
    okButton->SetPosition(glm::vec2((GetSize().x - 100.0f) * 0.5f, GetSize().y - 50.0f));
    okButton->SetSize(glm::vec2(100.0f, 30.0f));
    okButton->SetClickCallback([this]() { Close(); });
    AddChild(okButton);

    // Center the dialog
    CenterInParent();
}

void AboutDialog::OnUpdate(float deltaTime) {
    UIDialog::OnUpdate(deltaTime);
}

void AboutDialog::CenterInParent() {
    if (auto parent = GetParent()) {
        auto parentSize = parent->GetSize();
        auto dialogSize = GetSize();
        SetPosition({
            (parentSize.x - dialogSize.x) / 2.0f,
            (parentSize.y - dialogSize.y) / 2.0f
        });
    }
}

void AboutDialog::Show() {
    SetVisible(true);
}

void AboutDialog::Close() {
    SetVisible(false);
}

} // namespace UI
} // namespace VGE



