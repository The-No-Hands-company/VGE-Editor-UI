#pragma once

#include "Runtime/Core/Public/Core/UICore.h"
#include "Runtime/Core/Public/Widget/UIWidget.h"

#include <string>
#include <vector>
#include <memory>
#include <functional>

namespace VGE {
namespace UI {

struct TabItem {
    std::string Name;
    std::string Icon;  // Path to icon or icon identifier
    bool IsActive = false;
    bool IsHovered = false;
    bool CanClose = true;
    std::shared_ptr<UIWidget> Content;
};

class UI_API UITabControl : public UIWidget {
public:
    UITabControl(const std::string& label = "");
    virtual ~UITabControl() = default;

    virtual void Update() override;
    virtual void Draw() override;
    virtual bool HandleEvent(Event& event) override;

    // Tab management
    int AddTab(const std::string& name, std::shared_ptr<UIWidget> content = nullptr);
    void RemoveTab(int index);
    void RemoveTab(const std::string& name);
    void ClearTabs();
    void MoveTab(int fromIndex, int toIndex);

    // Tab access
    TabItem* GetTab(int index);
    TabItem* GetTab(const std::string& name);
    TabItem* GetActiveTab() { retur