#pragma once

#include "Runtime/Core/Public/Theme/UIAnimation.h"
#include <memory>
#include <string>

namespace VGE {
namespace Core {

/**
 * @class UIAnimatable
 * @brief Interface for objects that can be animated.
 * 
 * Provides animation capabilities to UI components by managing an internal
 * animator and providing convenient animation methods.
 */
class UIAnimatable {
public:
    UIAnimatable();
    virtual ~UIAnimatable();

    // Basic Animation Control
    void StartAnimation(const std::string& property, float startValue,
                       float endValue, float duration,
                       UIEaseType easeType = UIEaseType::QuadOut);
    
    void AnimateTo(const std::string& property, float endValue,
                  float duration, UIEaseType easeType = UIEaseType::QuadOut);
    
    void StopAnimation(const std::string& property);
    void StopAllAnimations();
    bool IsAnimating(const std::string& property) const;

    // Predefined Animations
    void FadeIn(float duration = 0.3f);
    void FadeOut(float duration = 0.3f);
    void SlideIn(const glm::vec2& from, float duration = 0.3f);
    void SlideOut(const glm::vec2& to, float duration = 0.3f);
    void Scale(const glm::vec2& from, const glm::vec2& to, float duration = 0.3f);
    void Rotate(float fromAngle, float toAngle, float duration = 0.3f);

    // Animation Clips
    void PlayClip(const std::string& name, bool loop = false);
    void StopClip(const std::string& name);
    void AddClip(std::shared_ptr<UIAnimationClip> clip);
    void RemoveClip(const std::string& name);

    // Update Method (called by the UI system)
    void UpdateAnimations(float deltaTime);

protected:
    // Override these in derived classes to handle property changes
    virtual void OnPropertyAnimationUpdate(const std::string& property, float value) = 0;
    virtual void OnPropertyAnimationComplete(const std::string& property) = 0;

private:
    std::unique_ptr<UIAnimator> m_Animator;

    // Internal callback handlers
    void HandlePropertyUpdate(const std::string& property, float value);
    void HandlePropertyComplete(const std::string& property);
};

} // namespace Core
} // namespace VGE 