#include <Entity/Components/Light.h>


void Light::InitBaseLight(glm::vec3 _position, glm::vec3 _ambient, glm::vec3 _diffuse, glm::vec3 _specular) {
	position = _position;
	ambient = _ambient;
	diffuse = _diffuse;
	specular = _specular;
}

Light::Light(glm::vec3 _position, glm::vec3 _ambient, glm::vec3 _diffuse, glm::vec3 _specular) {
	InitBaseLight(_position, _ambient, _diffuse, _specular);
}

void Light::InitShadowMap(unsigned int depthMapFBO, unsigned int depthMap) {
	glGenFramebuffers(1, &depthMapFBO);

	glGenTextures(1, &depthMap);
	glBindTexture(GL_TEXTURE_2D, depthMap);
	//glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, shadowWidth, shadowHeight, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);


	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

	float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

	glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);

	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "Shadow Framebuffer not complete!" << std::endl;
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Light::InitCubeMap(unsigned int depthCubeMapFBO,unsigned int depthCubemap) {
	glGenFramebuffers(1, &depthCubeMapFBO);

	glGenTextures(1, &depthCubemap);
	glBindTexture(GL_TEXTURE_CUBE_MAP, depthCubemap);
	for (unsigned int i = 0; i < 6; ++i)
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_DEPTH_COMPONENT, shadowWidth, shadowHeight, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);


	glBindFramebuffer(GL_FRAMEBUFFER, depthCubeMapFBO);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depthCubemap, 0);

	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "CubeMap Shadow Framebuffer not complete!" << std::endl;
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	//glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
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

PointLight::PointLight(glm::vec3 _position, glm::vec3 _ambient, glm::vec3 _diffuse, glm::vec3 _specular, float _constant, float _linear, float _quadratique, std::shared_ptr<Shader> _depthShaderCubeMap) {
	InitCubeMap(depthCubeMapFBO, depthCubeMap);
	InitBaseLight(_position, _ambient, _diffuse, _specular);

	constant = _constant;
	linear = _linear;
	quadratique = _quadratique;

	aspect = (float)shadowWidth / (float)shadowHeight;
	//Valeur par default
	near_plane = 0.1f;
	far_plane = 50.0f;

	depthShaderCubeMap = _depthShaderCubeMap;
}

SpotLight::SpotLight(glm::vec3 _position, glm::vec3 _ambient, glm::vec3 _diffuse, glm::vec3 _specular, glm::vec3 _direction, float _constant, float _linear, float _quadratique, float _cutOff, float _outercutOff) {
	InitBaseLight(_position, _ambient, _diffuse, _specular);
	constant = _constant;
	linear = _linear;
	quadratique = _quadratique;

	cutOff = _cutOff;
	outerCutOff = _outercutOff;
	direction = _direction;
}

DirLight::DirLight(glm::vec3 _position, glm::vec3 _ambient, glm::vec3 _diffuse, glm::vec3 _specular, glm::vec3 _direction, std::shared_ptr<Shader> _depthShader) {
	InitShadowMap(depthMapFBO, depthMap);
	InitBaseLight(_position, _ambient, _diffuse, _specular);

	direction = _direction;
	//Shadow Purpose
	lightProjection = glm::ortho(-orthoSize, orthoSize, -orthoSize, orthoSize, near_plane, far_plane);

	lightPos = normalize(_direction) * distance;

	lightView = glm::lookAt(lightPos, glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));

	lightMatrice = lightProjection * lightView;

	depthShader = _depthShader;
}