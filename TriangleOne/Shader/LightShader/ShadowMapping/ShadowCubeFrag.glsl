#version 330 core

uniform vec3 lightPos;
uniform float far_plane;

in vec4 FragPos;

void main()
{

	float lightDistance = length(FragPos.xyz - lightPos);

	lightDistance = lightDistance / far_plane;

	gl_FragDepth = lightDistance;
}