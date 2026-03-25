#version 430 core
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
	vec3 specular;

	float cutOff;
	float outerCutOff;

	float range;
};
#define NBR_MAX_SPOT_LIGHTS 8 // A setup dans config si possible ( quand il existera) 
uniform mat4 spotLightMatrices[NBR_MAX_SPOT_LIGHTS];

struct PointLight {
	vec3 position;

	vec3 ambient;
	vec3 diffuse;
	vec3 specular;

	float range;
};
#define NBR_MAX_POINT_LIGHTS 8  // A setup dans config si possible ( quand il existera) 



struct DirLight{
	vec3 direction;
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
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

vec3 CalcDirLight(DirLight light, vec3 viewDir, vec3 norm, vec4 finalDiffuse, vec4 finalSpecular);
vec3 CalcPointLight(PointLight light, int lightIndex, vec3 viewDir, vec3 norm, vec4 finalDiffuse, vec4 finalSpecular);
vec3 CalcSpotLight(SpotLight light, int lightIndex, vec3 viewDir, vec3 norm,vec4 finalDiffuse, vec4 finalSpecular);

void CheckOpacity(vec4 finalDiffuse, vec4 finalSpecular);

float ShadowDirLight();
float ShadowPointLight(PointLight light, int lightIndex, vec3 norm);
float ShadowSpotLight(SpotLight light, int lightIndex);

in vec3 FragPos;
in vec3 normal;
in vec2 TexCoords;
in vec4 FragPosLightSpace;

out vec4 FragColor;

in mat3 TBN;

void main()
{
	vec4 finalDiffuse = texture(material.diffuseText, TexCoords);
	vec4 finalSpecular = texture(material.specularText, TexCoords);

	CheckOpacity(finalDiffuse, finalSpecular);

	vec3 viewDir = normalize(viewPos - FragPos);
	vec3 _ambient = vec3(0,0,0);
	vec3 _diffuse = vec3(0,0,0);
	vec3 _specular = vec3(0,0,0);


	vec3 ambientLight = dirLight.ambient; // pourquoi pas   
	vec3 final_lightning = ambientLight;

	vec3 norm = vec3(0,0,0);
	if (have_NormalMap){
		norm = texture(material.normalText, TexCoords).rgb;
		norm = normalize(norm * 2.0 - 1.0);
		norm = normalize(TBN * norm);
	}
	else {
		norm = normalize(normal);
	}



	final_lightning += CalcDirLight(dirLight, viewDir, norm, finalDiffuse, finalSpecular); // Une seule lumiere dir dans la scene 

	for (int i = 0; i < nbrPointLight; i++)
	{
		if (length(pointLights[i].ambient + pointLights[i].diffuse + pointLights[i].specular )> 0.001  ){  // On aplique pas le calcul si les lumiere sont eteinte
			final_lightning += CalcPointLight(pointLights[i], i, viewDir, norm, finalDiffuse, finalSpecular);
		}
	}
	for (int i = 0; i < nbrSpotLight; i++){
		if (length(spotLights[i].ambient + spotLights[i].diffuse + spotLights[i].specular) > 0.001 ){
			final_lightning += CalcSpotLight(spotLights[i], i, viewDir, norm, finalDiffuse, finalSpecular);
		}
	}


	vec3 lighting = final_lightning * vec3(finalDiffuse);

	//Correction gamma
	float gamma = 2.2;
	vec3 mapped = lighting / (lighting + vec3(1.0));
	mapped = pow(mapped, vec3(1.0 / 2.2));
	FragColor = vec4(mapped, 1.0);

	//FragColor = vec4(lighting, 1.0);
	//FragColor = vec4(vec3(ShadowPointLight(pointLights[0], norm)), 1.0);
}


vec3 CalcDirLight(DirLight light, vec3 viewDir, vec3 norm, vec4 finalDiffuse, vec4 finalSpecular)
{
	vec3 lightDir = normalize(-light.direction);


	// Initialisation par defaut
	vec3 ambient = light.ambient * vec3(finalDiffuse);
    vec3 diffuse = vec3(0.0);
    vec3 specular = vec3(0.0);
    float shadow = 0.0;

	float diff = max(dot(norm, lightDir), 0.0);
	if (diff > 0.0) {
		// diffuse shading
		vec3 reflectDir = reflect(- lightDir, norm);
		float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32.0f); 


		diffuse = light.diffuse * diff * vec3(finalDiffuse);
		specular = light.specular * spec * vec3(finalSpecular);

		vec3 light_contribution = vec3(0);

		shadow = ShadowDirLight();
		if (!have_Specular){
			light_contribution = diffuse  * (1.0 - shadow);
		}
		else{
			light_contribution = (diffuse + specular ) * (1.0 - shadow);
		}


		return light_contribution;
	}
	return ambient + (diffuse + specular) * (1.0 - shadow);
}

