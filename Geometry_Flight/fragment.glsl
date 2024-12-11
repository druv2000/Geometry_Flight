#version 330 core

out vec4 Frag_Color;

in vec3 out_Color;
in vec3 out_Normal;
in vec3 out_Position;

uniform vec3 lightPos;
uniform vec3 lightColor;
uniform vec3 viewPos;
void main()
{


	float ambientLight = 1.0;
	vec3 ambient = ambientLight * lightColor;
	vec3 normalVector = normalize(out_Normal);
	vec3 lightDir = normalize (lightPos - out_Position);
	float diffuseLight = max(dot(normalVector, lightDir), 0.0);
	vec3 diffuse = diffuseLight * lightColor;
	
	int shininess = 128; //--- 광택 계수
	vec3 viewDir = normalize(viewPos - out_Position);
	vec3 reflectDir = reflect(-lightDir, normalVector); //--- 반사 방향: reflect 함수 - 입사 벡터의 반사 방향 계산
	float specularLight = max (dot(viewDir, reflectDir), 0.0); //--- V와 R의 내적값으로 강도 조절: 음수 방지
	specularLight = pow(specularLight, shininess); //--- shininess 승을 해주어 하이라이트를 만들어준다.
	vec3 specular = specularLight * lightColor; 
	vec3 result = (diffuse + ambient + specular) * out_Color;
	Frag_Color = vec4(result, 1.0);
}