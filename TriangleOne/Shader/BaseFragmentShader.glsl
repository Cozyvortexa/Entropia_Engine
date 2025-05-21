//#version 330 core
//out vec4 FragColor;
//
////in vec3 ourColor;
//in vec2 TexCoord;
//
//uniform sampler2D ourTexture;
//
//void main()
//{
//	//FragColor = vec4(ourColor, 1.0);
//	FragColor = texture(ourTexture, TexCoord);
//}

#version 330 core
struct Material {

sampler2D diffuse;
sampler2D specular;
float shininess;
};
uniform Material material;

struct Light {

vec3 position;
vec3 ambient;
vec3 diffuse;
vec3 specular;
};
uniform Light light;

uniform vec3 lightPosView;

in vec3 normal;
in vec3 FragPosView;
in vec2 TexCoords;

out vec4 FragColor;


void main()
{
	vec3 ambient = light.ambient * vec3(texture(material.diffuse, TexCoords)); 

	vec3 norm = normalize(normal);  // Tout est normaliser, on veut uniquement la direction
	vec3 lightDir = normalize(lightPosView - FragPosView);  // Distance entre la lumiere et la normal du vertex

	float diff = max(dot(norm, lightDir), 0.0);  // Calcul de l'angle entre la normal et le vec distance 
	vec3 diffuse = light.diffuse * diff * vec3(texture(material.diffuse, TexCoords));

	
	vec3 viewDir = normalize( - FragPosView);  // vecteur du vertex vers camera
	vec3 reflectDir = reflect(-lightDir, norm); // on inverse lightDir car c'est pas la bonne direction

	float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);  // angle entre le vecteur du reflet et le vecteur qui relie le vertex a la cam
	vec3 specular = light.specular * spec * vec3(texture(material.specular, TexCoords));


	vec3 result = ambient + diffuse + specular;
	FragColor = vec4(result, 1.0);
}