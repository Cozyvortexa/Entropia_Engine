#include <Entity/Components/Light.h>


Light::Light(glm::vec3 _position, glm::vec3 _ambient, glm::vec3 _diffuse, glm::vec3 _specular) {
	position = _position;
	ambient = _ambient;
	diffuse = _diffuse;
	specular = _specular;
}

//void Light::UseLight(Shader* shader) {
//	switch (lightType)
//	{
//	case Light::DirectionalLight: {
//		shader->setVec3("dirLight.direction", glm::normalize(_direction));
//		shader->setVec3("dirLight.ambient", _ambient);
//		shader->setVec3("dirLight.diffuse", _diffuse);
//		shader->setVec3("dirLight.specular", _specular);
//		break;
//	}
//	default:
//		break;
//	}
//}
//
//void Light::UseShadow(Shader* shader) {
//	switch (lightType)
//	{
//	case Light::DirectionalLight: {
//		float near_plane = 1.0f, far_plane = 50.0f;
//		float orthoSize = 50.0f;
//		float distance = far_plane / 2;
//		glm::mat4 lightProjection = glm::ortho(-orthoSize, orthoSize, -orthoSize, orthoSize, near_plane, far_plane);
//
//		glm::vec3 lightPos = normalize(_direction) * distance;
//
//		glm::mat4 lightView = glm::lookAt(lightPos, glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
//
//		glm::mat4 lightMatrice = lightProjection * lightView;
//		shader->setMatrix("lightSpaceMatrix", lightMatrice);
//		break;
//	}
//	default:
//		break;
//	}
//}

PointLight::PointLight(glm::vec3 _position, glm::vec3 _ambient, glm::vec3 _diffuse, glm::vec3 _specular, float _constant, float _linear, float _quadratique) {
	Light(_position, _ambient, _diffuse, _specular);
	constant = _constant;
	linear = _linear;
	quadratique = _quadratique;
}

DirLight::DirLight(glm::vec3 _position, glm::vec3 _ambient, glm::vec3 _diffuse, glm::vec3 _specular, glm::vec3 _direction) {
	Light(_position, _ambient, _diffuse, _specular);
	direction = _direction;
}