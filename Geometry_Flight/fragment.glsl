#version 330 core

out vec4 Frag_Color;

in vec3 out_Color;
in vec3 out_Normal;
in vec3 out_Position;
in vec2 TexCoord;


uniform sampler2D outTexture;

uniform vec3 lightPos;
uniform vec3 lightColor;
uniform vec3 viewPos;
void main()
{
    Frag_Color = texture(outTexture, TexCoord);

	if(Frag_Color == vec4(0.0, 0.0, 0.0, 1.0)) {

		float ambientLight = 0.3;
		vec3 ambient = ambientLight * lightColor;

		vec3 normalVector = normalize(out_Normal);
		vec3 lightDir = normalize (lightPos - out_Position);
		float diffuseLight = max(dot(normalVector, lightDir), 0.0);
		vec3 diffuse = diffuseLight * lightColor;
	
		int shininess = 128; //--- ���� ���
		vec3 viewDir = normalize(viewPos - out_Position);
		vec3 reflectDir = reflect(-lightDir, normalVector); //--- �ݻ� ����: reflect �Լ� - �Ի� ������ �ݻ� ���� ���
		float specularLight = max (dot(viewDir, reflectDir), 0.0); //--- V�� R�� ���������� ���� ����: ���� ����
		specularLight = pow(specularLight, shininess); //--- shininess ���� ���־� ���̶���Ʈ�� ������ش�.
		vec3 specular = specularLight * lightColor; 

		vec3 result = (diffuse + ambient + specular) * out_Color;

        Frag_Color = vec4(result, 1.0); // TexCoord�� ��ȿ���� ������ �⺻ ���� ���
    }
	
}