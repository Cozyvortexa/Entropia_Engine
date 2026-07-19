#version 430 core
#extension GL_ARB_bindless_texture : require
#extension GL_ARB_gpu_shader_int64 : require

layout (location = 0) out vec4 FragColor;
layout (location = 1) out vec4 BrightColor;

struct Material {
	vec3 albedo;
	float ambientOcclusion;
	float metallic;
	float roughness;
};


struct SpotLight {
	vec3 position;
	vec3 direction;

	vec3 color;

	float cutOff;
	float outerCutOff;

	float range;
	int shadowIndex;
	int shadowQuality;
};
#define NBR_MAX_SPOT_LIGHTS 8

struct PointLight {
	vec3 position;
	vec3 color;

	float range;
	int shadowIndex;
	int shadowQuality;
};
#define NBR_MAX_POINT_LIGHTS 8

struct DirLight{
	vec3 direction;
	vec3 color;
};


//SSBO
layout (std430, binding = 3) buffer Lights{
	DirLight dirLight;
	PointLight pointLights[NBR_MAX_POINT_LIGHTS];
	SpotLight spotLights[NBR_MAX_SPOT_LIGHTS];
	int nbrPointLight;
	int nbrSpotLight;
};

//Shadow SSBO
layout (std430, binding = 6) buffer ShadowText{
	uint64_t pointLight_Shadow[3]; 
	uint64_t spotLight_Shadow[3];// [0]=low, [1]=medium, [2]=hight
};
//The dir light is managed seperately
uniform sampler2DShadow dirLight_shadowMap; 
uniform bool dirLight_HaveShadow;


struct ShadowMap{
	mat4 matrice;
	int quality;
	float _pad[3];
};
struct ShadowCubeMap{
	int quality;
	mat4 shadowMatrices[6];
	vec3 lightPos;
	float far_plane;
};
const int MAX_SHADOW_CASTER_POINT_LIGHT = 4;
const int MAX_SHADOW_CASTER_SPOT_LIGHT = 8; // adapte à ta vraie constante

layout(std430, binding = 5) readonly buffer ShadowBuffer {
	ShadowMap dirLight_Shadow; // renommé pour éviter le clash avec la struct DirLight "dirLight" du binding 3
	ShadowCubeMap pointLightsShadow[MAX_SHADOW_CASTER_POINT_LIGHT];
	ShadowMap spotLightsShadow[];
};


uniform vec3 viewPos;
////////////////Function

////////Light
vec3 CalcDirLight(DirLight light, vec3 viewDir, vec3 FragPos, vec3 Normal, vec3 Albedo, vec4 FragPosLightSpace, Material material, vec3 F0, vec3 kD, vec3 kS);
vec3 CalcPointLight(PointLight light, int lightIndex, vec3 viewDir, vec3 FragPos, vec3 Normal, Material material, vec3 F0, vec3 kD, vec3 kS);
vec3 CalcSpotLight(SpotLight light, int lightIndex, vec3 viewDir, vec3 FragPos, vec3 Normal, vec3 Albedo, Material material, vec3 F0, vec3 kD, vec3 kS);

float ShadowDirLight(vec4 FragPosLightSpace);
float ShadowPointLight(PointLight light, int lightIndex, vec3 FragPos, vec3 Normal);
float ShadowSpotLight(SpotLight light, int lightIndex, vec3 FragPos);

////////PBR
vec3 fresnelSchlick(float cosTheta, vec3 F0);
vec3 FresnelSchlickRoughness(float cosTheta, vec3 F0, float roughness);
float DistributionGGX(vec3 N, vec3 H, float roughness);
float GeometrySchlickGGX(float NdotV, float roughness);
float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness);
vec3 CalcPbrLight(vec3 lightDir, vec3 V, vec3 H, vec3 Normal, vec3 radiance, Material material, vec3 F0);

////////////////
in vec2 TexCoords;

uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D gAlbedo;
uniform sampler2D gDepth;
uniform sampler2D gARM;
uniform sampler2D ssaoTexture;

uniform samplerCube prefilterMap;
uniform sampler2D brdfLUT;

uniform samplerCube irradianceMap;

uniform int renderTarget;


const float PI = 3.14159265359;
const float MAX_REFLECTION_LOD = 4.0;

