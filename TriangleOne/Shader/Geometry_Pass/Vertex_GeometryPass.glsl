#version 430 core
layout (location = 0) in vec3 aPos; 
layout (location = 1) in vec3 aNormal; 
layout (location = 2) in vec2 aTexCoords;
layout (location = 3) in vec3 aTangent;


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
	TexCoords = aTexCoords;

	vec3 T = normalize(vec3(model * vec4(aTangent, 0.0)));
	vec3 N = normalize(normal);
	T = normalize(T - dot(T, N) * N);
	vec3 B = cross(N, T);

	TBN = mat3(T, B, N);

	gl_Position = projection * view * model * vec4(aPos, 1.0);
}