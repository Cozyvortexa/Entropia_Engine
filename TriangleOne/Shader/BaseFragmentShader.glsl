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
out vec4 FragColor;

uniform vec3 objectColor;
uniform vec3 lightColor;
uniform vec3 lightPosView;

in vec3 normal;
in vec3 FragPosView;


void main()
{
	float ambientStrength = 0.1;
	vec3 ambient = ambientStrength * lightColor;  

	vec3 norm = normalize(normal);  // Tout est normaliser, on veut uniquement la direction
	vec3 lightDir = normalize(lightPosView - FragPosView);  // Distance entre la lumiere et la normal du vertex

	float diff = max(dot(norm, lightDir), 0.0);  // Calcul de l'angle entre la normal et le vec distance 
	vec3 diffuse = diff * lightColor;


	float specularStrength = 0.5;
	vec3 viewDir = normalize( - FragPosView);  // vecteur du vertex vers camera
	vec3 reflectDir = reflect(-lightDir, norm); // on inverse lightDir car c'est pas la bonne direction

	float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);  // angle entre le vecteur du reflet et le vecteur qui relie le vertex a la cam
	vec3 specular = specularStrength * spec * lightColor;


	vec3 result = (ambient + diffuse + specular) * objectColor;
	FragColor = vec4(result, 1.0);
}