#version 330 core

in vec2 TexCoords;
uniform sampler2D scene;
uniform sampler2D bloomBlur;

uniform bool bloomEnable;

uniform float exposure;

out vec4 FragColor;

void main()
{
	vec4 hdrColor = texture(scene, TexCoords);
	float gamma = 2.2;
	if (bloomEnable){
		vec3 bloomColor = texture(bloomBlur, TexCoords).rgb;
		hdrColor.rgb += bloomColor; 
	}

	vec3 mapped =  vec3(1.0) - exp(-hdrColor.rgb * exposure);
	mapped = pow(mapped, vec3(1.0 / gamma));

	FragColor = vec4(mapped, hdrColor.a);
}