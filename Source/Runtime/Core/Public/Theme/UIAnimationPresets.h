#pragma once

#include "Runtime/Core/Public/Theme/UIAnimationState.h"
#include <memory>

namespace VGE {
namespace Core {

/**
 * @class UIAnimationPresets
 * @brief Provides pre-configured animation states and transitions for UI components.
 */
class UIAnimationPresets {
public:
    // Button Animations
    static std::shared_ptr<UIAnimationStateMachine> CreateButtonStateMachine();
    static std::shared_ptr<UIAnimationState> CreateButtonHoverState();
    static std::shared_ptr<UIAnimationState> CreateButtonPressState();
    static std::shared_ptr<UIAnimationState> CreateButtonDisabledState();

    // Window Animations
    static std::shared_ptr<UIAnimationStateMachine> CreateWindowStateMachine();
    static std::shared_ptr<UIAnimationState> CreateWindowOpenState();
    static std::shared_ptr<UIAnimationState> CreateWindowCloseState();
    static std::shared_ptr<UIAnimationState> CreateWindowMinimizeState();
    static std::shared_ptr<UIAnimationState> CreateWindowMaximizeState();

    // Menu Animations
    static std::shared_ptr<UIAnimationStateMachine> CreateMenuStateMachine();
    static std::shared_ptr<UIAnimationState> CreateMenuExpandState();
    static std::shared_ptr<UIAnimationState> CreateMenuCollapseState();
    static std::shared_ptr<UIAnimationState> CreateMenuItemHoverState();

    // Panel Animations
    static std::shared_ptr<UIAnimationStateMachine> CreatePanelStateMachine();
    static std::shared_ptr<UIAnimationState> CreatePanelSlideInState();
    static std::shared_ptr<UIAnimationState> CreatePanelSlideOutState();
    static std::shared_ptr<UIAnimationState> CreatePanelFadeState();

    // Tooltip Animations
    static std::shared_ptr<UIAnimationStateMachine> CreateTooltipStateMachine();
    static std::shared_ptr<UIAnimationState> CreateTooltipShowState();
    static std::shared_ptr<UIAnimationState> CreateTooltipHideState();

    // Dialog Animations
    static std::shared_ptr<UIAnimationStateMachine> CreateDialogStateMachine();
    static std::shared_ptr<UIAnimationState> CreateDialogShowState();
    static std::shared_ptr<UIAnimationState> CreateDialogHideState();
    static std::shared_ptr<UIAnimationState> CreateDialogShakeState();

    // Notification Animations
    static std::shared_ptr<UIAnimationStateMachine> CreateNotificationStateMachine();
    static std::shared_ptr<UIAnimationState> CreateNotificationEnterState();
    static std::shared_ptr<UIAnimationState> CreateNotificationExitState();
    static std::shared_ptr<UIAnimationState> CreateNotificationPulseState();

    // Tab Animations
    static std::shared_ptr<UIAnimationStateMachine> CreateTabStateMachine();
    static std::shared_ptr<UIAnimationState> CreateTabSwitchState();
    static std::shared_ptr<UIAnimationState> CreateTabCloseState();

    // Scroll Animations
    static std::shared_ptr<UIAnimationStateMachine> CreateScrollStateMachine();
    static std::shared_ptr<UIAnimationState> CreateScrollInertiaState();
    static std::shared_ptr<UIAnimationState> CreateScrollBounceState();

    // Loading Animations
    static std::shared_ptr<UIAnimationStateMachine> CreateLoadingStateMachine();
    static std::shared_ptr<UIAnimationState> CreateLoadingSpinState();
    static std::shared_ptr<UIAnimationState> CreateLoadingPulseState();
    static std::shared_ptr<UIAnimationState> CreateLoadingProgressState();

private:
    // Helper methods for creating common animation patterns
    static std::shared_ptr<UIAnimationBlendTree> CreateHoverBlendTree();
    static std::shared_ptr<UIAnimationBlendTree> CreatePressBlendTree();
    static std::shared_ptr<UIAnimationBlendTree> CreateSlideBlendTree();
    static std::shared_ptr<UIAnimationBlendTree> CreateFadeBlendTree();
    
    // Helper methods for creating common transitions
    static std::shared_ptr<UIAnimationStateTransition> CreateFadeTransition(
        const std::string& from, const std::string& to, float duration = 0.3f);
    static std::shared_ptr<UIAnimationStateTransition> CreateSlideTransition(
        const std::string& from, const std::string& to, float duration = 0.3f);
    static std::shared_ptr<UIAnimationStateTransition> CreateScaleTransition(
        const std::string& from, const std::string& to, float duration = 0.3f);
};

} // namespace Core
} // namespace VGE 