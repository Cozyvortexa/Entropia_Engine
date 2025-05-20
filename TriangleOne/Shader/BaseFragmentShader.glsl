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

vec3 ambient;
vec3 diffuse;
vec3 specular;
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

out vec4 FragColor;


void main()
{
	vec3 ambient = material.ambient * light.ambient;  

	vec3 norm = normalize(normal);  // Tout est normaliser, on veut uniquement la direction
	vec3 lightDir = normalize(lightPosView - FragPosView);  // Distance entre la lumiere et la normal du vertex

	float diff = max(dot(norm, lightDir), 0.0);  // Calcul de l'angle entre la normal et le vec distance 
	vec3 diffuse = (diff * material.diffuse )* light.diffuse;

	
	vec3 viewDir = normalize( - FragPosView);  // vecteur du vertex vers camera
	vec3 reflectDir = reflect(-lightDir, norm); // on inverse lightDir car c'est pas la bonne direction

	float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);  // angle entre le vecteur du reflet et le vecteur qui relie le vertex a la cam
	vec3 specular = (material.specular * spec ) * light.specular;


	vec3 result = ambient + diffuse + specular;
	FragColor = vec4(result, 1.0);
}