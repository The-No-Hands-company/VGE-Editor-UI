#include "Runtime/Core/Public/Renderer/UITexture.h"
#include <glad/gl.h>
#include <iostream>

namespace VGE {
namespace UI {

UITexture::UITexture(const TextureSpecification& spec)
    : m_Specification(spec)
    , m_RendererID(0)
{
    Initialize();
}

UITexture::UITexture(const std::string& path)
    : m_Path(path)
    , m_RendererID(0)
{
    m_Specification.Format = TextureFormat::RGBA;
    if (!LoadFromFile(path))
    {
        std::cerr << "Failed to load texture: " << path << std::endl;
        return;
    }
}

UITexture::~UITexture()
{
    if (m_RendererID)
        glDeleteTextures(1, &m_RendererID);
}

void UITexture::Bind(uint32_t slot) const
{
    glBindTextureUnit(slot, m_RendererID);
}

void UITexture::Unbind() const
{
    glBindTexture(GL_TEXTURE_2D, 0);
}

void UITexture::SetData(const void* data, uint32_t size)
{
    uint32_t bpp = m_Specification.Format == TextureFormat::RGBA ? 4 : 3;
    if (size != m_Specification.Width * m_Specification.Height * bpp)
    {
        std::cerr << "Data must be entire texture!" << std::endl;
        return;
    }

    glTextureSubImage2D(m_RendererID, 0, 0, 0, m_Specification.Width, m_Specification.Height,
        GetOpenGLFormat(m_Specification.Format), GL_UNSIGNED_BYTE, data);

    if (m_Specification.GenerateMips)
        GenerateMipmaps();
}

void UITexture::SetSubData(const void* data, uint32_t xoffset, uint32_t yoffset, uint32_t width, uint32_t height)
{
    glTextureSubImage2D(m_RendererID, 0, xoffset, yoffset, width, height,
        GetOpenGLFormat(m_Specification.Format), GL_UNSIGNED_BYTE, data);

    if (m_Specification.GenerateMips)
        GenerateMipmaps();
}

void UITexture::SetFilter(TextureFilter minFilter, TextureFilter magFilter)
{
    glTextureParameteri(m_RendererID, GL_TEXTURE_MIN_FILTER, static_cast<GLint>(minFilter));
    glTextureParameteri(m_RendererID, GL_TEXTURE_MAG_FILTER, static_cast<GLint>(magFilter));
}

void UITexture::SetWrap(TextureWrap wrapS, TextureWrap wrapT)
{
    glTextureParameteri(m_RendererID, GL_TEXTURE_WRAP_S, static_cast<GLint>(wrapS));
    glTextureParameteri(m_RendererID, GL_TEXTURE_WRAP_T, static_cast<GLint>(wrapT));
}

void UITexture::GenerateMipmaps()
{
    glGenerateTextureMipmap(m_RendererID);
}

void UITexture::Initialize()
{
    glCreateTextures(GL_TEXTURE_2D, 1, &m_RendererID);
    glTextureStorage2D(m_RendererID, 1, GetOpenGLInternalFormat(m_Specification.Format),
        m_Specification.Width, m_Specification.Height);

    SetFilter(m_Specification.MinFilter, m_Specification.MagFilter);
    SetWrap(m_Specification.WrapS, m_Specification.WrapT);
}

bool UITexture::LoadFromFile(const std::string& path)
{
    // TODO: Implement texture loading using stb_image or similar
    return false;
}

uint32_t UITexture::GetOpenGLFormat(TextureFormat format)
{
    switch (format)
    {
        case TextureFormat::RGB:     return GL_RGB;
        case TextureFormat::RGBA:    return GL_RGBA;
        case TextureFormat::RGBA8:   return GL_RGBA;
        case TextureFormat::RGBA16F: return GL_RGBA;
        case TextureFormat::RGBA32F: return GL_RGBA;
        case TextureFormat::Depth:   return GL_DEPTH_COMPONENT;
        default:                     return GL_RGBA;
    }
}

uint32_t UITexture::GetOpenGLInternalFormat(TextureFormat format)
{
    switch (format)
    {
        case TextureFormat::RGB:     return GL_RGB8;
        case TextureFormat::RGBA:    return GL_RGBA8;
        case TextureFormat::RGBA8:   return GL_RGBA8;
        case TextureFormat::RGBA16F: return GL_RGBA16F;
        case TextureFormat::RGBA32F: return GL_RGBA32F;
        case TextureFormat::Depth:   return GL_DEPTH_COMPONENT24;
        default:                     return GL_RGBA8;
    }
}

}} // namespace VGE::UI


