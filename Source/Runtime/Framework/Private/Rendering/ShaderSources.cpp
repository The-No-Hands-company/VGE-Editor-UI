#pragma once

#include "Runtime/Core/Public/Core/UICore.h"
#include "Runtime/Core/Public/Widget/UIWidget.h"
#include "Runtime/Core/Public/Math/Math.h"

#include <functional>

namespace VGE {
namespace UI {

struct ScrollViewProperties {
    bool EnableHorizontal = true;
    bool EnableVertical = true;
    bool ShowHorizontalScrollbar = true;
    bool ShowVerticalScrollbar = true;
    bool EnableMomentum = true;
    bool EnableSmoothScrolling = true;
    float ScrollSpeed = 1.0f;
    float MomentumDecay = 0.95f;
    float SmoothScrollingSpeed = 10.0f;
    float ScrollbarWidth = 12.0f;
    float MinimumScrollbarLength = 30.0f;
};

class UI_API UIScrollView : public UIWidget {
public:
    UIScrollView();
    virtual ~UIScrollView() = default;

    virtual void Initialize() override;
    virtual void Update(float deltaTime) override;
    virtual void Render() override;

    // Content management
    void SetContent(const Ref<UIWidget>& content);
    Ref<UIWidget> GetCont