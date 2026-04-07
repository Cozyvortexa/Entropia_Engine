#version 430 core

uniform sampler2D gNormal;
uniform sampler2D texNoise;
uniform sampler2D gDepthMap;

uniform mat4 projection;
uniform mat4 invProjection;

uniform vec3 samples[64];
uniform int kernelNbr;
const float radius = 0.1;
const float bias = 0.005;


out float FragColor;
in vec2 TexCoords;

// tile noise texture over screen, based on screen dimensions / noise size
uniform vec2 noiseScale; // screen = 800x600

vec3 ReconstructViewSpace(vec2 TexCoords, float depth);

void main()
{
	vec3 normal = texture(gNormal, TexCoords).rgb;
	vec3 randomVec = texture(texNoise, TexCoords * noiseScale).xyz;
	float depth = texture(gDepthMap, TexCoords).r;

	vec3 position = ReconstructViewSpace(TexCoords,depth);


	//TBN 
	vec3 tangent = normalize(randomVec - normal * dot(randomVec, normal));
	vec3 bitangent = cross(normal, tangent);
	mat3 TBN = mat3(tangent, bitangent, normal);

	float occlusion = 0.0;
	for(int i = 0; i < kernelNbr; ++i)
	{
		// get sample position
		vec3 currentSample = TBN * samples[i]; // from tangent to view-space
		currentSample = position + currentSample * radius;

		vec4 offset = vec4(currentSample, 1.0);
		offset = projection * offset; // from view to clip-space
		offset.xyz /= offset.w; // perspective divide
		offset.xyz = offset.xyz * 0.5 + 0.5;

		float sampleDepth = texture(gDepthMap, offset.xy).r;
		vec3 otherPos = ReconstructViewSpace(offset.xy, sampleDepth);

		float rangeCheck = smoothstep(0.0, 1.0, radius / abs(position.z - otherPos.z));
		occlusion += (otherPos.z >= currentSample.z + bias ? 1.0 : 0.0) * rangeCheck;
	}
	occlusion = 1.0 - (occlusion / kernelNbr);
	float linearZ = position.z / -100.0; 
	FragColor = occlusion;
}

//Reconstruct position
vec3 ReconstructViewSpace(vec2 TextureCoords, float depth){
	vec4 pos_NDC = vec4(TextureCoords.x * 2 - 1, TextureCoords.y * 2 - 1, depth * 2 - 1, 1.0);  //Screen Space (UV/Depth) to NDC
	pos_NDC = invProjection * pos_NDC;  //NDC to view space
	return pos_NDC.xyz / pos_NDC.w;
}
