#version 430 core
layout (location = 0) in vec2 aPos;
layout (location = 1) in vec2 aTexCoords;

out vec2 TexCoords;

uniform mat4 lightSpaceMatrix;
uniform sampler2D gPosition;

out vec4 FragPosLightSpace;

void main()
{
	gl_Position = vec4(aPos.x, aPos.y, 0.0, 1.0);
	TexCoords = aTexCoords;

	vec3 FragPos = texture(gPosition, TexCoords).rgb;
	FragPosLightSpace = lightSpaceMatrix * vec4(FragPos, 1.0);
}