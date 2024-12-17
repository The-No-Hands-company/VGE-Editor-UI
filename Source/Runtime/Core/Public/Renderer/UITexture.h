#pragma once

#include "Runtime/Core/Public/Core/UICore.h"
#include <glad/gl.h>
#include <string>
#include <cstdint>

namespace VGE {
namespace UI {

enum class TextureFormat : uint32_t {
    None = 0,
    RGB = GL_RGB,
    RGBA = GL_RGBA,
    RGBA8 = GL_RGBA8,
    RGBA16F = GL_RGBA16F,
    RGBA32F = GL_RGBA32F,
    Depth = GL_DEPTH_COMPONENT,
    DepthStencil = GL_DEPTH24_STENCIL8
};

enum class TextureFilter : uint32_t {
    None = 0,
    Linear = GL_LINEAR,
    Nearest = GL_NEAREST,
    LinearMipmapLinear = GL_LINEAR_MIPMAP_LINEAR,
    LinearMipmapNearest = GL_LINEAR_MIPMAP_NEAREST,
    NearestMipmapLinear = GL_NEAREST_MIPMAP_LINEAR,
    NearestMipmapNearest = GL_NEAREST_MIPMAP_NEAREST
};

enum class TextureWrap : uint32_t {
    None = 0,
    Repeat = GL_REPEAT,
    ClampToEdge = GL_CLAMP_TO_EDGE,
    ClampToBorder = GL_CLAMP_TO_BORDER,
    MirroredRepeat = GL_MIRRORED_REPEAT
};

struct TextureSpecification {
    uint32_t Width = 0;
    uint32_t Height = 0;
    TextureFormat Format = TextureFormat::RGBA;
    TextureFilter MinFilter = TextureFilter::Linear;
    TextureFilter MagFilter = TextureFilter::Linear;
    TextureWrap WrapS = TextureWrap::Repeat;
    TextureWrap WrapT = TextureWrap::Repeat;
    bool GenerateMips = true;
};

class UI_API UITexture {
public:
    UITexture(const TextureSpecification& spec);
    UITexture(const std::string& path);
    ~UITexture();

    void Bind(uint32_t slot = 0) const;
    void Unbind() const;

    void SetData(const void* data, uint32_t size);
    void SetSubData(const void* data, uint32_t xoffset, uint32_t yoffset, uint32_t width, uint32_t height);

    void SetFilter(TextureFilter minFilter, TextureFilter magFilter);
    void SetWrap(TextureWrap wrapS, TextureWrap wrapT);
    void GenerateMipmaps();

    const TextureSpecification& GetSpecification() const { return m_Specification; }
    uint32_t GetWidth() const { return m_Specification.Width; }
    uint32_t GetHeight() const { return m_Specification.Height; }
    uint32_t GetRendererID() const { return m_RendererID; }

    bool operator==(const UITexture& other) const { return m_RendererID == other.m_RendererID; }

private:
    void Initialize();
    bool LoadFromFile(const std::string& path);
    static uint32_t GetOpenGLFormat(TextureFormat format);
    static uint32_t GetOpenGLInternalFormat(TextureFormat format);
    static uint32_t GetOpenGLFilter(TextureFilter filter);
    static uint32_t GetOpenGLWrap(TextureWrap wrap);

private:
    TextureSpecification m_Specification;
    uint32_t m_RendererID = 0;
    std::string m_Path;
    bool m_IsLoaded = false;
};

}} // namespace VGE::UI


