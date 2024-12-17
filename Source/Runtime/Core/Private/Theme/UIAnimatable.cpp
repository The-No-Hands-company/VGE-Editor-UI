#include "Runtime/Core/Public/Theme/UIAnimatable.h"

namespace VGE {
namespace Core {

UIAnimatable::UIAnimatable()
    : m_Animator(std::make_unique<UIAnimator>()) {}

UIAnimatable::~UIAnimatable() {}

void UIAnimatable::StartAnimation(const std::string& property, float startValue,
                                float endValue, float duration, UIEaseType easeType) {
    m_Animator->Animate(property, startValue, endValue, duration, easeType);
    m_Animator->SetUpdateCallback(property,
        [this, property](float value) { HandlePropertyUpdate(property, value); });
    m_Animator->SetCompleteCallback(property,
        [this, property]() { HandlePropertyComplete(property); });
}

void UIAnimatable::AnimateTo(const std::string& property, float endValue,
                           float duration, UIEaseType easeType) {
    m_Animator->AnimateTo(property, endValue, duration, easeType);
    m_Animator->SetUpdateCallback(property,
        [this, property](float value) { HandlePropertyUpdate(property, value); });
    m_Animator->SetCompleteCallback(property,
        [this, property]() { HandlePropertyComplete(property); });
}

void UIAnimatable::StopAnimation(const std::string& property) {
    m_Animator->Stop(property);
}

void UIAnimatable::StopAllAnimations() {
    m_Animator->StopAll();
}

bool UIAnimatable::IsAnimating(const std::string& property) const {
    return m_Animator->IsAnimating(property);
}

void UIAnimatable::FadeIn(float duration) {
    StartAnimation("opacity", 0.0f, 1.0f, duration);
}

void UIAnimatable::FadeOut(float duration) {
    StartAnimation("opacity", 1.0f, 0.0f, duration);
}

void UIAnimatable::SlideIn(const glm::vec2& from, float duration) {
    StartAnimation("positionX", from.x, 0.0f, duration);
    StartAnimation("positionY", from.y, 0.0f, duration);
}

void UIAnimatable::SlideOut(const glm::vec2& to, float duration) {
    StartAnimation("positionX", 0.0f, to.x, duration);
    StartAnimation("positionY", 0.0f, to.y, duration);
}

void UIAnimatable::Scale(const glm::vec2& from, const glm::vec2& to, float duration) {
    StartAnimation("scaleX", from.x, to.x, duration);
    StartAnimation("scaleY", from.y, to.y, duration);
}

void UIAnimatable::Rotate(float fromAngle, float toAngle, float duration) {
    StartAnimation("rotation", fromAngle, toAngle, duration);
}

void UIAnimatable::PlayClip(const std::string& name, bool loop) {
    m_Animator->PlayClip(name, loop);
}

void UIAnimatable::StopClip(const std::string& name) {
    m_Animator->StopClip(name);
}

void UIAnimatable::AddClip(std::shared_ptr<UIAnimationClip> clip) {
    m_Animator->AddClip(clip);
}

void UIAnimatable::RemoveClip(const std::string& name) {
    m_Animator->RemoveClip(name);
}

void UIAnimatable::UpdateAnimations(float deltaTime) {
    m_Animator->Update(deltaTime);
}

void UIAnimatable::HandlePropertyUpdate(const std::string& property, float value) {
    OnPropertyAnimationUpdate(property, value);
}

void UIAnimatable::HandlePropertyComplete(const std::string& property) {
    OnPropertyAnimationComplete(property);
}

} // namespace Core
} // namespace VGE 