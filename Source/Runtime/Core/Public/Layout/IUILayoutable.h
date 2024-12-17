#pragma once

#include "Runtime/Core/Public/Core/UICore.h"

#include "Runtime/Core/Public/Core/UICore.h"
#include "Runtime/Core/Public/Core/UITypes.h"
#include <glm/glm.hpp>

namespace VGE {
namespace UI {

class UI_API IUILayoutable {
public:
    virtual ~IUILayoutable() = default;

    virtual void SetPosition(const glm::vec2& position) = 0;
    virtual void SetSize(const glm::vec2& size) = 0;
    virtual const glm::vec2& GetPosition() const = 0;
    virtual const glm::vec2& GetSize() const = 0;
    virtual Rect GetRect() const = 0;
    virtual void UpdateLayout() = 0;
};

}} // namespace VGE::UI



