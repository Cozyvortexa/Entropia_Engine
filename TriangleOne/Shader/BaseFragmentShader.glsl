#version 330 core
struct Material {
	
	sampler2D diffuseText[8];
	sampler2D SpecularText[8];
	int diffuseNbr;
	int specularNbr;

	sampler2D diffuse;
	sampler2D specular;
	float shininess;
};
uniform Material material;

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


vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir);
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir);
vec3 CalcSpotLight(SpotLight light,vec3 normal, vec3 fragPosView, vec3 viewDir);

in vec3 normal;
in vec3 FragPosView;
in vec2 TexCoords;

out vec4 FragColor;


void main()
{
	vec3 norm = normalize(normal);
	vec3 viewDir = normalize(- FragPosView);
	vec3 result = CalcDirLight(dirLight, norm, viewDir);

	for(int i = 0; i < NR_POINT_LIGHTS; i++)
	{
		result += CalcPointLight(pointLights[i], norm, FragPosView, viewDir);
	}
	
	result += CalcSpotLight(spotLight, norm, FragPosView, viewDir);

	FragColor = vec4(result, 1.0);
}


vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir)
{
	vec3 lightDir = normalize(-light.direction);
	// diffuse shading
	float diff = max(dot(normal, lightDir), 0.0);

	// specular shading
	vec3 reflectDir = reflect(-lightDir, normal);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);

	// combine results
	vec3 ambient = light.ambient * vec3(texture(material.diffuse,TexCoords));
	vec3 diffuse = light.diffuse * diff * vec3(texture(material.diffuse, TexCoords));
	vec3 specular = light.specular * spec * vec3(texture(material.specular, TexCoords));

	return (ambient + diffuse + specular);
}

vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPosView, vec3 viewDir)
{
	vec3 lightDir = normalize(light.viewPosition - FragPosView);  // Direction de la lumiere vers la normal du vertex

	float diff = max(dot(normal, lightDir), 0.0);  // Calcul de l'angle entre la normal et le vec distance 

	vec3 reflectDir = reflect(-lightDir, normal); // on inverse lightDir car c'est pas la bonne direction

	float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);  // angle entre le vecteur du reflet et le vecteur qui relie le vertex a la cam


	vec3 ambient = light.ambient * texture(material.diffuse, TexCoords).rgb; 
	vec3 diffuse = light.diffuse * diff * texture(material.diffuse, TexCoords).rgb;
	vec3 specular = light.specular * spec * texture(material.specular, TexCoords).rgb;

	// attenuation
	float distance = length(light.viewPosition - FragPosView);
	float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));

	ambient *= attenuation;
	diffuse *= attenuation;
	specular *= attenuation;

	return (ambient + diffuse + specular );
}

vec3 CalcSpotLight(SpotLight light,vec3 normal, vec3 fragPosView, vec3 viewDir)
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


	vec3 ambient = light.ambient * texture(material.diffuse, TexCoords).rgb; 
	vec3 diffuse = light.diffuse * diff * texture(material.diffuse, TexCoords).rgb;
	vec3 specular = light.specular * spec * texture(material.specular, TexCoords).rgb;

	// attenuation
	float distance = length(- FragPosView);
	float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));

	ambient *= attenuation;
	diffuse *= intensity;
	specular *= intensity;

	return ( ambient + diffuse + specular );
}