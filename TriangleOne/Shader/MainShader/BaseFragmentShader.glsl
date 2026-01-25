#version 330 core
struct Material {
	sampler2D diffuseText[8];
	sampler2D specularText[8];
	sampler2D normalText;

	float shininess;
	bool haveNormalText;
};
uniform Material material;

uniform	int diffuseNbr;
uniform	int specularNbr;

struct SpotLight {
	vec3 position;
	vec3 direction;
	float cutOff;
	float outerCutOff;

	vec3 ambient;
	vec3 diffuse;
	vec3 specular;

	float constant;
	float linear;
	float quadratic;
};
uniform SpotLight spotLight;

struct PointLight {
	vec3 position;

	vec3 ambient;
	vec3 diffuse;
	vec3 specular;

	float constant;
	float linear;
	float quadratic;
};
#define NBR_MAX_POINT_LIGHTS 8  // A setup dans config si possible ( quand il existera) 
uniform PointLight pointLights[NBR_MAX_POINT_LIGHTS];
uniform int nbrPointLight;

struct DirLight{
	vec3 direction;
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};
uniform DirLight dirLight;

//Shadow
uniform sampler2D shadowMap;
uniform samplerCube shadowCubeMap;
uniform float far_plane;

uniform vec3 viewPos;

vec4 CalcFinalDiffuse();
vec4 CalcFinalSpecular();

vec3 CalcDirLight(DirLight light, vec3 viewDir, vec3 norm, vec4 finalDiffuse, vec4 finalSpecular);
vec3 CalcPointLight(PointLight light, vec3 viewDir, vec3 norm, vec4 finalDiffuse, vec4 finalSpecular);
vec3 CalcSpotLight(SpotLight light, vec3 viewDir, vec3 norm, vec4 finalDiffuse, vec4 finalSpecular);

void CheckOpacity(vec4 finalDiffuse, vec4 finalSpecular);

float ShadowDirLight();
float ShadowPointLight(PointLight light);
float ShadowSpotLight(SpotLight light);

in vec3 FragPos;
in vec3 normal;
in vec2 TexCoords;
in vec4 FragPosLightSpace;

out vec4 FragColor;


void main()
{
	vec4 finalDiffuse = CalcFinalDiffuse();
	vec4 finalSpecular = CalcFinalSpecular();

	CheckOpacity(finalDiffuse, finalSpecular);

	vec3 viewDir = normalize(viewPos - FragPos);
	vec3 _ambient, _diffuse, _specular = vec3(0,0,0);


	vec3 ambientLight = dirLight.ambient; // pourquoi pas   
	vec3 final_lightning = ambientLight;

	vec3 norm = normalize(normal);



	if (length(dirLight.ambient + dirLight.diffuse + dirLight.specular ) > 0.001){
		final_lightning += CalcDirLight(dirLight, viewDir, norm, finalDiffuse, finalSpecular); // Une seule lumiere dir dans la scene 
	}

	for (int i = 0; i < nbrPointLight; i++)
	{
		if (length(pointLights[i].ambient + pointLights[i].diffuse + pointLights[i].specular )> 0.001  ){  // On aplique pas le calcul si les lumiere sont eteinte
			final_lightning += CalcPointLight(pointLights[i], viewDir, norm, finalDiffuse, finalSpecular);
		}
	}

	if (length(spotLight.ambient + spotLight.diffuse + spotLight.specular) > 0.001 ){
		//final_lightning += CalcSpotLight(spotLight, viewDir, norm, finalDiffuse, finalSpecular);  // SpotLight non fonctionnelle ( casser intentionnellement ) 
	}

//	float gamma = 2.2;
//	vec3 diffuseColor = pow(finalDiffuse.rgb, vec3(gamma));
//	_diffuse += diffuseColor;
//

	vec3 lighting = final_lightning * vec3(finalDiffuse);


	FragColor = vec4(lighting, 1.0);

	//FragColor = vec4(_ambient + _diffuse + _specular, 1.0);
}


vec3 CalcDirLight(DirLight light, vec3 viewDir, vec3 norm, vec4 finalDiffuse, vec4 finalSpecular)
{
	if (material.haveNormalText){  // Application de la normal map si elle existe 
		norm = texture(material.normalText, TexCoords).rgb;
		norm = normalize(normal * 2.0 - 1.0);
	}

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
		float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess); 


		diffuse = light.diffuse * diff * vec3(finalDiffuse);
		specular = light.specular * spec * vec3(finalSpecular);

		vec3 light_contribution = vec3(0);

		shadow = ShadowDirLight();
		if (specularNbr == 0 ){
			light_contribution = diffuse  * (1.0 - shadow);
		}
		else{
			light_contribution = (diffuse + specular ) * (1.0 - shadow);  //Sale batard on devrait t'envoyer au goulag pour cque ta fais 
		}


		return light_contribution;
	}
	return ambient + (diffuse + specular) * (1.0 - shadow);
}

vec3 CalcPointLight(PointLight light, vec3 viewDir, vec3 norm,vec4 finalDiffuse, vec4 finalSpecular)
{
	vec3 lightDir = normalize(light.position - FragPos);
	float diff = max(dot(lightDir, norm), 0.0);

	float distance = length(light.position - FragPos);
	float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));

	//light.diffuse -= ShadowPointLight(light);

	vec3 diffuse = light.diffuse * diff * attenuation;
	vec3 finalColor = diffuse * finalDiffuse.rgb;

	vec3 light_contribution = vec3(0.0);


	if (specularNbr == 0 ){
		light_contribution = finalColor;
	}
	else{  // Specular
		vec3 reflectDir = reflect(-lightDir, norm); // on inverse lightDir car c'est pas la bonne direction
		float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);  // angle entre le vecteur du reflet et le vecteur qui relie le vertex a la cam
		vec3 specular = light.specular * spec * finalSpecular.rgb * attenuation;

		light_contribution = (finalColor + specular ) * 1.0;  //Sale batard on devrait t'envoyer au goulag pour cque ta fais 
	}


	return light_contribution;
}

