#pragma once

namespace VGE {
namespace UI {

// Basic vertex shader for 2D UI rendering
static const char* g_BasicVertexShader = R"(
    #version 330 core
    layout (location = 0) in vec3 a_Position;
    layout (location = 1) in vec2 a_TexCoord;
    layout (location = 2) in vec4 a_Color;

    uniform mat4 u_ProjectionView;
    uniform mat4 u_Transform;

    out vec2 v_TexCoord;
    out vec4 v_Color;

    void main()
    {
        v_TexCoord = a_TexCoord;
        v_Color = a_Color;
        gl_Position = u_ProjectionView * u_Transform * vec4(a_Position, 1.0);
    }
)";

// Basic fragment shader for 2D UI rendering
static const char* g_BasicFragmentShader = R"(
    #version 330 core
    in vec2 v_TexCoord;
    in vec4 v_Color;

    uniform sampler2D u_Texture;
    uniform bool u_UseTexture;

    out vec4 FragColor;

    void main()
    {
        if (u_UseTexture)
            FragColor = texture(u_Texture, v_TexCoord) * v_Color;
        else
            FragColor = v_Color;
    }
)";

// Text vertex shader
static const char* g_TextVertexShader = R"(
    #version 330 core
    layout (location = 0) in vec3 a_Position;
    layout (location = 1) in vec2 a_TexCoord;
    layout (location = 2) in vec4 a_Color;

    uniform mat4 u_ProjectionView;

    out vec2 v_TexCoord;
    out vec4 v_Color;

    void main()
    {
        v_TexCoord = a_TexCoord;
        v_Color = a_Color;
        gl_Position = u_ProjectionView * vec4(a_Position, 1.0);
    }
)";

// Text fragment shader
static const char* g_TextFragmentShader = R"(
    #version 330 core
    in vec2 v_TexCoord;
    in vec4 v_Color;

    uniform sampler2D u_Texture;

    out vec4 FragColor;

    void main()
    {
        vec4 sampled = vec4(1.0, 1.0, 1.0, texture(u_Texture, v_TexCoord).r);
        FragColor = v_Color * sampled;
    }
)";

}} // namespace VGE::UI