vec3 CalcPointLight(PointLight light, int lightIndex,vec3 viewDir, vec3 norm,vec4 finalDiffuse, vec4 finalSpecular)
{
	vec3 lightDir = normalize(light.position - FragPos);


	float distance = length(light.position - FragPos);
	if(distance > light.range) { 
        return vec3(0.0, 0.0, 0.0);  // A modiff
    }
	// C'est l'heure de la triche 
	float distFrac = distance / light.range;
	float attenuation = clamp(1.0 - (distFrac * distFrac * distFrac * distFrac), 0.0, 1.0);

	attenuation = attenuation * attenuation;
	attenuation = attenuation / (distance * distance + 1.0);

	float diff = dot(norm, lightDir) * 0.5 + 0.5;
	diff = diff * diff;
	vec3 diffuse = light.diffuse * diff;


	 // Specular
	vec3 V = normalize(viewPos - FragPos);
	vec3 halfwayVec = normalize(lightDir + V);
	float spec = pow(max(dot(viewDir, halfwayVec), 0.0), 32.0f); // angle entre le vecteur du reflet et le vecteur qui relie le vertex a la cam
	vec3 specular = light.specular * spec * finalSpecular.rgb * attenuation;

	vec3 finalColor = diffuse * finalDiffuse.rgb;

	float shadow = ShadowPointLight(light, lightIndex, norm);
    
	float lightModifier = (1.0 - shadow); 

	vec3 light_contribution = vec3(0.0);

	if (!have_Specular){
		light_contribution = finalColor * attenuation * lightModifier ;
	}
	else{ 
		light_contribution = (finalColor + specular ) * attenuation * lightModifier ;
	}

	return light_contribution / (light_contribution + vec3(1.0));
}

vec3 CalcSpotLight(SpotLight light, int lightIndex, vec3 viewDir, vec3 norm, vec4 finalDiffuse, vec4 finalSpecular)  // SpottLight non fonctionnelle
{
	vec3 lightDir = normalize(light.position - FragPos);  // Direction entre la source de lumiere et la normal du vertex

	//
    float theta = dot(lightDir, normalize(-light.direction)); 
    float epsilon = light.cutOff - light.outerCutOff;
    float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);
	//
	
	float diff = max(dot(norm, lightDir), 0.0);  // Calcul de l'angle entre la normal et le vec distance 


	vec3 reflectDir = reflect(-lightDir, norm); // on inverse lightDir car c'est pas la bonne direction

	float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32.0f);  // angle entre le vecteur du reflet et le vecteur qui relie le vertex a la cam


	vec3 ambient = light.ambient * finalDiffuse.rgb; 
	vec3 diffuse = light.diffuse * diff * finalDiffuse.rgb;
	vec3 specular = light.specular * spec * finalSpecular.rgb;

	// attenuation
	float distance = length(light.position - FragPos);
	float attenuation = clamp(1.0 - (distance / light.range), 0.0, 1.0);

	ambient *= attenuation * intensity;
	diffuse *= attenuation * intensity;
	specular *= attenuation * intensity;

	float shadow = ShadowSpotLight(light, lightIndex);
	//vec3 light_contribution = ambient + (diffuse + specular);
//	vec3 light_contribution = (diffuse + specular) * (1.0 - shadow);

	vec3 light_contribution = vec3(0.0);

	if (!have_Specular){
		light_contribution = ambient + (diffuse) * (1.0 - shadow);
	}
	else{ 
		light_contribution = ambient + (diffuse + specular) * (1.0 - shadow);
	}

	return light_contribution;
}

//vec4 CalcFinalDiffuse(){
//	vec4 finalDiffuse = texture(material.diffuseText[0], TexCoords);
//	if (diffuseNbr == 0)
//		return finalDiffuse;
//
//
//	for (int i = 1; i < diffuseNbr; i++)
//	{
//		finalDiffuse += texture(material.diffuseText[i], TexCoords);
//	}
//
//	return clamp(finalDiffuse, 0.0, 1.0);
//}

//vec4 CalcFinalSpecular(){
//	vec4 finalSpecular = texture(material.specularText[0], TexCoords);
//	if (specularNbr == 0 )
//		return finalSpecular;
//
//	for (int i = 1; i < specularNbr; i++)
//	{
//		finalSpecular += texture(material.specularText[i], TexCoords);
//	}
//
//	return clamp(finalSpecular, 0.0, 1.0);;
//}

void CheckOpacity(vec4 finalDiffuse, vec4 finalSpecular){
	if(	finalDiffuse.a +  finalSpecular.a < 0.1)
		discard;
}

float GetDirShadowMapValue(int index, vec3 dir) {
    if (index == 0) return texture(shadowMap, dir);
    return 0.0;
}

float ShadowDirLight(){

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

float ShadowPointLight(PointLight light, int lightIndex, vec3 norm){
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
	float samples = 20.0;
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

float ShadowSpotLight(SpotLight light, int lightIndex){
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