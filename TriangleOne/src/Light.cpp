#include <Light.h>


Light::Light(glm::vec3 position, glm::vec3 direction, glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular) {
	lightType = Light::DirectionalLight;

	_position = position;
	_direction = direction;
	_ambient = ambient;
	_diffuse = diffuse;
	_specular = specular;
}
Light::Light(glm::vec3 position, glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular, float constant, float linear, float quadratique) {
	lightType = Light::PointLight;

	_position = position;
	_ambient = ambient;
	_diffuse = diffuse;
	_specular = specular;

	_constant = constant;
	_linear = linear;
	_quadratique = quadratique;




}

void Light::UseLight(Shader* shader) {
	switch (lightType)
	{
	case Light::DirectionalLight: {
		shader->setVec3("dirLight.direction", glm::normalize(_direction));
		shader->setVec3("dirLight.ambient", _ambient);
		shader->setVec3("dirLight.diffuse", _diffuse);
		shader->setVec3("dirLight.specular", _specular);
		break;
	}
	default:
		break;
	}
}

void Light::UseShadow(Shader* shader) {
	switch (lightType)
	{
	case Light::DirectionalLight: {
		float near_plane = 1.0f, far_plane = 50.0f;
		float orthoSize = 50.0f;
		float distance = far_plane / 2;
		glm::mat4 lightProjection = glm::ortho(-orthoSize, orthoSize, -orthoSize, orthoSize, near_plane, far_plane);

		glm::vec3 lightPos = normalize(_direction) * distance;

		glm::mat4 lightView = glm::lookAt(lightPos, glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));

		glm::mat4 lightMatrice = lightProjection * lightView;
		shader->setMatrix("lightSpaceMatrix", lightMatrice);
		break;
	}
	default:
		break;
	}
}