#version 430 core

uniform sampler2D gNormal;
uniform sampler2D texNoise;
uniform sampler2D gDepthMap;

uniform mat4 projection;

uniform mat4 view;
uniform mat4 invProjection;


uniform vec3 samples[64];
uniform int kernelNbr;
uniform float radius = 1.0;
const float bias = 0.025;

uniform float zNear;
uniform float zFar;

out float FragColor;
in vec2 TexCoords;

// tile noise texture over screen, based on screen dimensions / noise size
uniform vec2 noiseScale; 

vec3 ReconstructViewSpace(vec2 TexCoords, float depth);

void main()
{
	vec3 normal = texture(gNormal, TexCoords).xyz;
	normal = normalize(normal * 2.0 - 1.0);
	normal = normalize(mat3(view) * normal);  // Normal to view space

	float depth = texture(gDepthMap, TexCoords).r;



	//TBN - view space
	vec3 randomVec = normalize(texture(texNoise, TexCoords * noiseScale).xyz);
	vec3 tangent = normalize(randomVec - normal * dot(randomVec, normal));
	vec3 bitangent = cross(normal, tangent);
	mat3 TBN = mat3(tangent, bitangent, normal);

	vec3 position = ReconstructViewSpace(TexCoords, depth);



	float occlusion = 0.0;
	for(int i = 0; i < kernelNbr; ++i)
	{
		// get sample position
		vec3 currentSamplePos = TBN * samples[i]; // from tangent to view-space
		currentSamplePos = position + currentSamplePos * radius;

		vec4 offset = projection  * vec4(currentSamplePos, 1.0);   // from view to clip-space
		offset.xyz /= offset.w; // perspective divide
		offset.xyz = offset.xyz * 0.5 + 0.5;

		float sampleDepth = texture(gDepthMap, offset.xy).r;
		vec3 otherPosView = ReconstructViewSpace(offset.xy, sampleDepth);


		float rangeCheck = smoothstep(0.0, 1.0, radius / abs(position.z - otherPosView.z));
        
        occlusion += (otherPosView.z >= currentSamplePos.z + bias ? 1.0 : 0.0) * rangeCheck;
	}
	occlusion = 1.0 - (occlusion / float(kernelNbr));

	FragColor = pow(occlusion, 3.0);
}

//Reconstruct position
vec3 ReconstructViewSpace(vec2 uv, float depth) {
    vec4 ndc = vec4(uv * 2.0 - 1.0, depth * 2.0 - 1.0, 1.0);
    vec4 viewPos = invProjection * ndc;
    return viewPos.xyz / viewPos.w;
}