void main()
{
	float depth = texture(gDepth, TexCoords).r;
	if(depth == 1.0) {
		FragColor = vec4(0.2, 0.3, 0.3, 1.0);
		BrightColor = vec4(0.0, 0.0, 0.0, 1.0);
		return;
	}

	vec3 FragPos = texture(gPosition, TexCoords).rgb;
	vec3 Normal = texture(gNormal, TexCoords).rgb;
	Normal = normalize(Normal * 2.0 - 1.0);
	vec3 Albedo = texture(gAlbedo, TexCoords).rgb;
	Albedo = pow(Albedo, vec3(2.2));
	
	//ARM
	vec3 ARM_Text = texture(gARM, TexCoords).rgb;
	float ambientOcclusion = ARM_Text.r * texture(ssaoTexture, TexCoords).r;
	float roughness = ARM_Text.g;
	float metallic = ARM_Text.b;
	
	vec3 diffuse = texture(irradianceMap, Normal).rgb * Albedo;


	if (renderTarget == 1){
		FragColor = vec4(Albedo,1.0);
		return;
	}
	else if (renderTarget == 2){
		FragColor = vec4(FragPos, 1.0f);
		return;
	}
	else if (renderTarget == 3){
		FragColor = vec4(Normal * 0.5 + 0.5, 1.0);
		return;
	}
	else if (renderTarget == 4){
		FragColor = vec4(vec3(depth), 1.0f);
		return;
	}
	else if (renderTarget == 5){
		FragColor = vec4(vec3(ambientOcclusion), 1.0f);
		return;
	}
	else if (renderTarget == 6){
		FragColor = vec4(vec3(roughness), 1.0f);
		return;
	}
	else if (renderTarget == 7){
		FragColor = vec4(vec3(metallic), 1.0f);
		return;
	}
	else if (renderTarget == 8){
		FragColor = vec4(diffuse / (diffuse + vec3(1.0)), 1.0f);
		return;
	}


	Material material;
	material.albedo = Albedo;
	material.metallic = metallic;
	material.roughness = roughness;
	material.ambientOcclusion = ambientOcclusion;

	vec3 viewDir = normalize(viewPos - FragPos);

	vec3 final_lightning = vec3(0,0,0);

	vec4 FragPosLightSpace = dirLight_Shadow.matrice * vec4(FragPos, 1.0);

	vec3 F0 = vec3(0.04);
	F0 = mix(F0, material.albedo, material.metallic);
	//Reflected light
	vec3 kS = FresnelSchlickRoughness(max(dot(Normal, viewDir), 0.0), F0, material.roughness);
	//Absorbed light
	vec3 kD = vec3(1.0) - kS;
	kD *= 1.0 - material.metallic;

	vec3 R = reflect(-viewDir, Normal);
	vec3 prefilteredColor = textureLod(prefilterMap, R, roughness * MAX_REFLECTION_LOD).rgb;
	vec3 F = FresnelSchlickRoughness(max(dot(Normal, viewDir), 0.0), F0, roughness);
	vec2 envBRDF = texture(brdfLUT, vec2(max(dot(Normal, viewDir), 0.0), roughness)).rg;
	vec3 specular = prefilteredColor * (F * envBRDF.x + envBRDF.y);

	//Indirect Ambient
	vec3 ambient = (kD * diffuse + specular) * material.ambientOcclusion;


	final_lightning += CalcDirLight(dirLight, viewDir, FragPos, Normal, Albedo, FragPosLightSpace, material, F0, kD, kS); // Une seule lumiere dir dans la scene 

	for (int i = 0; i < nbrPointLight; i++)
	{
		if (length(pointLights[i].color )> 0.001  ){  // On aplique pas le calcul si les lumiere sont eteinte
			final_lightning += CalcPointLight(pointLights[i], i, viewDir, FragPos, Normal, material, F0, kD, kS);
		}
	}
	for (int i = 0; i < nbrSpotLight; i++){
		if (length(spotLights[i].color) > 0.001 ){
			final_lightning += CalcSpotLight(spotLights[i], i, viewDir, FragPos, Normal, Albedo, material, F0, kD, kS);
		}
	}


	
	FragColor = vec4(final_lightning + ambient, 1.0);

	//Bloom
	float brightness = dot(FragColor.rgb, vec3(0.2126, 0.7152, 0.0722));
	if(brightness > 1.0)
		BrightColor = vec4(FragColor.rgb, 1.0);
	else
		BrightColor = vec4(0.0, 0.0, 0.0, 1.0);

}

///////////////////////////  Light
vec3 CalcDirLight(DirLight light, vec3 viewDir, vec3 FragPos, vec3 Normal, vec3 Albedo, vec4 FragPosLightSpace, Material material, vec3 F0, vec3 kD, vec3 kS)
{
	vec3 lightDir = normalize(light.direction);

	vec3 currentSpecular = vec3(0);
	float diff = max(dot(Normal, lightDir), 0.0);
	if (diff > 0.0) {
		vec3 H = normalize(viewDir + lightDir);

		vec3 radiance = light.color;
		vec3 Lo = CalcPbrLight(lightDir, viewDir, H, Normal, radiance, material, F0);

	    float shadow = ShadowDirLight(FragPosLightSpace);
		return Lo * (1.0 - shadow);;
	}
	return vec3(0.0);
}

