#include "Runtime/Core/Public/Theme/UIAnimationPresets.h"
#include "Runtime/Core/Public/Theme/UIAnimationTemplates.h"

namespace VGE {
namespace Core {

// Button State Machine
std::shared_ptr<UIAnimationStateMachine> UIAnimationPresets::CreateButtonStateMachine() {
    auto machine = std::make_shared<UIAnimationStateMachine>();
    
    // Add states
    machine->AddState("Normal", std::make_shared<UIAnimationState>("Normal"));
    machine->AddState("Hover", CreateButtonHoverState());
    machine->AddState("Press", CreateButtonPressState());
    machine->AddState("Disabled", CreateButtonDisabledState());
    
    // Add transitions
    machine->AddTransition(CreateFadeTransition("Normal", "Hover", 0.2f));
    machine->AddTransition(CreateFadeTransition("Hover", "Normal", 0.2f));
    machine->AddTransition(CreateScaleTransition("Hover", "Press", 0.1f));
    machine->AddTransition(CreateScaleTransition("Press", "Hover", 0.1f));
    machine->AddTransition(CreateFadeTransition("Normal", "Disabled", 0.3f));
    machine->AddTransition(CreateFadeTransition("Disabled", "Normal", 0.3f));
    
    machine->SetDefaultState("Normal");
    return machine;
}

std::shared_ptr<UIAnimationState> UIAnimationPresets::CreateButtonHoverState() {
    auto state = std::make_shared<UIAnimationState>("Hover");
    auto blendTree = CreateHoverBlendTree();
    
    // Add hover animations
    auto scaleClip = UIAnimationTemplates::CreateScale(
        glm::vec2(1.0f), glm::vec2(1.05f), 0.2f);
    auto glowClip = UIAnimationTemplates::CreateAttentionGrab(0.2f);
    
    blendTree->AddAnimation("Scale", scaleClip, {1.0f});
    blendTree->AddAnimation("Glow", glowClip, {1.0f});
    
    state->SetBlendTree(blendTree);
    return state;
}

std::shared_ptr<UIAnimationState> UIAnimationPresets::CreateButtonPressState() {
    auto state = std::make_shared<UIAnimationState>("Press");
    auto blendTree = CreatePressBlendTree();
    
    // Add press animations
    auto scaleClip = UIAnimationTemplates::CreateScale(
        glm::vec2(1.05f), glm::vec2(0.95f), 0.1f);
    auto colorClip = UIAnimationTemplates::CreateColorShift(
        glm::vec4(1.0f), glm::vec4(0.8f, 0.8f, 0.8f, 1.0f), 0.1f);
    
    blendTree->AddAnimation("Scale", scaleClip, {1.0f});
    blendTree->AddAnimation("Color", colorClip, {1.0f});
    
    state->SetBlendTree(blendTree);
    return state;
}

std::shared_ptr<UIAnimationState> UIAnimationPresets::CreateButtonDisabledState() {
    auto state = std::make_shared<UIAnimationState>("Disabled");
    auto blendTree = CreateFadeBlendTree();
    
    // Add disabled animations
    auto fadeClip = UIAnimationTemplates::CreateFade(1.0f, 0.5f, 0.3f);
    auto desaturateClip = UIAnimationTemplates::CreateDesaturate(0.3f);
    
    blendTree->AddAnimation("Fade", fadeClip, {1.0f});
    blendTree->AddAnimation("Desaturate", desaturateClip, {1.0f});
    
    state->SetBlendTree(blendTree);
    return state;
}

// Window State Machine
std::shared_ptr<UIAnimationStateMachine> UIAnimationPresets::CreateWindowStateMachine() {
    auto machine = std::make_shared<UIAnimationStateMachine>();
    
    // Add states
    machine->AddState("Closed", std::make_shared<UIAnimationState>("Closed"));
    machine->AddState("Opening", CreateWindowOpenState());
    machine->AddState("Open", std::make_shared<UIAnimationState>("Open"));
    machine->AddState("Minimized", CreateWindowMinimizeState());
    machine->AddState("Maximized", CreateWindowMaximizeState());
    
    // Add transitions
    machine->AddTransition(CreateScaleTransition("Closed", "Opening", 0.3f));
    machine->AddTransition(CreateFadeTransition("Opening", "Open", 0.2f));
    machine->AddTransition(CreateSlideTransition("Open", "Minimized", 0.3f));
    machine->AddTransition(CreateSlideTransition("Minimized", "Open", 0.3f));
    machine->AddTransition(CreateScaleTransition("Open", "Maximized", 0.3f));
    machine->AddTransition(CreateScaleTransition("Maximized", "Open", 0.3f));
    
    machine->SetDefaultState("Closed");
    return machine;
}

std::shared_ptr<UIAnimationState> UIAnimationPresets::CreateWindowMinimizeState() {
    auto state = std::make_shared<UIAnimationState>("Minimize");
    auto blendTree = CreateSlideBlendTree();
    
    // Add minimize animations
    auto scaleClip = UIAnimationTemplates::CreateScale(
        glm::vec2(1.0f), glm::vec2(0.1f, 0.1f), 0.3f);
    auto slideClip = UIAnimationTemplates::CreateSlide(
        glm::vec2(0.0f), glm::vec2(0.0f, 1.0f), 0.3f);
    
    blendTree->AddAnimation("Scale", scaleClip, {1.0f});
    blendTree->AddAnimation("Slide", slideClip, {1.0f});
    
    state->SetBlendTree(blendTree);
    return state;
}

std::shared_ptr<UIAnimationState> UIAnimationPresets::CreateWindowMaximizeState() {
    auto state = std::make_shared<UIAnimationState>("Maximize");
    auto blendTree = CreateSlideBlendTree();
    
    // Add maximize animations
    auto scaleClip = UIAnimationTemplates::CreateScale(
        glm::vec2(1.0f), glm::vec2(1.0f), 0.3f);
    auto expandClip = UIAnimationTemplates::CreateExpand(
        glm::vec4(0.0f), glm::vec4(0.0f), 0.3f);
    
    blendTree->AddAnimation("Scale", scaleClip, {1.0f});
    blendTree->AddAnimation("Expand", expandClip, {1.0f});
    
    state->SetBlendTree(blendTree);
    return state;
}

// Dialog State Machine
std::shared_ptr<UIAnimationStateMachine> UIAnimationPresets::CreateDialogStateMachine() {
    auto machine = std::make_shared<UIAnimationStateMachine>();
    
    // Add states
    machine->AddState("Hidden", std::make_shared<UIAnimationState>("Hidden"));
    machine->AddState("Showing", CreateDialogShowState());
    machine->AddState("Visible", std::make_shared<UIAnimationState>("Visible"));
    machine->AddState("Hiding", CreateDialogHideState());
    machine->AddState("Shaking", CreateDialogShakeState());
    
    // Add transitions with blend times
    auto showTransition = CreateScaleTransition("Hidden", "Showing", 0.3f);
    showTransition->SetBlendTime(0.1f);
    machine->AddTransition(showTransition);
    
    auto hideTransition = CreateFadeTransition("Visible", "Hiding", 0.3f);
    hideTransition->SetBlendTime(0.1f);
    machine->AddTransition(hideTransition);
    
    auto shakeTransition = std::make_shared<UIAnimationStateTransition>("Visible", "Shaking");
    shakeTransition->SetDuration(0.5f);
    machine->AddTransition(shakeTransition);
    
    machine->SetDefaultState("Hidden");
    return machine;
}

// Helper Methods
std::shared_ptr<UIAnimationBlendTree> UIAnimationPresets::CreateHoverBlendTree() {
    auto tree = std::make_shared<UIAnimationBlendTree>();
    
    // Add default parameters
    tree->SetParameter(0, 0.0f); // Hover progress
    tree->SetParameter(1, 0.0f); // Distance from center
    
    return tree;
}

std::shared_ptr<UIAnimationStateTransition> UIAnimationPresets::CreateFadeTransition(
    const std::string& from, const std::string& to, float duration) {
    auto transition = std::make_shared<UIAnimationStateTransition>(from, to);
    transition->SetDuration(duration);
    transition->SetBlendTime(duration * 0.5f);
    return transition;
}

std::shared_ptr<UIAnimationStateTransition> UIAnimationPresets::CreateSlideTransition(
    const std::string& from, const std::string& to, float duration) {
    auto transition = std::make_shared<UIAnimationStateTransition>(from, to);
    transition->SetDuration(duration);
    transition->SetBlendTime(duration * 0.3f);
    return transition;
}

std::shared_ptr<UIAnimationStateTransition> UIAnimationPresets::CreateScaleTransition(
    const std::string& from, const std::string& to, float duration) {
    auto transition = std::make_shared<UIAnimationStateTransition>(from, to);
    transition->SetDuration(duration);
    transition->SetBlendTime(duration * 0.2f);
    return transition;
}

std::shared_ptr<UIAnimationBlendTree> UIAnimationPresets::CreatePressBlendTree() {
    auto tree = std::make_shared<UIAnimationBlendTree>();
    
    // Add press-specific parameters
    tree->SetParameter(0, 0.0f); // Press progress
    tree->SetParameter(1, 0.0f); // Press intensity
    
    return tree;
}

std::shared_ptr<UIAnimationBlendTree> UIAnimationPresets::CreateSlideBlendTree() {
    auto tree = std::make_shared<UIAnimationBlendTree>();
    
    // Add slide-specific parameters
    tree->SetParameter(0, 0.0f); // Slide progress
    tree->SetParameter(1, 0.0f); // Slide direction
    
    return tree;
}

std::shared_ptr<UIAnimationBlendTree> UIAnimationPresets::CreateFadeBlendTree() {
    auto tree = std::make_shared<UIAnimationBlendTree>();
    
    // Add fade-specific parameters
    tree->SetParameter(0, 0.0f); // Fade progress
    tree->SetParameter(1, 0.0f); // Fade intensity
    
    return tree;
}

std::shared_ptr<UIAnimationState> UIAnimationPresets::CreateMenuExpandState() {
    auto state = std::make_shared<UIAnimationState>("Expand");
    auto blendTree = CreateSlideBlendTree();
    
    // Add expand animations
    auto heightClip = UIAnimationTemplates::CreateHeightExpand(0.0f, 1.0f, 0.3f);
    auto fadeClip = UIAnimationTemplates::CreateFade(0.0f, 1.0f, 0.3f);
    
    blendTree->AddAnimation("Height", heightClip, {1.0f});
    blendTree->AddAnimation("Fade", fadeClip, {1.0f});
    
    state->SetBlendTree(blendTree);
    return state;
}

std::shared_ptr<UIAnimationState> UIAnimationPresets::CreateMenuCollapseState() {
    auto state = std::make_shared<UIAnimationState>("Collapse");
    auto blendTree = CreateSlideBlendTree();
    
    // Add collapse animations
    auto heightClip = UIAnimationTemplates::CreateHeightExpand(1.0f, 0.0f, 0.3f);
    auto fadeClip = UIAnimationTemplates::CreateFade(1.0f, 0.0f, 0.3f);
    
    blendTree->AddAnimation("Height", heightClip, {1.0f});
    blendTree->AddAnimation("Fade", fadeClip, {1.0f});
    
    state->SetBlendTree(blendTree);
    return state;
}

std::shared_ptr<UIAnimationState> UIAnimationPresets::CreateLoadingSpinState() {
    auto state = std::make_shared<UIAnimationState>("Spin");
    
    // Create a continuous rotation animation
    auto rotateClip = UIAnimationTemplates::CreateRotation(
        0.0f, 360.0f, 1.0f);
    
    state->AddClip(rotateClip);
    state->SetLooping(true);
    state->SetSpeed(1.0f);
    
    return state;
}

std::shared_ptr<UIAnimationState> UIAnimationPresets::CreateLoadingPulseState() {
    auto state = std::make_shared<UIAnimationState>("Pulse");
    
    // Create a pulsing scale and opacity animation
    auto sequence = std::make_shared<UIAnimationSequence>();
    
    auto scaleClip = UIAnimationTemplates::CreateScale(
        glm::vec2(0.8f), glm::vec2(1.2f), 0.5f);
    auto fadeClip = UIAnimationTemplates::CreateFade(0.5f, 1.0f, 0.5f);
    
    sequence->AddClip(scaleClip, 0.0f);
    sequence->AddClip(fadeClip, 0.0f);
    
    state->AddSequence(sequence);
    state->SetLooping(true);
    
    return state;
}

} // namespace Core
} // namespace VGE 