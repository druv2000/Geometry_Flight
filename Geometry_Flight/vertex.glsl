#version 330 core

layout(location = 0) in vec3 in_Position;
layout(location = 1) in vec3 in_Color;
layout(location = 2) in vec3 in_normal;
layout(location = 3) in vec2 in_texture;

out vec3 out_Color;
out vec3 out_Normal;
out vec3 out_Position;
out vec2 TexCoord;



uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    gl_Position = projection * view * model * vec4(in_Position, 1.0);

    out_Color = in_Color;
    out_Normal = in_normal;
    out_Position = vec3(model * vec4(in_Position, 1.0));
    TexCoord = in_texture;
}