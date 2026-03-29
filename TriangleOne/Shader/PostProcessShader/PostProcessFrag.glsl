#version 330 core

in vec2 TexCoords;
uniform sampler2D screenTexture;

out vec4 FragColor;

void main()
{
	vec4 color = texture(screenTexture, TexCoords);
	float gamma = 2.2;
	vec3 mapped = color.rgb  / (color.rgb  + vec3(1.0));
	mapped = pow(mapped, vec3(1.0 / gamma));

	FragColor = vec4(mapped, color.a);
}