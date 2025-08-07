#version 330 core
struct Material {
	sampler2D diffuseText[8];
	sampler2D specularText[8];

	float shininess;
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
#define NR_POINT_LIGHTS 4
uniform PointLight pointLights[NR_POINT_LIGHTS];

struct DirLight{
	vec3 direction;
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};
uniform DirLight dirLight;

uniform sampler2D shadowMap;

uniform vec3 viewPos;

vec4 CalcFinalDiffuse();
vec4 CalcFinalSpecular();

void CalcDirLight(DirLight light, vec3 viewDir, vec3 norm, vec4 finalDiffuse, vec4 finalSpecular,   inout vec3 _ambient,inout vec3 _diffuse, inout vec3 _specular);
void CalcPointLight(PointLight light, vec3 viewDir, vec3 norm, vec4 finalDiffuse, vec4 finalSpecular,   inout vec3 _ambient, inout vec3 _diffuse, inout vec3 _specular);
void CalcSpotLight(SpotLight light, vec3 viewDir, vec3 norm, vec4 finalDiffuse, vec4 finalSpecular,   inout vec3 _ambient, inout vec3 _diffuse, inout vec3 _specular);

void CheckOpacity(vec4 finalDiffuse, vec4 finalSpecular);

float ShadowCalculation();

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

	vec3 norm = normalize(normal);


	if (length(dirLight.ambient + dirLight.diffuse + dirLight.specular ) > 0.001){
		CalcDirLight(dirLight, viewDir, norm, finalDiffuse, finalSpecular, _ambient, _diffuse, _specular );
	}

	for (int i = 0; i < NR_POINT_LIGHTS; i++)
	{
		if (length(pointLights[i].ambient + pointLights[i].diffuse + pointLights[i].specular )> 0.001  ){  // On aplique pas le calcul si les lumiere sont eteint
			CalcPointLight(pointLights[i], viewDir, norm, finalDiffuse, finalSpecular, _ambient, _diffuse, _specular);
		}
	}

	if (length(spotLight.ambient + spotLight.diffuse + spotLight.specular) > 0.001 ){
		CalcSpotLight(spotLight, viewDir, norm, finalDiffuse, finalSpecular, _ambient, _diffuse, _specular);
	}

//	float gamma = 2.2;
//	vec3 diffuseColor = pow(finalDiffuse.rgb, vec3(gamma));
//	_diffuse += diffuseColor;
//
	float shadow = ShadowCalculation();
	vec3 lighting = _ambient + (1.0 - shadow) * (_diffuse +  _specular);


	FragColor = vec4(lighting, 1.0);

	//FragColor = vec4(_ambient + _diffuse + _specular, 1.0);
}


void CalcDirLight(DirLight light, vec3 viewDir, vec3 norm, vec4 finalDiffuse, vec4 finalSpecular,   inout vec3 _ambient, inout vec3 _diffuse, inout vec3 _specular)
{
	vec3 lightDir = normalize(-light.direction);
	// diffuse shading
	float diff = max(dot(norm, lightDir), 0.0);
	if (diff > 0.0) {
		// diffuse shading
		vec3 reflectDir = reflect(- lightDir, norm);
		float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess); 

		// combine results
		vec3 ambient = light.ambient * vec3(finalDiffuse);
		vec3 diffuse = light.diffuse * diff * vec3(finalDiffuse);
		vec3 specular = light.specular * spec * vec3(finalSpecular);


		_ambient += ambient;
		_diffuse += diffuse;
		_specular += specular;
	}
}

void CalcPointLight(PointLight light, vec3 viewDir, vec3 norm,vec4 finalDiffuse, vec4 finalSpecular,   inout vec3 _ambient, inout vec3 _diffuse, inout vec3 _specular)
{
	vec3 lightDir = normalize(light.position - FragPos);  // Direction de la lumiere vers la normal du vertex

	float diff = max(dot(norm, lightDir), 0.0);  // Calcul de l'angle entre la normal et le vec distance 

	vec3 reflectDir = reflect(-lightDir, norm); // on inverse lightDir car c'est pas la bonne direction

	float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);  // angle entre le vecteur du reflet et le vecteur qui relie le vertex a la cam


	vec3 ambient = light.ambient * finalDiffuse.rgb; 
	vec3 diffuse = light.diffuse * diff * finalDiffuse.rgb;
	vec3 specular = light.specular * spec * finalSpecular.rgb;

	// attenuation
	float distance = length(light.position - FragPos);
	float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));

	ambient *= attenuation;
	diffuse *= attenuation;
	specular *= attenuation;

	_ambient += ambient;
	_diffuse += diffuse;
	_specular += specular;
}

void CalcSpotLight(SpotLight light, vec3 viewDir, vec3 norm,vec4 finalDiffuse, vec4 finalSpecular,   inout vec3 _ambient, inout vec3 _diffuse, inout vec3 _specular)
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

	_ambient += ambient;
	_diffuse += diffuse;
	_specular += specular;
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

float ShadowCalculation(){

	vec3 projCoords = FragPosLightSpace.xyz / FragPosLightSpace.w;
	projCoords = projCoords * 0.5 + 0.5;

	if (projCoords.z > 1.0) 
		return 0.0;

	float closestDepth = texture(shadowMap, projCoords.xy).r;
	float currentDepth = projCoords.z ;

	//float bias = 0.005;
	vec3 lightDir = normalize(-dirLight.direction );
    float bias = max(0.05 * (1.0 - dot(normal, lightDir)), 0.005);


	float shadow = 0.0;
	vec2 texelSize = 1.0 / textureSize(shadowMap, 0);
	for(int x = -1; x <= 1; ++x)
	{
		for(int y = -1; y <= 1; ++y)
		{
			float pcfDepth = texture(shadowMap, projCoords.xy + vec2(x, y) *
			texelSize).r;
			shadow += currentDepth - bias > pcfDepth ? 1.0 : 0.0;
		}
	}
	shadow /= 9.0;

	return shadow;
}