vec3 CalcPointLight(PointLight light, int lightIndex, vec3 viewDir, vec3 FragPos, vec3 Normal, Material material, vec3 F0, vec3 kD, vec3 kS)
{
	float distance = length(light.position - FragPos);
	if(distance > light.range) { 
        return vec3(0.0, 0.0, 0.0);  // A modiff
    }

	vec3 lightDir = normalize(light.position - FragPos);
	vec3 H = normalize(viewDir + lightDir);

	float distFrac = distance / light.range;
	float attenuation = clamp(1.0 - (distFrac * distFrac * distFrac * distFrac), 0.0, 1.0);
	attenuation = attenuation * attenuation;
	attenuation = attenuation / (distance * distance + 1.0);

	vec3 radiance = light.color * attenuation;

	float shadow = ShadowPointLight(light, lightIndex, FragPos, Normal);
	float lightModifier = (1.0 - shadow); 

	vec3 Lo = CalcPbrLight(lightDir, viewDir, H, Normal, radiance, material, F0);

	return Lo * lightModifier;
}

vec3 CalcSpotLight(SpotLight light, int lightIndex, vec3 viewDir, vec3 FragPos, vec3 Normal, vec3 Albedo, Material material, vec3 F0, vec3 kD, vec3 kS) 
{
	vec3 lightDir = normalize(light.position - FragPos);  // Direction entre la source de lumiere et la normal du vertex

	//
    float theta = dot(lightDir, normalize(-light.direction)); 
    float epsilon = light.cutOff - light.outerCutOff;
    float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);
	//
	float diff = max(dot(Normal, lightDir), 0.0);  // Calcul de l'angle entre la normal et le vec distance 

	vec3 diffuse = light.color * diff * Albedo.rgb;

	// attenuation
	float distance    = length(light.position - FragPos);
    float distFrac    = distance / light.range;
	float attenuation = clamp(1.0 - distFrac * distFrac * distFrac * distFrac, 0.0, 1.0);
    attenuation       = (attenuation * attenuation) / (distance * distance + 1.0);

	vec3 H = normalize(viewDir + lightDir);

	vec3 radiance = light.color * attenuation * intensity;

	vec3 Lo = CalcPbrLight(lightDir, viewDir, H, Normal, radiance, material, F0);

	//Shadow
	float shadow = ShadowSpotLight(light, lightIndex, FragPos);
	float lightModifier = 1.0 - shadow;
	
	return Lo * lightModifier;
}

///////////////////////////  Shadow
float ShadowDirLight(vec4 FragPosLightSpace)
{
    vec3 projCoords = FragPosLightSpace.xyz / FragPosLightSpace.w;
    projCoords = projCoords * 0.5 + 0.5;

    if (projCoords.z > 1.0 || !dirLight_HaveShadow)
        return 0.0;

    float bias = 0.005;

    float shadow = texture(
        dirLight_shadowMap,
        vec3(projCoords.xy, projCoords.z - bias)
    );

    return 1.0 - shadow;
}


float GetShadowMapValue(int quality, int index, vec4 dirDepth) {
	if (quality < 0 || quality > 3) return 0.0;

	samplerCubeArrayShadow shadowMap = samplerCubeArrayShadow(pointLight_Shadow[quality]);

    return texture(shadowMap, vec4(dirDepth.xyz, float(index)), dirDepth.w);
}

float ShadowPointLight(PointLight light, int lightIndex, vec3 FragPos, vec3 Normal){
	if (light.shadowIndex == -1)
		return 0.0; // This light does not cast shadow

	vec3 sampleOffsetDirections[20] = vec3[]
	(
		vec3( 1, 1, 1), vec3( 1, -1, 1), vec3(-1, -1, 1), vec3(-1, 1, 1),
		vec3( 1, 1, -1), vec3( 1, -1, -1), vec3(-1, -1, -1), vec3(-1, 1, -1),
		vec3( 1, 1, 0), vec3( 1, -1, 0), vec3(-1, -1, 0), vec3(-1, 1, 0),
		vec3( 1, 0, 1), vec3(-1, 0, 1), vec3( 1, 0, -1), vec3(-1, 0, -1),
		vec3( 0, 1, 1), vec3( 0, -1, 1), vec3( 0, -1, -1), vec3( 0, 1, -1)
	);

    vec3 fragToLight = FragPos - light.position;
	float currentDepth = length(fragToLight);

	if(currentDepth >= light.range)
        return 0.0;

	float shadow = 0.0;
	float samples = 8.0;
	float viewDistance = length(viewPos- FragPos);

    
    float lightFar_plane = light.range; 

	float bias = 0.005;
	float diskRadius = (1.0 + (viewDistance / lightFar_plane)) / 25.0;
	float currentDepthNormalized = (currentDepth - bias) / light.range;

	for(int i = 0; i < samples; ++i)
	{
		vec3 dir = fragToLight + (sampleOffsetDirections[i] * diskRadius);
        shadow += GetShadowMapValue(light.shadowQuality, light.shadowIndex, vec4(dir, currentDepthNormalized));
	}
    
    // Average the results
	shadow /= samples;

	return  shadow;
}


