#include <Entity/Components/Light.h>

#pragma region Init
void Light::InitBaseLight(glm::vec3 _ambient, glm::vec3 _diffuse, glm::vec3 _specular, float newIntensity) {
	ambient = _ambient;
	diffuse = _diffuse;
	specular = _specular;

	intensity = newIntensity;

	ambient = glm::clamp(ambient, 0.0f, 1.0f);
	diffuse = glm::clamp(diffuse, 0.0f, 1.0f);
	diffuse *= intensity;
}

Light::Light(glm::vec3 _ambient, glm::vec3 _diffuse, glm::vec3 _specular, float newIntensity) {
	InitBaseLight(_ambient, _diffuse, _specular, newIntensity);
}

std::pair<unsigned int, unsigned int> Light::InitShadowMap() {
	unsigned int depthMapFBO;
	unsigned int depthMap;
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


	return std::make_pair(depthMapFBO, depthMap);
}

std::pair<unsigned int, unsigned int> Light::InitCubeMap() {
	unsigned int depthCubeMapFBO;
	unsigned int depthCubemap;
	glGenFramebuffers(1, &depthCubeMapFBO);

	glGenTextures(1, &depthCubemap);
	glBindTexture(GL_TEXTURE_CUBE_MAP, depthCubemap);
	for (int i = 0; i < 6; ++i)
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_DEPTH_COMPONENT32F, shadowWidth, shadowHeight, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);


	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);

	glBindFramebuffer(GL_FRAMEBUFFER, depthCubeMapFBO);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depthCubemap, 0);

	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "CubeMap Shadow Framebuffer not complete!" << std::endl;
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	//glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

	return std::make_pair(depthCubeMapFBO, depthCubemap);
}

std::pair<unsigned int, unsigned int> Light::InitSpotShadowMap() {
	unsigned int depthMapFBO;
	unsigned int depthMap;
	glGenFramebuffers(1, &depthMapFBO);

	glGenTextures(1, &depthMap);
	glBindTexture(GL_TEXTURE_2D, depthMap);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32F, shadowWidth, shadowHeight, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);


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


	return std::make_pair(depthMapFBO, depthMap);
}

#pragma endregion Init

#pragma region SpotLight
SpotLight::SpotLight(glm::vec3 _ambient, glm::vec3 _diffuse, glm::vec3 _specular, glm::vec3 _direction, float _cutOff, float _outercutOff, float range, std::shared_ptr<Shader> depthShaderSpotMap, float newIntensity) {
	std::pair<unsigned int, unsigned int> depthBuffer = InitSpotShadowMap();
	depthMapFBO = depthBuffer.first;
	depthMap = depthBuffer.second;

	InitBaseLight(_ambient, _diffuse, _specular, newIntensity);

	aspect = (float)shadowWidth / (float)shadowHeight;

	intensity = newIntensity;
	cutOff = _cutOff;
	outerCutOff = _outercutOff;
	direction = _direction;
	this->range = range;
	this->depthSpotShaderMap = depthShaderSpotMap;
}
#pragma endregion SpotLight

#pragma region DirLight

DirLight::DirLight(glm::vec3 _ambient, glm::vec3 _diffuse, glm::vec3 _specular, glm::vec3 _direction, std::shared_ptr<Shader> _depthShader, float newIntensity) {
	std::pair<unsigned int, unsigned int> depthBuffer = InitShadowMap();
	depthMapFBO = depthBuffer.first;
	depthMap = depthBuffer.second;  // On sait jamais

	InitBaseLight(_ambient, _diffuse, _specular, newIntensity);
	direction = _direction;

	depthShader = _depthShader;



	ndcCubePoint.push_back(glm::vec3(-1, -1, -1));
	ndcCubePoint.push_back(glm::vec3(1, -1, -1));
	ndcCubePoint.push_back(glm::vec3(-1, 1, -1));
	ndcCubePoint.push_back(glm::vec3(1, 1, -1));

	ndcCubePoint.push_back(glm::vec3(-1, -1, 1));
	ndcCubePoint.push_back(glm::vec3(1, -1, 1));
	ndcCubePoint.push_back(glm::vec3(-1, 1, 1));
	ndcCubePoint.push_back(glm::vec3(1, 1, 1));
}
std::vector<glm::vec3> DirLight::CalcWorldCorner(const glm::mat4 projection, glm::mat4 viewMatrice) {
	glm::mat4 invProjectionViewMatrice = glm::inverse(projection * viewMatrice);
	std::vector<glm::vec3> result;

	for (glm::vec3 ndcCorner : ndcCubePoint) {
		glm::vec4 point = invProjectionViewMatrice * glm::vec4(ndcCorner, 1);
		glm::vec3 worldCorner = glm::vec3(point) / point.w;
		result.push_back(worldCorner);
	}
	return result;
}

