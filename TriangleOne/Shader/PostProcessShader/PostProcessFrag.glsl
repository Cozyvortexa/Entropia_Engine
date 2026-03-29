#version 330 core

in vec2 TexCoords;
uniform sampler2D screenTexture;

uniform float exposure;

out vec4 FragColor;

void main()
{
	vec4 hdrColor = texture(screenTexture, TexCoords);
	float gamma = 2.2;
	vec3 mapped =  vec3(1.0) - exp(-hdrColor.rgb * exposure);
	mapped = pow(mapped, vec3(1.0 / gamma));

	FragColor = vec4(mapped, hdrColor.a);
}