#version 430 core
uniform samplerCube environmentMap;

in vec3 TexCoords;

out vec4 FragColor;

void main()
{
	vec3 envColor =  textureLod(environmentMap, TexCoords, 1.2).rgb;
	FragColor = vec4(envColor, 1.0);
}