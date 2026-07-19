#version 460 core
#extension GL_ARB_gpu_shader_int64 : require
#extension GL_ARB_bindless_texture : require

#extension GL_ARB_shader_draw_parameters : enable

layout (location = 0) in vec3 in_Position;
layout (location = 1) in vec3 in_Normal;
layout (location = 2) in vec2 in_UV;
layout (location = 3) in vec3 in_Tangent;

out flat uint uniqueID;
out vec2 TexCoords;

struct ShadowCubeMap{
	int quality;
	mat4 shadowMatrices[6];
	vec3 lightPos;
	float far_plane;
};

struct ShadowMap{
	mat4 matrice;
	int quality;
};

struct ShadowInstanceData {
    mat4 modelMatrix;
    uint64_t diffuseTex;
};

layout(std430, binding = 4) readonly buffer Instance_ShadowBuffer {
	ShadowInstanceData shadowInstanceData[];
};

const int MAX_SHADOW_CASTER_POINT_LIGHT = 4;
layout(std430, binding = 5) readonly buffer ShadowBuffer {
	ShadowMap dirLight;
    ShadowCubeMap pointLights[MAX_SHADOW_CASTER_POINT_LIGHT];
	ShadowMap spotLights[];
};


void main()
{
	uint uniqueInstanceID = gl_InstanceID + gl_BaseInstance;
	uniqueID = uniqueInstanceID;
	TexCoords = in_UV;
	gl_Position = dirLight.matrice * shadowInstanceData[uniqueInstanceID].modelMatrix * vec4(in_Position, 1.0);
}