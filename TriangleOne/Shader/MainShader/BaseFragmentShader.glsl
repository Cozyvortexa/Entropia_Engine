#version 430 core
layout (location = 0) out vec3 gPosition;
layout (location = 1) out vec3 gNormal;
layout (location = 2) out vec4 gAlbedo;

struct Material {
	sampler2D diffuseText;
	sampler2D specularText;
	sampler2D normalText;

	float shininess;  // Not use, 32.0f
};
uniform Material material;

uniform bool have_NormalMap;
uniform	bool have_Specular;

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

	CheckOpacity(finalDiffuse, finalSpecular);


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

	// Store the value
    gPosition = FragPos;


    gAlbedo.rgb = texture(texture_diffuse1, TexCoords).rgb;
	if (have_Specular){
	    gAlbedo.a = texture(texture_specular1, TexCoords).r;
	}
	else {
		gAlbedo.a = 0.0f;
	}






}

void CheckOpacity(vec4 finalDiffuse, vec4 finalSpecular){
	if(	finalDiffuse.a +  finalSpecular.a < 0.1)
		discard;
}