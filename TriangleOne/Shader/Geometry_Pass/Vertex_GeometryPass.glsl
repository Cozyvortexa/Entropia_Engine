#version 460 core
#extension GL_ARB_gpu_shader_int64 : require
#extension GL_ARB_bindless_texture : require

#extension GL_ARB_shader_draw_parameters : enable

layout (location = 0) in vec3 in_Position;
layout (location = 1) in vec3 in_Normal;
layout (location = 2) in vec2 in_UV;
layout (location = 3) in vec3 in_Tangent;

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


uniform mat4 view;
uniform mat4 projection;

out flat uint uniqueID;
out vec3 FragPos;
out vec3 normal;
out vec2 TexCoords;
out mat3 TBN;

void main()
{
	uint uniqueInstanceID = gl_InstanceID + gl_BaseInstance;
	uniqueID = uniqueInstanceID;
	mat4 modelMatrix = instances[uniqueInstanceID].modelMatrix;

	FragPos = vec3(modelMatrix * vec4(in_Position, 1.0));

	normal = mat3(transpose(inverse(modelMatrix))) * in_Normal;
	normal = normalize(normal);
	TexCoords = in_UV;

	vec3 T = normalize(vec3(modelMatrix * vec4(in_Tangent, 0.0)));
	vec3 N = normalize(normal);
	T = normalize(T - dot(T, N) * N);
	vec3 B = cross(N, T);

	TBN = mat3(T, B, N);

	gl_Position = projection * view * modelMatrix * vec4(in_Position, 1.0);
}