#version 450 core

in vec2 v_TexCoord;

uniform vec4 u_Color;
uniform sampler2D u_Texture;
uniform bool u_UseTexture;
uniform float u_CornerRadius;

out vec4 FragColor;

float roundedBoxSDF(vec2 centerUV, vec2 size, float radius) {
    vec2 d = abs(centerUV) - size + radius;
    return length(max(d, 0.0)) + min(max(d.x, d.y), 0.0) - radius;
}

void main() {
    if (u_UseTexture) {
        vec4 texColor = texture(u_Texture, v_TexCoord);
        FragColor = texColor * u_Color;
    } else {
        // For rounded rectangles
        if (u_CornerRadius > 0.0) {
            vec2 centerUV = v_TexCoord * 2.0 - 1.0;
            float distance = roundedBoxSDF(centerUV, vec2(1.0), u_CornerRadius);
            float smoothedAlpha = 1.0 - smoothstep(-0.01, 0.01, distance);
            FragColor = vec4(u_Color.rgb, u_Color.a * smoothedAlpha);
        } else {
            FragColor = u_Color;
        }
    }
}