vec3 CalcSpotLight(SpotLight light, vec3 viewDir, vec3 norm,vec4 finalDiffuse, vec4 finalSpecular)  // SpottLight non fonctionnelle
{
	vec3 lightDir = normalize(light.position - FragPos);  // Direction entre la source de lumiere et la normal du vertex

	//
    float theta = dot(lightDir, normalize(-light.direction)); 
    float epsilon = light.cutOff - light.outerCutOff;
    float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);
	//
	
	float diff = max(dot(norm, lightDir), 0.0);  // Calcul de l'angle entre la normal et le vec distance 


	vec3 reflectDir = reflect(-lightDir, norm); // on inverse lightDir car c'est pas la bonne direction

	float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);  // angle entre le vecteur du reflet et le vecteur qui relie le vertex a la cam


	vec3 ambient = light.ambient * finalDiffuse.rgb; 
	vec3 diffuse = light.diffuse * diff * finalDiffuse.rgb;
	vec3 specular = light.specular * spec * finalSpecular.rgb;

	// attenuation
	float distance = length(light.position - FragPos);
	float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));

	ambient *= attenuation * intensity;
	diffuse *= attenuation * intensity;
	specular *= attenuation * intensity;

//	float shadow = ShadowSpotLight();
//	vec3 light_contribution = (diffuse + specular) * (1.0 - shadow);

	vec3 light_contribution = vec3(0);
	return light_contribution;
}

vec4 CalcFinalDiffuse(){
	vec4 finalDiffuse = texture(material.diffuseText[0], TexCoords);
	if (diffuseNbr == 0)
		return finalDiffuse;


	for (int i = 1; i < diffuseNbr; i++)
	{
		finalDiffuse += texture(material.diffuseText[i], TexCoords);
	}

	return clamp(finalDiffuse, 0.0, 1.0);
}

vec4 CalcFinalSpecular(){
	vec4 finalSpecular = texture(material.specularText[0], TexCoords);
	if (specularNbr == 0 )
		return finalSpecular;

	for (int i = 1; i < specularNbr; i++)
	{
		finalSpecular += texture(material.specularText[i], TexCoords);
	}

	return clamp(finalSpecular, 0.0, 1.0);;
}

void CheckOpacity(vec4 finalDiffuse, vec4 finalSpecular){
	if(	finalDiffuse.a +  finalSpecular.a < 0.1)
		discard;
}

float ShadowDirLight(){

	vec3 projCoords = FragPosLightSpace.xyz / FragPosLightSpace.w;
	projCoords = projCoords * 0.5 + 0.5;

	if (projCoords.z > 1.0) 
		return 0.0;

	float closestDepth = texture(shadowMap, projCoords.xy).r;
	float currentDepth = projCoords.z ;

	//float bias = 0.005;
	vec3 lightDir = normalize(-dirLight.direction );
    float bias = max(0.05 * (1.0 - dot(normal, lightDir)), 0.005);

	//PCF marche en verifiant les pixel voisin... askip
	float shadow = 0.0;
	vec2 texelSize = 1.0 / textureSize(shadowMap, 0);
	for(int x = -1; x <= 1; ++x)
	{
		for(int y = -1; y <= 1; ++y)
		{
			float pcfDepth = texture(shadowMap, projCoords.xy + vec2(x, y) * texelSize).r;
			shadow += currentDepth - bias > pcfDepth ? 1.0 : 0.0;
		}
	}
	shadow /= 9.0;

	return shadow;
}

float ShadowPointLight(PointLight light){
	vec3 sampleOffsetDirections[20] = vec3[]
	(
		vec3( 1, 1, 1), vec3( 1, -1, 1), vec3(-1, -1, 1), vec3(-1, 1, 1),
		vec3( 1, 1, -1), vec3( 1, -1, -1), vec3(-1, -1, -1), vec3(-1, 1, -1),
		vec3( 1, 1, 0), vec3( 1, -1, 0), vec3(-1, -1, 0), vec3(-1, 1, 0),
		vec3( 1, 0, 1), vec3(-1, 0, 1), vec3( 1, 0, -1), vec3(-1, 0, -1),
		vec3( 0, 1, 1), vec3( 0, -1, 1), vec3( 0, -1, -1), vec3( 0, 1, -1)
	);

	float shadow = 0.0;
	float bias = 0.5;
	float samples = 4.0;
	float offset = 0.1;
	float viewDistance = length(viewPos- FragPos);

	vec3 fragToLight = FragPos - light.position;
	float currentDepth = length(fragToLight);

	float diskRadius = (1.0 + (viewDistance / far_plane)) / 25.0;
	for(int i = 0; i < samples; ++i)
	{
		float closestDepth = texture(shadowCubeMap, fragToLight + sampleOffsetDirections[i] * diskRadius).r;
		closestDepth *= far_plane; // undo mapping [0;1]
		if(currentDepth - bias > closestDepth)
		shadow += 1.0;
	}
	shadow /= samples;

//closestDepth *= far_plane; // undo mapping [0;1]
//
//	vec3 fragToLight = FragPos - light.position;
//	float closestDepth = texture(shadowCubeMap, fragToLight).r;
//	closestDepth *= far_plane;
//
//	float currentDepth = length(fragToLight);
//
//
//	float shadow = currentDepth - bias > closestDepth ? 1.0 : 0.0;

	return shadow;
}

float ShadowSpotLight(SpotLight light){

	return 0.0f;
}