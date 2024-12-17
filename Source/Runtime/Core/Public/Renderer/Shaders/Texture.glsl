#type vertex
#version 330 core

layout(location = 0) in vec2 a_Position;
layout(location = 1) in vec2 a_TexCoord;

uniform mat4 u_Projection;

out vec2 v_TexCoord;

void main()
{
    v_TexCoord = a_TexCoord;
    gl_Position = u_Projection * vec4(a_Position, 0.0, 1.0);
}

#type fragment
#version 330 core

layout(location = 0) out vec4 color;

uniform sampler2D u_Texture;
uniform vec4 u_Color;

in vec2 v_TexCoord;

void main()
{
    color = texture(u_Texture, v_TexCoord) * u_Color;
}
