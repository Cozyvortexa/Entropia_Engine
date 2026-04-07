#version 430 core
layout (location = 0) out vec4 FragColor;
layout (location = 1) out vec4 BrightColor;


struct Material {
	sampler2D diffuseText;
	sampler2D specularText;
	sampler2D normalText;

	float shininess;  // Not use, 32.0f
};
uniform Material material;

uniform bool have_NormalMap;
uniform	bool have_Specular;

struct SpotLight {
	vec3 position;
	vec3 direction;

	vec3 ambient;
	vec3 diffuse;
	vec3 Specular;

	float cutOff;
	float outerCutOff;

	float range;
};
#define NBR_MAX_SPOT_LIGHTS 8
uniform mat4 spotLightMatrices[NBR_MAX_SPOT_LIGHTS];

struct PointLight {
	vec3 position;

	vec3 ambient;
	vec3 diffuse;
	vec3 Specular;

	float range;
};
#define NBR_MAX_POINT_LIGHTS 8

struct DirLight{
	vec3 direction;
	vec3 ambient;
	vec3 diffuse;
	vec3 Specular;
};


//SSBO
layout (std430, binding = 1) buffer Lights
{
	DirLight dirLight;
	PointLight pointLights[NBR_MAX_POINT_LIGHTS];
	SpotLight spotLights[NBR_MAX_SPOT_LIGHTS];
	int nbrPointLight;
	int nbrSpotLight;
};


//Shadow
uniform sampler2DShadow shadowMap;
uniform samplerCubeShadow shadowCubeMaps[NBR_MAX_POINT_LIGHTS];
uniform sampler2DShadow shadowMapSpot [NBR_MAX_SPOT_LIGHTS];
uniform float far_plane;

uniform vec3 viewPos;

//vec4 CalcFinalDiffuse();
//vec4 CalcFinalSpecular();

vec3 CalcDirLight(DirLight light, vec3 viewDir, vec3 FragPos, vec3 Normal, vec3 Albedo, float Specular, vec4 FragPosLightSpace, float ambientOcclusion);
vec3 CalcPointLight(PointLight light, int lightIndex, vec3 viewDir, vec3 FragPos, vec3 Normal, vec3 Albedo, float Specular, float ambientOcclusion);
vec3 CalcSpotLight(SpotLight light, int lightIndex, vec3 viewDir, vec3 FragPos, vec3 Normal, vec3 Albedo, float Specular, float ambientOcclusion);

float ShadowDirLight(vec4 FragPosLightSpace);
float ShadowPointLight(PointLight light, int lightIndex, vec3 FragPos, vec3 Normal);
float ShadowSpotLight(SpotLight light, int lightIndex, vec3 FragPos);

in vec2 TexCoords;

uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D gAlbedo;
uniform sampler2D gDepth;
uniform sampler2D ssaoTexture;

uniform bool ssao_Toogle;


uniform mat4 lightSpaceMatrix;

void main()
{
	vec3 FragPos = texture(gPosition, TexCoords).rgb;
	vec3 Normal = texture(gNormal, TexCoords).rgb;
	vec3 Albedo = texture(gAlbedo, TexCoords).rgb;
	Albedo = pow(Albedo, vec3(2.2));
	float Specular = texture(gAlbedo, TexCoords).a;
	float ambientOcclusion = 1.0f;
	if (ssao_Toogle){
		ambientOcclusion = texture(ssaoTexture, TexCoords).r;
	}

	float depth = texture(gDepth, TexCoords).r;
	if(depth == 1.0) {
		FragColor = vec4(0.2, 0.3, 0.3, 1.0);
		return;
	}

	vec3 viewDir = normalize(viewPos - FragPos);
	vec3 _ambient = vec3(0,0,0);
	vec3 _diffuse = vec3(0,0,0);
	vec3 _specular = vec3(0,0,0);

	vec3 final_lightning = vec3(0,0,0);

	vec4 FragPosLightSpace = lightSpaceMatrix * vec4(FragPos, 1.0);


	final_lightning += CalcDirLight(dirLight, viewDir, FragPos, Normal, Albedo, Specular, FragPosLightSpace, ambientOcclusion); // Une seule lumiere dir dans la scene 

	for (int i = 0; i < nbrPointLight; i++)
	{
		if (length(pointLights[i].ambient + pointLights[i].diffuse + pointLights[i].Specular )> 0.001  ){  // On aplique pas le calcul si les lumiere sont eteinte
			final_lightning += CalcPointLight(pointLights[i], i, viewDir, FragPos, Normal, Albedo, Specular, ambientOcclusion);
		}
	}
	for (int i = 0; i < nbrSpotLight; i++){
		if (length(spotLights[i].ambient + spotLights[i].diffuse + spotLights[i].Specular) > 0.001 ){
			final_lightning += CalcSpotLight(spotLights[i], i, viewDir, FragPos, Normal, Albedo, Specular, ambientOcclusion);
		}
	}


	
	FragColor = vec4(final_lightning , 1.0);

	//Bloom
	float brightness = dot(FragColor.rgb, vec3(0.2126, 0.7152, 0.0722));
	if(brightness > 1.0)
		BrightColor = vec4(FragColor.rgb, 1.0);
	else
		BrightColor = vec4(0.0, 0.0, 0.0, 1.0);

}


