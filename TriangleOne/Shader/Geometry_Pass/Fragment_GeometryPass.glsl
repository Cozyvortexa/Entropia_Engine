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


in vec3 FragPos;
in vec3 normal;
in vec2 TexCoords;
in vec4 FragPosLightSpace;


in mat3 TBN;

void main()
{
	vec4 finalDiffuse = texture(material.diffuseText, TexCoords);

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

	if (material.hasARM_Text){
		vec3 ARM_text = texture(material.ARM_Text, TexCoords).rgb;
		g_ARM.r = ARM_text.r * material.ao_Factor;
		g_ARM.g = ARM_text.g * material.roughness_Factor;
		g_ARM.b = ARM_text.b * material.metallic_Factor;
	}
	else {
		g_ARM.r = material.ao_Factor;
		g_ARM.g = material.roughness_Factor;
		g_ARM.b = material.metallic_Factor;
	}

    gAlbedo = vec4(finalDiffuse.rgb, 1.0f);
}