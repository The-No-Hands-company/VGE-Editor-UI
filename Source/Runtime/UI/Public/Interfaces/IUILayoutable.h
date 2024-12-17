#pragma once

#include "Runtime/Core/Public/Core/UICore.h"
#include <glm/glm.hpp>

namespace VGE {
namespace UI {

class IUILayoutable {
public:
    virtual ~IUILayoutable() = default;
    virtual void SetPosition(const glm::vec2& position) = 0;
    virtual void SetSize(const glm::vec2& size) = 0;
    virtual glm::vec2 GetPosition() const = 0;
    virtual glm::vec2 GetSize() const = 0;
    virtual void UpdateLayout() = 0;
};

}} // namespace VGE::UI



