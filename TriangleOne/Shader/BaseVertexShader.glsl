#version 330 core
layout (location = 0) in vec3 aPos; 
layout (location = 1) in vec3 aNormal; 
layout (location = 2) in vec2 aTexCoords;



uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

uniform mat3 normalViewMatrix;

out vec3 FragPosView;
out vec3 normal;
out vec2 TexCoords;

void main()
{
	gl_Position = projection * view * model * vec4(aPos, 1.0);
	FragPosView = vec3(view * model * vec4(aPos, 1.0));
	normal = normalViewMatrix * aNormal;

	//ourColor = aColor; // set ourColor to input color from the vertex data
	TexCoords = aTexCoords;
}