#include "Renderer/UIRenderTarget.h"
#include <glad/gl.h>

namespace VGE {
namespace UI {

UIRenderTarget::UIRenderTarget()
    : m_Framebuffer(0)
    , m_ColorTexture(0)
    , m_DepthStencil(0)
    , m_MultisampleFBO(0)
    , m_MultisampleColor(0)
    , m_MultisampleDepth(0)
    , m_Size(0, 0)
    , m_Samples(0)
{
}

UIRenderTarget::~UIRenderTarget() {
    Cleanup();
}

bool UIRenderTarget::Initialize(int width, int height, int samples) {
    // Store dimensions and sample count
    m_Size = glm::ivec2(width, height);
    m_Samples = samples;

    // Create main framebuffer and attachments
    if (!CreateFramebuffer()) {
        Cleanup();
        return false;
    }

    // Create MSAA buffers if needed
    if (m_Samples > 0) {
        if (!CreateMultisampleBuffers()) {
            Cleanup();
            return false;
        }
    }

    return true;
}

void UIRenderTarget::Cleanup() {
    // Delete MSAA resources
    if (m_MultisampleFBO) {
        glDeleteFramebuffers(1, &m_MultisampleFBO);
        m_MultisampleFBO = 0;
    }
    if (m_MultisampleColor) {
        glDeleteRenderbuffers(1, &m_MultisampleColor);
        m_MultisampleColor = 0;
    }
    if (m_MultisampleDepth) {
        glDeleteRenderbuffers(1, &m_MultisampleDepth);
        m_MultisampleDepth = 0;
    }

    // Delete main framebuffer resources
    if (m_Framebuffer) {
        glDeleteFramebuffers(1, &m_Framebuffer);
        m_Framebuffer = 0;
    }
    if (m_ColorTexture) {
        glDeleteTextures(1, &m_ColorTexture);
        m_ColorTexture = 0;
    }
    if (m_DepthStencil) {
        glDeleteRenderbuffers(1, &m_DepthStencil);
        m_DepthStencil = 0;
    }

    m_Size = glm::ivec2(0, 0);
    m_Samples = 0;
}

void UIRenderTarget::Bind(bool clear) {
    // Bind appropriate framebuffer
    GLuint targetFBO = (m_Samples > 0) ? m_MultisampleFBO : m_Framebuffer;
    glBindFramebuffer(GL_FRAMEBUFFER, targetFBO);

    // Set viewport to match target size
    glViewport(0, 0, m_Size.x, m_Size.y);

    if (clear) {
        // Clear color and depth buffers
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }
}

void UIRenderTarget::Unbind() {
    // Resolve MSAA if needed
    if (m_Samples > 0) {
        ResolveMultisample();
    }

    // Restore default framebuffer
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

bool UIRenderTarget::Resize(int width, int height) {
    // Skip if same size
    if (m_Size.x == width && m_Size.y == height) {
        return true;
    }

    // Store current sample count
    int samples = m_Samples;

    // Cleanup existing resources
    Cleanup();

    // Reinitialize with new size
    return Initialize(width, height, samples);
}

bool UIRenderTarget::CreateFramebuffer() {
    // Create framebuffer
    glGenFramebuffers(1, &m_Framebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, m_Framebuffer);

    // Create color texture
    glGenTextures(1, &m_ColorTexture);
    glBindTexture(GL_TEXTURE_2D, m_ColorTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, m_Size.x, m_Size.y, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_ColorTexture, 0);

    // Create depth/stencil buffer
    glGenRenderbuffers(1, &m_DepthStencil);
    glBindRenderbuffer(GL_RENDERBUFFER, m_DepthStencil);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, m_Size.x, m_Size.y);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, m_DepthStencil);

    // Check framebuffer completeness
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        return false;
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    return true;
}

bool UIRenderTarget::CreateMultisampleBuffers() {
    // Create MSAA framebuffer
    glGenFramebuffers(1, &m_MultisampleFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, m_MultisampleFBO);

    // Create MSAA color buffer
    glGenRenderbuffers(1, &m_MultisampleColor);
    glBindRenderbuffer(GL_RENDERBUFFER, m_MultisampleColor);
    glRenderbufferStorageMultisample(GL_RENDERBUFFER, m_Samples, GL_RGBA8, m_Size.x, m_Size.y);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, m_MultisampleColor);

    // Create MSAA depth/stencil buffer
    glGenRenderbuffers(1, &m_MultisampleDepth);
    glBindRenderbuffer(GL_RENDERBUFFER, m_MultisampleDepth);
    glRenderbufferStorageMultisample(GL_RENDERBUFFER, m_Samples, GL_DEPTH24_STENCIL8, m_Size.x, m_Size.y);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, m_MultisampleDepth);

    // Check framebuffer completeness
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        return false;
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    return true;
}

void UIRenderTarget::ResolveMultisample() {
    // Blit MSAA framebuffer to regular framebuffer
    glBindFramebuffer(GL_READ_FRAMEBUFFER, m_MultisampleFBO);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_Framebuffer);

    // Resolve color and depth
    glBlitFramebuffer(
        0, 0, m_Size.x, m_Size.y,
        0, 0, m_Size.x, m_Size.y,
        GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT,
        GL_NEAREST
    );

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

}} // namespace VGE::UI 