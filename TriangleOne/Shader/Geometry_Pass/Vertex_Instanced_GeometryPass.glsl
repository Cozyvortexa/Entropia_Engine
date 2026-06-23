#version 430 core
layout (location = 0) in vec3 aPos; 
layout (location = 1) in vec3 aNormal; 
layout (location = 2) in vec2 aTexCoords;
layout (location = 3) in vec3 aTangent;
//OpenGL does not support passing matrices with layout
layout (location = 4) in vec4 instanceMatrix_lineOne;
layout (location = 5) in vec4 instanceMatrix_lineTwo;
layout (location = 6) in vec4 instanceMatrix_lineThree;
layout (location = 7) in vec4 instanceMatrix_lineFour;


uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;


out vec3 FragPos;
out vec3 normal;
out vec2 TexCoords;
out mat3 TBN;

void main()
{
	FragPos = vec3(model * vec4(aPos, 1.0));

	normal = mat3(transpose(inverse(model))) * aNormal;
	normal = normalize(normal);
	TexCoords = aTexCoords;

	vec3 T = normalize(vec3(model * vec4(aTangent, 0.0)));
	vec3 N = normalize(normal);
	T = normalize(T - dot(T, N) * N);
	vec3 B = cross(N, T);

	TBN = mat3(T, B, N);

	mat4 instanceMatrix = mat4(instanceMatrix_lineOne, instanceMatrix_lineTwo, instanceMatrix_lineThree, instanceMatrix_lineFour);

	gl_Position = projection * view * model * instanceMatrix * vec4(aPos, 1.0);
}