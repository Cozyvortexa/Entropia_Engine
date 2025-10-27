#version 330 core
uniform vec3 cameraPos;
uniform samplerCube skybox;

in vec3 Normal;
in vec3 Position;

out vec4 FragColor;

void main()
{
	vec3 I = normalize(Position - cameraPos);
	vec3 R = reflect(I, normalize(Normal));
	FragColor = vec4(texture(skybox, R).rgb, 1.0);
}