#include <ECS/Components/LightComponent.h>

namespace Component = Engine::Component;
#pragma region Init
Component::Light::Light(glm::vec3 color, float intensity) {
	this->color = color;
	this->intensity = intensity;
}

#pragma endregion Init

#pragma region SpotLight
Component::SpotLight::SpotLight(glm::vec3 color, float intensity, glm::vec3 _direction, float _cutOff, float _outercutOff, float range, Render::Shadow_Quality shadowQuality) : Light(color, intensity) {
	cutOff = _cutOff;
	outerCutOff = _outercutOff;
	direction = _direction;
	this->range = range;

	//ShadowQuality
	shadowCaster.quality = shadowQuality;
}

#pragma endregion SpotLight

#pragma region DirLight
Component::DirLight::DirLight(glm::vec3 color, float intensity, glm::vec3 _direction, Render::Shadow_Quality shadowQuality) : Light(color, intensity) {
	direction = _direction;

	//ShadowQuality
	shadowCaster.quality = shadowQuality;

	ndcCubePoint.push_back(glm::vec3(-1, -1, -1));
	ndcCubePoint.push_back(glm::vec3(1, -1, -1));
	ndcCubePoint.push_back(glm::vec3(-1, 1, -1));
	ndcCubePoint.push_back(glm::vec3(1, 1, -1));

	ndcCubePoint.push_back(glm::vec3(-1, -1, 1));
	ndcCubePoint.push_back(glm::vec3(1, -1, 1));
	ndcCubePoint.push_back(glm::vec3(-1, 1, 1));
	ndcCubePoint.push_back(glm::vec3(1, 1, 1));
}

std::vector<glm::vec3> Component::DirLight::CalcWorldCorner(const glm::mat4 projection, glm::mat4 viewMatrice) {
	glm::mat4 invProjectionViewMatrice = glm::inverse(projection * viewMatrice);
	std::vector<glm::vec3> result;

	for (glm::vec3 ndcCorner : ndcCubePoint) {
		glm::vec4 point = invProjectionViewMatrice * glm::vec4(ndcCorner, 1);
		glm::vec3 worldCorner = glm::vec3(point) / point.w;
		result.push_back(worldCorner);
	}
	return result;
}

glm::vec3 Component::DirLight::FrustumCenter(std::vector<glm::vec3> corners)
{
	glm::vec3 center(0.0f);

	for (int i = 0; i < 8; ++i)
		center += corners[i];

	return center / 8.0f;
}

AABB Component::DirLight::CalcBoundingBox(const std::vector<glm::vec3> worldCorner) {
	glm::vec3 minPoint(std::numeric_limits<float>::infinity());
	glm::vec3 maxPoint(-std::numeric_limits<float>::infinity());


	for (glm::vec3 worldPoint : worldCorner) {

		minPoint = glm::min(minPoint, worldPoint);
		maxPoint = glm::max(maxPoint, worldPoint);
	}

	return AABB(minPoint, maxPoint);
}

std::vector<glm::vec3> Component::DirLight::WorldCornerToLightSpace(glm::mat4 lightViewMatrice, std::vector<glm::vec3> worldCorners) {
	std::vector<glm::vec3> lightCorners;
	for (glm::vec3 currentCorner : worldCorners) {
		lightCorners.push_back(lightViewMatrice * glm::vec4(currentCorner, 1.0f));
	}
	return lightCorners;
}

glm::mat4 Component::DirLight::UpdateMatrix(const glm::mat4 viewMatrice, const glm::mat4 projectionCamera) {
	if (glm::length(direction) < 0.001f) direction = glm::vec3(0, -0.95, 0); // Valeur par défaut

	std::vector<glm::vec3> worldCorners = CalcWorldCorner(projectionCamera, viewMatrice);

	lightPos =  normalize(direction) * 10.0f;
	// First pass
	lightViewMatrice = glm::lookAt(lightPos, glm::vec3(0), glm::vec3(0.0f, 1.0f, 0.0f));

	std::vector<glm::vec3> lightCorners = WorldCornerToLightSpace(lightViewMatrice, worldCorners);

	AABB box = CalcBoundingBox(lightCorners);

	// Second passage
	float distance = (box.max.z - box.min.z) / 2.0f;
	lightPos = normalize(direction) * distance;
	glm::vec3 frustumCenter = FrustumCenter(worldCorners);
	lightViewMatrice = glm::lookAt(frustumCenter, frustumCenter - lightPos, glm::vec3(0.0f, 1.0f, 0.0f));

	lightCorners = WorldCornerToLightSpace(lightViewMatrice, worldCorners);
	box = CalcBoundingBox(lightCorners);


	projection = glm::ortho(box.min.x, box.max.x, box.min.y, box.max.y, box.min.z , box.max.z);

	lightMatrice = projection * lightViewMatrice;

	return lightMatrice;
}

#pragma endregion DirLight


#pragma region PointLight
Component::PointLight::PointLight(glm::vec3 color, float intensity, float _range, Render::Shadow_Quality shadowQuality) : Light(color, intensity) {
	range = _range;
	near_plane = 0.01f;

	//ShadowQuality
	shadowCaster.quality = shadowQuality;
}

#pragma endregion PointLight