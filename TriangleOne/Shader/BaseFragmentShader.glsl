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
	vec3 viewPosition;
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
	vec3 viewPosition;

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


vec4 CalcFinalDiffuse();
vec4 CalcFinalSpecular();

vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir, vec4 finalDiffuse, vec4 finalSpecular);
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir, vec4 finalDiffuse, vec4 finalSpecular);
vec3 CalcSpotLight(SpotLight light,vec3 normal, vec3 fragPosView, vec3 viewDir, vec4 finalDiffuse, vec4 finalSpecular);



in vec3 normal;
in vec3 FragPosView;
in vec2 TexCoords;

out vec4 FragColor;


void main()
{
	vec4 finalDiffuse = CalcFinalDiffuse();
	vec4 finalSpecular = CalcFinalSpecular();


	vec3 norm = normalize(normal);
	vec3 viewDir = normalize(- FragPosView);
	vec3 result = CalcDirLight(dirLight, norm, viewDir, finalDiffuse, finalSpecular);

	for (int i = 0; i < NR_POINT_LIGHTS; i++)
	{
		result += CalcPointLight(pointLights[i], norm, FragPosView, viewDir, finalDiffuse, finalSpecular);
	}

	result += CalcSpotLight(spotLight, norm, FragPosView, viewDir, finalDiffuse, finalSpecular);

	FragColor = vec4(result, 1.0);
}


vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir,vec4 finalDiffuse, vec4 finalSpecular)
{
	vec3 lightDir = normalize(-light.direction);
	// diffuse shading
	float diff = max(dot(normal, lightDir), 0.0);

	// specular shading
	vec3 reflectDir = reflect(-lightDir, normal);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);

	// combine results
	vec3 ambient = light.ambient * vec3(finalDiffuse);
	vec3 diffuse = light.diffuse * diff * vec3(finalDiffuse);
	vec3 specular = light.specular * spec * vec3(finalSpecular);

	return (ambient + diffuse + specular);
}

vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPosView, vec3 viewDir, vec4 finalDiffuse, vec4 finalSpecular)
{
	vec3 lightDir = normalize(light.viewPosition - FragPosView);  // Direction de la lumiere vers la normal du vertex

	float diff = max(dot(normal, lightDir), 0.0);  // Calcul de l'angle entre la normal et le vec distance 

	vec3 reflectDir = reflect(-lightDir, normal); // on inverse lightDir car c'est pas la bonne direction

	float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);  // angle entre le vecteur du reflet et le vecteur qui relie le vertex a la cam


	vec3 ambient = light.ambient * finalDiffuse.rgb; 
	vec3 diffuse = light.diffuse * diff * finalDiffuse.rgb;
	vec3 specular = light.specular * spec * finalSpecular.rgb;

	// attenuation
	float distance = length(light.viewPosition - FragPosView);
	float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));

	ambient *= attenuation;
	diffuse *= attenuation;
	specular *= attenuation;

	return (ambient + diffuse + specular );
}

vec3 CalcSpotLight(SpotLight light,vec3 normal, vec3 fragPosView, vec3 viewDir,vec4 finalDiffuse, vec4 finalSpecular)
{
	vec3 lightDir = normalize(light.viewPosition - FragPosView);  // Direction entre la source de lumiere et la normal du vertex

	//
	float theta = dot(lightDir, normalize(light.viewPosition -light.direction));
	float epsilon = light.cutOff - light.outerCutOff;
	float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);
	//
	
	float diff = max(dot(normal, lightDir), 0.0);  // Calcul de l'angle entre la normal et le vec distance 


	vec3 reflectDir = reflect(-lightDir, normal); // on inverse lightDir car c'est pas la bonne direction

	float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);  // angle entre le vecteur du reflet et le vecteur qui relie le vertex a la cam


	vec3 ambient = light.ambient * finalDiffuse.rgb; 
	vec3 diffuse = light.diffuse * diff * finalDiffuse.rgb;
	vec3 specular = light.specular * spec * finalSpecular.rgb;

	// attenuation
	float distance = length(- FragPosView);
	float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));

	ambient *= attenuation;
	diffuse *= intensity;
	specular *= intensity;

	return ( ambient + diffuse + specular );
}


vec4 CalcFinalDiffuse(){
	vec4 finalDiffuse = texture(material.diffuseText[0], TexCoords);
	if (diffuseNbr == 0)
		return finalDiffuse;


	for (int i = 1; i < diffuseNbr; i++)
	{
		finalDiffuse = mix(finalDiffuse , texture(material.diffuseText[i], TexCoords), 0.5);
	}

	return finalDiffuse;
}

vec4 CalcFinalSpecular(){
	vec4 finalSpecular = texture(material.specularText[0], TexCoords);
	if (specularNbr == 0 )
		return finalSpecular;

	for (int i = 1; i < specularNbr; i++)
	{
		finalSpecular = mix(finalSpecular , texture(material.specularText[i], TexCoords), 0.5);
	}

	return finalSpecular;
}
