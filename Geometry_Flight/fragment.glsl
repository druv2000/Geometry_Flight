#version 330 core

in vec3 out_Color;
out vec4 Frag_Color;

void main()
{
	Frag_Color = vec4(out_Color, 1.0);
}