vec3 CalcDirLight(DirLight light, vec3 viewDir, vec3 FragPos, vec3 Normal, vec3 Albedo, float Specular, vec4 FragPosLightSpace, float ambientOcclusion)
{
	vec3 lightDir = normalize(light.direction);


	// Initialisation par defaut
	vec3 ambient = light.ambient * vec3(Albedo) * ambientOcclusion;
    vec3 diffuse = vec3(0.0);
    float shadow = 0.0;

	vec3 currentSpecular = vec3(0);
	float diff = max(dot(Normal, lightDir), 0.0);
	if (diff > 0.0) {
		// diffuse shading
		vec3 reflectDir = reflect(- lightDir, Normal);
		float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32.0f); 


		diffuse = light.diffuse * diff * vec3(Albedo);
		currentSpecular = light.Specular * spec * vec3(Specular);

		vec3 light_contribution = vec3(0);

		shadow = ShadowDirLight(FragPosLightSpace);
		if (!have_Specular){
			light_contribution = ambient + diffuse  * (1.0 - shadow);
		}
		else{
			light_contribution = ambient + (diffuse + currentSpecular ) * (1.0 - shadow);
		}


		return light_contribution;
	}
	return ambient + (diffuse + currentSpecular) * (1.0 - shadow);
}

vec3 CalcPointLight(PointLight light, int lightIndex, vec3 viewDir, vec3 FragPos,  vec3 Normal, vec3 Albedo, float Specular, float ambientOcclusion)
{
	vec3 lightDir = normalize(light.position - FragPos);

	float distance = length(light.position - FragPos);
	if(distance > light.range) { 
        return vec3(0.0, 0.0, 0.0);  // A modiff
    }
	vec3 ambient = light.ambient * vec3(Albedo) * ambientOcclusion;

	float distFrac = distance / light.range;
	float attenuation = clamp(1.0 - (distFrac * distFrac * distFrac * distFrac), 0.0, 1.0);

	attenuation = attenuation * attenuation;
	attenuation = attenuation / (distance * distance + 1.0);

	float diff = dot(Normal, lightDir) * 0.5 + 0.5;
	diff = diff * diff;
	vec3 diffuse = light.diffuse * diff;


	 // Specular
	vec3 V = normalize(viewPos - FragPos);
	vec3 halfwayVec = normalize(lightDir + V);
	float spec = pow(max(dot(viewDir, halfwayVec), 0.0), 32.0f); // angle entre le vecteur du reflet et le vecteur qui relie le vertex a la cam
	vec3 currentSpecular = light.Specular * spec * Specular * attenuation;

	vec3 finalColor = diffuse * Albedo.rgb;

	float shadow = ShadowPointLight(light, lightIndex, FragPos, Normal);
    
	float lightModifier = (1.0 - shadow); 

	vec3 light_contribution = vec3(0.0);

	if (!have_Specular){
		light_contribution = ambient + finalColor * attenuation * lightModifier ;
	}
	else{ 
		light_contribution = ambient + (finalColor + currentSpecular) * attenuation * lightModifier ;
	}

	return light_contribution;
}

vec3 CalcSpotLight(SpotLight light, int lightIndex, vec3 viewDir, vec3 FragPos, vec3 Normal, vec3 Albedo, float Specular, float ambientOcclusion) 
{
	vec3 lightDir = normalize(light.position - FragPos);  // Direction entre la source de lumiere et la normal du vertex

	//
    float theta = dot(lightDir, normalize(-light.direction)); 
    float epsilon = light.cutOff - light.outerCutOff;
    float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);
	//
	
	float diff = max(dot(Normal, lightDir), 0.0);  // Calcul de l'angle entre la normal et le vec distance 


	vec3 reflectDir = reflect(-lightDir, Normal); // on inverse lightDir car c'est pas la bonne direction

	float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32.0f);  // angle entre le vecteur du reflet et le vecteur qui relie le vertex a la cam


	vec3 ambient = light.ambient * vec3(Albedo) * ambientOcclusion;
	vec3 diffuse = light.diffuse * diff * Albedo.rgb;
	vec3 currentSpecular = light.Specular * spec * Specular;

	// attenuation
	float distance = length(light.position - FragPos);
	float attenuation = clamp(1.0 - (distance / light.range), 0.0, 1.0);

	ambient *= attenuation * intensity;
	diffuse *= attenuation * intensity;
	currentSpecular *= attenuation * intensity;

	float shadow = ShadowSpotLight(light, lightIndex, FragPos);
	//vec3 light_contribution = ambient + (diffuse + Specular);
