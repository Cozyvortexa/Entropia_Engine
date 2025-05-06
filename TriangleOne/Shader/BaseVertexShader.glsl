#version 330 core
layout (location = 0) in vec3 aPos; // position has attribute position 0
//layout (location = 1) in vec3 aColor; // color has attribute position 1
layout (location = 1) in vec2 aTexCoord;  // position 2

//out vec3 ourColor; // output a color to the fragment shader
out vec2 TexCoord;

//uniform mat4 transform;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
	gl_Position = projection * view * model * vec4(aPos, 1.0);
	//ourColor = aColor; // set ourColor to input color from the vertex data
	TexCoord = aTexCoord;
}