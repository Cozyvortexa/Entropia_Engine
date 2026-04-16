#version 330 core
uniform samplerCube environmentMap;

in vec3 TexCoords;

out vec4 FragColor;

void main()
{
	vec3 envColor = texture(environmentMap, TexCoords).rgb;
	envColor = envColor / (envColor + vec3(1.0));
	envColor = pow(envColor, vec3(1.0/2.2));
	FragColor = vec4(envColor, 1.0);
}