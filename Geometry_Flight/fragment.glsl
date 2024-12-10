#version 330 core

out vec4 Frag_Color;

in vec3 out_Color;
in vec3 out_Normal;
in vec3 out_Position;

uniform vec3 lightPos;
uniform vec3 lightColor;

void main()
{


	float ambientLight = 1.0;
	vec3 ambient = ambientLight * lightColor;
	vec3 normalVector = normalize(out_Normal);
	vec3 lightDir = normalize (lightPos - out_Position);
	float diffuseLight = max(dot(normalVector, lightDir), 0.0);
	vec3 diffuse = diffuseLight * lightColor;
	vec3 result = (diffuse + ambient) * out_Color;


	Frag_Color = vec4(result, 1.0);
}