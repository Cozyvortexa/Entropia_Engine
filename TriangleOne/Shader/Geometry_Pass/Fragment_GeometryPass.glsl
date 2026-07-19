#version 460 core
#extension GL_ARB_bindless_texture : require
#extension GL_ARB_gpu_shader_int64 : require

layout (location = 0) out vec3 gPosition;
layout (location = 1) out vec3 gNormal;
layout (location = 2) out vec4 gAlbedo;
layout (location = 3) out vec3 g_ARM;

struct InstanceData {
    mat4 modelMatrix;
    uint64_t diffuseTex;
    uint64_t normalTex;
    uint64_t aoTex;
    uint64_t roughnessTex;
    uint64_t metallicTex;

    float aoFactor;
    float roughnessFactor;
    float metallicFactor;
    uint hasRoughness;
    uint hasMetallic;
	uint hasNormalMap;
};

layout(std430, binding = 1) readonly buffer InstanceBuffer {
    InstanceData instances[];
};


//From the vertex shader 
in flat uint uniqueID;
in vec3 FragPos;
in vec3 normal;
in vec2 TexCoords;
in mat3 TBN;


void main()
{
	InstanceData data = instances[uniqueID];
	
	sampler2D diffuseSampler = sampler2D(data.diffuseTex);
	vec4 diffuse = texture(diffuseSampler, TexCoords);

	if (diffuse.a < 0.5)
		discard;

	vec3 norm = vec3(0,0,0);
	if (data.hasNormalMap == 1){
		sampler2D normalSampler = sampler2D(data.normalTex);
		norm = texture(normalSampler, TexCoords).rgb;
		norm = normalize(norm * 2.0 - 1.0);
		norm = normalize(TBN * norm);
	}
	else {
		norm = normal;
	}
	gNormal = normalize(norm) * 0.5 + 0.5;

    gPosition = FragPos;

	sampler2D aoSampler = sampler2D(data.aoTex);
	g_ARM.r = texture(aoSampler, TexCoords).r * data.aoFactor;
	g_ARM.g = data.roughnessFactor;
	g_ARM.b = data.metallicFactor;

	//temp, case by case, AO have a fallback texture
	if(data.hasRoughness == 1){
		sampler2D roughnessSampler = sampler2D(data.roughnessTex);
		g_ARM.g = texture(roughnessSampler, TexCoords).g * data.roughnessFactor;
	}
	if(data.hasMetallic == 1){
		sampler2D metalicSampler = sampler2D(data.metallicTex);
		g_ARM.b = texture(metalicSampler, TexCoords).r * data.metallicFactor;
	}

    gAlbedo = vec4(diffuse.rgb, 1.0f);
}