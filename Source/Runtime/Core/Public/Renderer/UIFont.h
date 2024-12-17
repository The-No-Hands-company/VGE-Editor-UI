#pragma once

#include "Runtime/Core/Public/Core/UICore.h"
#include "Runtime/Core/Public/Core/UITypes.h"
#include <ft2build.h>
#include FT_FREETYPE_H
#include <string>
#include <unordered_map>

namespace VGE {
namespace UI {

class UI_API UIFont {
public:
    UIFont();
    ~UIFont();

    bool Load(const std::string& filename, unsigned int size);
    void Unload();

    const Character* GetCharacter(char c) const;
    float GetSize() const { return m_Size; }

private:
    FT_Library m_FreeType;
    FT_Face m_Face;
    std::unordered_map<char, Character> m_Characters;
    float m_Size;
};

}} // namespace VGE::UI
