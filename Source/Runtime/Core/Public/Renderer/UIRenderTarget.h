#pragma once

#include "Core/UICore.h"
#include "Renderer/UIRenderContext.h"
#include <glad/gl.h>
#include <memory>
#include <glm/glm.hpp>

namespace VGE {
namespace UI {

/**
 * @brief Manages off-screen rendering targets
 * 
 * The UIRenderTarget class provides:
 * - Framebuffer object (FBO) management
 * - Texture-based render targets
 * - Multi-sample anti-aliasing support
 * - Depth and stencil buffer support
 * 
 * Usage example:
 * @code
 * UIRenderTarget target;
 * target.Initialize(512, 512);  // Create 512x512 render target
 * 
 * target.Bind();
 * // Render content to target
 * target.Unbind();
 * 
 * // Use the rendered content as a texture
 * GLuint texture = target.GetColorTexture();
 * @endcode
 */
class UI_API UIRenderTarget {
public:
    /**
     * @brief Constructor
     * Initializes member variables to default values
     */
    UIRenderTarget();

    /**
     * @brief Destructor
     * Cleans up OpenGL resources (FBO, textures, etc.)
     */
    ~UIRenderTarget();

    /**
     * @brief Initialize the render target
     * 
     * Creates OpenGL resources:
     * - Framebuffer object
     * - Color texture
     * - Depth/stencil buffer
     * - Optional MSAA buffers
     * 
     * @param width Width in pixels
     * @param height Height in pixels
     * @param samples Number of MSAA samples (0 for no MSAA)
     * @return True if initialization succeeded
     */
    bool Initialize(int width, int height, int samples = 0);

    /**
     * @brief Clean up resources
     * 
     * Releases all OpenGL resources:
     * - Deletes framebuffer
     * - Deletes textures
     * - Deletes render buffers
     */
    void Cleanup();

    /**
     * @brief Bind this render target for rendering
     * 
     * Makes this the active render target:
     * - Binds framebuffer
     * - Sets viewport
     * - Clears buffers
     * 
     * @param clear Whether to clear the target (default: true)
     */
    void Bind(bool clear = true);

    /**
     * @brief Unbind this render target
     * 
     * Restores default framebuffer and state
     */
    void Unbind();

    /**
     * @brief Resize the render target
     * 
     * Updates target size and recreates resources:
     * - Resizes textures
     * - Resizes render buffers
     * - Maintains MSAA settings
     * 
     * @param width New width in pixels
     * @param height New height in pixels
     * @return True if resize succeeded
     */
    bool Resize(int width, int height);

    /**
     * @brief Get the color texture
     * @return OpenGL texture handle for the color buffer
     */
    uint32_t GetColorTexture() const { return m_ColorTexture; }

    /**
     * @brief Get the target dimensions
     * @return Size in pixels (width, height)
     */
    const glm::ivec2& GetSize() const { return m_Size; }

    /**
     * @brief Check if target is valid
     * @return True if target is initialized and ready
     */
    bool IsValid() const { return m_Framebuffer != 0; }

    /**
     * @brief Get MSAA sample count
     * @return Number of MSAA samples (0 if MSAA disabled)
     */
    int GetSampleCount() const { return m_Samples; }

private:
    uint32_t m_Framebuffer;        ///< OpenGL framebuffer object
    uint32_t m_ColorTexture;       ///< Color buffer texture
    uint32_t m_DepthStencil;       ///< Combined depth/stencil buffer
    uint32_t m_MultisampleFBO;     ///< MSAA framebuffer (if enabled)
    uint32_t m_MultisampleColor;   ///< MSAA color buffer
    uint32_t m_MultisampleDepth;   ///< MSAA depth buffer
    glm::ivec2 m_Size;           ///< Target dimensions
    int m_Samples;               ///< MSAA sample count

    /**
     * @brief Create framebuffer resources
     * 
     * Sets up framebuffer and attachments:
     * - Creates textures and buffers
     * - Configures attachments
     * - Validates framebuffer completeness
     * 
     * @return True if creation succeeded
     */
    bool CreateFramebuffer();

    /**
     * @brief Create MSAA resources
     * 
     * Sets up MSAA framebuffer and buffers:
     * - Creates MSAA color buffer
     * - Creates MSAA depth buffer
     * - Configures MSAA framebuffer
     * 
     * @return True if creation succeeded
     */
    bool CreateMultisampleBuffers();

    /**
     * @brief Resolve MSAA to final texture
     * 
     * Blits MSAA framebuffer to regular framebuffer:
     * - Copies and resolves color buffer
     * - Copies and resolves depth buffer
     */
    void ResolveMultisample();
};

}} // namespace VGE::UI 