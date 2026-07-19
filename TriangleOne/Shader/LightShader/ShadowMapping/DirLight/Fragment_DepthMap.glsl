#version 460 core

#extension GL_ARB_bindless_texture : require
#extension GL_ARB_gpu_shader_int64 : require

struct ShadowInstanceData {
    mat4 modelMatrix;
    uint64_t diffuseTex;
};

layout(std430, binding = 4) readonly buffer Instance_ShadowBuffer {
	ShadowInstanceData shadowInstanceData[];
};

in flat uint uniqueID;
in vec2 TexCoords;

void main()
{
ShadowInstanceData data = shadowInstanceData[uniqueID];
    if (data.diffuseTex != 0ul)
    {
        sampler2D diffuseSampler = sampler2D(data.diffuseTex);
        if (texture(diffuseSampler, TexCoords).a < 0.5)
            discard;
    }
}