//	vec3 light_contribution = (diffuse + Specular) * (1.0 - shadow);

	vec3 light_contribution = vec3(0.0);

	if (!have_Specular){
		light_contribution = ambient + (diffuse) * (1.0 - shadow);
	}
	else{ 
		light_contribution = ambient + (diffuse + currentSpecular) * (1.0 - shadow);
	}

	return light_contribution;
}


float GetDirShadowMapValue(int index, vec3 dir) {
    if (index == 0) return texture(shadowMap, dir);
    return 0.0;
}

float ShadowDirLight(vec4 FragPosLightSpace){

	vec3 projCoords = FragPosLightSpace.xyz / FragPosLightSpace.w;
	projCoords = projCoords * 0.5 + 0.5;

	if (projCoords.z > 1.0) 
		return 0.0;


	vec3 lightDir = normalize(-dirLight.direction );
    float bias = 0.005;

	float shadow = 0.0;
	vec2 texelSize = 1.0 / textureSize(shadowMap, 0);
	for(int x = -1; x <= 1; ++x)
	{
		for(int y = -1; y <= 1; ++y)
		{
			shadow += texture(shadowMap, vec3(projCoords.xy + vec2(x, y) * texelSize, projCoords.z - bias));
		}
	}
	shadow /= 9.0;

	return 1.0 - shadow;
}

float GetShadowMapValue(int index, vec4 dirDepth) {
    if (index == 0) return texture(shadowCubeMaps[0], dirDepth);
    if (index == 1) return texture(shadowCubeMaps[1], dirDepth);
    if (index == 2) return texture(shadowCubeMaps[2], dirDepth);
    if (index == 3) return texture(shadowCubeMaps[3], dirDepth);
    if (index == 4) return texture(shadowCubeMaps[4], dirDepth);
    if (index == 5) return texture(shadowCubeMaps[5], dirDepth);
    if (index == 6) return texture(shadowCubeMaps[6], dirDepth);
    if (index == 7) return texture(shadowCubeMaps[7], dirDepth);
    return 0.0;
}

float ShadowPointLight(PointLight light, int lightIndex, vec3 FragPos, vec3 Normal){
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
        return 1.0; // Not in shadow if out of range

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
        shadow += GetShadowMapValue(lightIndex, vec4(dir, currentDepthNormalized));
	}
    
    // Average the results
	shadow /= samples;

	return 1.0 - shadow;
}

float GetSpotShadowMapValue(int index, vec3 coords) {
    if (index == 0) return texture(shadowMapSpot[0], coords);
    if (index == 1) return texture(shadowMapSpot[1], coords);
    if (index == 2) return texture(shadowMapSpot[2], coords);
    if (index == 3) return texture(shadowMapSpot[3], coords);
    if (index == 4) return texture(shadowMapSpot[4], coords);
    if (index == 5) return texture(shadowMapSpot[5], coords);
    if (index == 6) return texture(shadowMapSpot[6], coords);
    if (index == 7) return texture(shadowMapSpot[7], coords);
    return 0.0;
}

float ShadowSpotLight(SpotLight light, int lightIndex, vec3 FragPos){
	vec4 fragPosSpotSpace = spotLightMatrices[lightIndex] * vec4(FragPos, 1.0);

    vec3 projCoords = fragPosSpotSpace.xyz / fragPosSpotSpace.w;
	projCoords = projCoords * 0.5 + 0.5;

	if (projCoords.z > 1.0 || projCoords.z < 0.0
	|| projCoords.x < 0.0 || projCoords.x > 1.0
	|| projCoords.y < 0.0 || projCoords.y > 1.0)
		return 0.0;

	float currentDepth = projCoords.z ;

	vec3 lightDir = normalize(light.position - FragPos);
    float bias = 0.005;

	float shadow = 0.0;
	vec2 texelSize = 1.0 / textureSize(shadowMapSpot[lightIndex], 0);
	for(int x = -1; x <= 1; ++x) {
        for(int y = -1; y <= 1; ++y) {
            vec2 offset = vec2(x, y) * texelSize;
            vec3 shadowCoords = vec3(projCoords.xy + offset, projCoords.z - bias);
            
            shadow += GetSpotShadowMapValue(lightIndex, shadowCoords);
        }
    }
	shadow /= 9.0;

	return 1.0 - shadow;
}