#version 430 core
layout (location = 0) out vec3 gPosition;
layout (location = 1) out vec3 gNormal;
layout (location = 2) out vec4 gAlbedo;
layout (location = 3) out vec3 g_ARM;

struct Material {
	sampler2D diffuseText;
	sampler2D normalText;
	sampler2D ARM_Text;

	bool hasARM_Text;

	float ao_Factor;
	float roughness_Factor;
	float metallic_Factor;
};
uniform Material material;

uniform bool have_NormalMap;

uniform vec3 viewPos;


void CheckOpacity(vec4 finalDiffuse, vec4 finalSpecular);



in vec3 FragPos;
in vec3 normal;
in vec2 TexCoords;
in vec4 FragPosLightSpace;


in mat3 TBN;

void main()
{
	vec4 finalDiffuse = texture(material.diffuseText, TexCoords);
	vec4 finalSpecular = texture(material.specularText, TexCoords);

	if (finalDiffuse.a < 0.5)
		discard;

	vec3 norm = vec3(0,0,0);
	if (have_NormalMap){
		norm = texture(material.normalText, TexCoords).rgb;
		norm = normalize(norm * 2.0 - 1.0);
		norm = normalize(TBN * norm);
	}
	else {
		norm = normalize(normal);
	}
	gNormal = normalize(norm);

    gPosition = FragPos;

	if (hasARM_Text){
		g_ARM.r = texture(material.AO_Text, TexCoords).r;
	}
	else {
		g_ARM.r = ao_Factor;
		g_ARM.g = roughness_Factor;
		g_ARM.b = metallic_Factor;
	}

    gAlbedo = vec4(finalDiffuse, 1.0f);
}

void CheckOpacity(vec4 finalDiffuse, vec4 finalSpecular){
	if(	finalDiffuse.a +  finalSpecular.a < 0.1)
		discard;
}