float GetSpotShadowMapValue(int quality, int index, vec3 coords) {
    if (quality < 0 || quality >= 3)
        return 0.0;

    sampler2DArrayShadow shadowMap = sampler2DArrayShadow(spotLight_Shadow[quality]);

    return texture(shadowMap, vec4(coords.xy, float(index), coords.z));
}

float ShadowSpotLight(SpotLight light, int lightIndex, vec3 FragPos){
	if (light.shadowIndex == -1)
		return 0.0; //This light does not cast shadow

	vec4 fragPosSpotSpace = spotLightsShadow[lightIndex].matrice * vec4(FragPos, 1.0);
    vec3 projCoords = fragPosSpotSpace.xyz / fragPosSpotSpace.w;
	projCoords = projCoords * 0.5 + 0.5;

	if (projCoords.z > 1.0 || projCoords.z < 0.0
	|| projCoords.x < 0.0 || projCoords.x > 1.0
	|| projCoords.y < 0.0 || projCoords.y > 1.0)
		return 0.0;

	vec3 lightDir = normalize(light.position - FragPos);
    float bias = 0.005; //max(0.02 * (1.0 - dot(Normal, lightDir)), 0.002);
	float shadow = 0.0;

	sampler2DArrayShadow shadowMap = sampler2DArrayShadow(spotLight_Shadow[light.shadowQuality]);
	vec2 texelSize = 1.0 / vec2(textureSize(shadowMap, 0).xy);

	for(int x = -1; x <= 1; ++x) {
		for(int y = -1; y <= 1; ++y) {
			vec2 offset = vec2(x, y) * texelSize;
			vec3 shadowCoords = vec3(projCoords.xy + offset, projCoords.z - bias);
			shadow += GetSpotShadowMapValue(light.shadowQuality, light.shadowIndex, shadowCoords);
		}
	}

	return  1.0 - shadow;
}


/////////////////////////// PBR
vec3 fresnelSchlick(float cosTheta, vec3 F0)
{
	return F0 + (1.0 - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}
vec3 FresnelSchlickRoughness(float cosTheta, vec3 F0, float roughness)
{
	return F0 + (max(vec3(1.0 - roughness), F0) - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}
float DistributionGGX(vec3 N, vec3 H, float roughness)
{
	float a = roughness*roughness;
	float a2 = a*a;
	float NdotH = max(dot(N, H), 0.0);
	float NdotH2 = NdotH*NdotH;
	float num = a2;
	float denom = (NdotH2 * (a2 - 1.0) + 1.0);
	denom = PI * denom * denom;
	return num / denom;
}
float GeometrySchlickGGX(float NdotV, float roughness)
{
	float r = (roughness + 1.0);
	float k = (r*r) / 8.0;
	float num = NdotV;
	float denom = NdotV * (1.0 - k) + k;
	return num / denom;
}
float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness)
{
	float NdotV = max(dot(N, V), 0.0);
	float NdotL = max(dot(N, L), 0.0);
	float ggx2 = GeometrySchlickGGX(NdotV, roughness);
	float ggx1 = GeometrySchlickGGX(NdotL, roughness);
	return ggx1 * ggx2;
}

vec3 CalcPbrLight(vec3 lightDir, vec3 V, vec3 H, vec3 Normal, vec3 radiance, Material material, vec3 F0){
	vec3 F = fresnelSchlick(max(dot(H, V), 0.0), F0);

	float NDF = DistributionGGX(Normal, H, material.roughness);
	float G = GeometrySmith(Normal, V, lightDir, material.roughness);

	vec3 numerator = NDF * G * F;
	float denominator = 4.0 * max(dot(Normal, V), 0.0) * max(dot(Normal, lightDir), 0.0);
	vec3 specular = numerator / max(denominator, 0.001);

	vec3 kS_local = F;
    vec3 kD_local = (vec3(1.0) - kS_local) * (1.0 - material.metallic);

	float NdotL = max(dot(Normal, lightDir), 0.0);
	vec3 Lo = (kD_local  * material.albedo / PI + specular) * radiance * NdotL;


	return Lo;
}

/////////////////////////// 