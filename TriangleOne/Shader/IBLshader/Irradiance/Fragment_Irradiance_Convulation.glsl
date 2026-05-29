#version 430 core
out vec4 FragColor;

uniform samplerCube environmentMap;

in vec3 localPos;

const float PI = 3.14159265359;

void main()
{
	vec3 normal = normalize(localPos);

	vec3 irradiance = vec3(0.0);
	vec3 up = abs(normal.y) < 0.999 ? vec3(0.0, 1.0, 0.0) : vec3(0.0, 0.0, 1.0);
	vec3 right = normalize(cross(up, normal));
	up  = cross(normal, right);

	float sampleDelta = 0.025;
	float nrSamples = 0.0;
	for(float phi = 0.0; phi < 2.0 * PI; phi += sampleDelta)
	{
		for(float theta = 0.0; theta < 0.5 * PI; theta += sampleDelta)
		{
			// spherical to cartesian (in tangent space)
			vec3 tangentSample = vec3(sin(theta) * cos(phi), sin(theta) * sin(phi), cos(theta));
			// tangent space to world
			vec3 sampleVec = tangentSample.x * right + tangentSample.y * up + tangentSample.z * normal;

			vec3 currentSample = textureLod(environmentMap, sampleVec, 0.0).rgb;
			currentSample = min(currentSample , vec3(10.0));
			irradiance += currentSample * cos(theta) * sin(theta);
			nrSamples++;
		}
	}
	irradiance = PI * irradiance * (1.0 / float(nrSamples));

	FragColor = vec4(irradiance, 1.0);

}