glm::vec3 DirLight::FrustumCenter(std::vector<glm::vec3> corners)
{
	glm::vec3 center(0.0f);

	for (int i = 0; i < 8; ++i)
		center += corners[i];

	return center / 8.0f;
}


AABB DirLight::CalcBoundingBox(const std::vector<glm::vec3> worldCorner) {
	glm::vec3 minPoint(std::numeric_limits<float>::infinity());
	glm::vec3 maxPoint(-std::numeric_limits<float>::infinity());


	for (glm::vec3 worldPoint : worldCorner) {

		minPoint = glm::min(minPoint, worldPoint);
		maxPoint = glm::max(maxPoint, worldPoint);
	}

	return AABB(minPoint, maxPoint);
}

std::vector<glm::vec3> DirLight::WorldCornerToLightSpace(glm::mat4 lightViewMatrice, std::vector<glm::vec3> worldCorners) {
	std::vector<glm::vec3> lightCorners;
	for (glm::vec3 currentCorner : worldCorners) {
		lightCorners.push_back(lightViewMatrice * glm::vec4(currentCorner, 1.0f));
	}
	return lightCorners;
}

void DirLight::UpdateMatrix(glm::mat4 projection, const glm::mat4 viewMatrice) {
	if (glm::length(direction) < 0.001f) direction = glm::vec3(0, -1, 0); // Valeur par défaut

	std::vector<glm::vec3> worldCorners = CalcWorldCorner(projection, viewMatrice);

	lightPos =  normalize(direction) * 10.0f;
	lightViewMatrice = glm::lookAt(lightPos, glm::vec3(0), glm::vec3(0.0f, 1.0f, 0.0f));

	std::vector<glm::vec3> lightCorners = WorldCornerToLightSpace(lightViewMatrice, worldCorners);

	AABB box = CalcBoundingBox(lightCorners);

	// Deuxieme passage
	float distance = (box.max.z - box.min.z) / 2.0f;
	lightPos = normalize(direction) * distance;
	glm::vec3 frustumCenter = FrustumCenter(worldCorners);
	lightViewMatrice = glm::lookAt(frustumCenter, frustumCenter - lightPos, glm::vec3(0.0f, 1.0f, 0.0f));

	lightCorners = WorldCornerToLightSpace(lightViewMatrice, worldCorners);
	box = CalcBoundingBox(lightCorners);


	lightProjection = glm::ortho(box.min.x, box.max.x, box.min.y, box.max.y, box.min.z , box.max.z);

	lightMatrice = lightProjection * lightViewMatrice;


}

#pragma endregion DirLight

#pragma region PointLight

PointLight::PointLight(glm::vec3 _ambient, glm::vec3 _diffuse, glm::vec3 _specular, float _range, std::shared_ptr<Shader> _depthShaderCubeMap, float newIntensity) {
	std::pair<unsigned int, unsigned int> depthBuffer = InitCubeMap();
	depthCubeMapFBO = depthBuffer.first;
	depthCubeMap = depthBuffer.second;

	InitBaseLight(_ambient, _diffuse, _specular, newIntensity);


	range = _range;
	near_plane = 0.01f;

	aspect = (float)shadowWidth / (float)shadowHeight;


	depthShaderCubeMap = _depthShaderCubeMap;
}

#pragma endregion PointLight