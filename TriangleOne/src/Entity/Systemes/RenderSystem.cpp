#include "Entity/Systemes/RenderSystem.h"

void RenderSystem::UpdateLight(std::shared_ptr<Shader> shader) {
	for (std::shared_ptr<DirLight> dirLight: directionalLightList) {
		shader->setVec3("dirLight.direction", glm::normalize(dirLight->direction));
		shader->setVec3("dirLight.ambient", dirLight->ambient);
		shader->setVec3("dirLight.diffuse", dirLight->diffuse);
		shader->setVec3("dirLight.specular", dirLight->specular);
	}

	for (int i = 0; i < pointLightList.size();i++) {
		shader->setVec3("pointLights[" + std::to_string(i) + "].position", pointLightList[i]->position);

		shader->setVec3("pointLights[" + std::to_string(i) + "].ambient", pointLightList[i]->ambient);
		shader->setVec3("pointLights[" + std::to_string(i) + "].diffuse", pointLightList[i]->diffuse);
		shader->setVec3("pointLights[" + std::to_string(i) + "].specular", pointLightList[i]->specular);

		shader->setFloat("pointLights[" + std::to_string(i) + "].constant", pointLightList[i]->constant);
		shader->setFloat("pointLights[" + std::to_string(i) + "].linear", pointLightList[i]->linear);
		shader->setFloat("pointLights[" + std::to_string(i) + "].quadratic", pointLightList[i]->quadratique);

	}

	for (int i = 0; i < spotLightList.size();i++)
	{
		shader->setVec3("spotLight.Position", spotLightList[i]->position);

		shader->setVec3("spotLight.ambient", spotLightList[i]->ambient);
		shader->setVec3("spotLight.diffuse", spotLightList[i]->diffuse);
		shader->setVec3("spotLight.specular", spotLightList[i]->specular);

		shader->setFloat("spotLight.constant", spotLightList[i]->constant);
		shader->setFloat("spotLight.linear", spotLightList[i]->linear);
		shader->setFloat("spotLight.quadratic", spotLightList[i]->quadratique);

		shader->setVec3("spotLight.direction", spotLightList[i]->direction);
		shader->setFloat("spotLight.cutOff", glm::radians(spotLightList[i]->cutOff));
		shader->setFloat("spotLight.outerCutOff", glm::radians(spotLightList[i]->outerCutOff));

	}

}

void RenderSystem::AddMeshComponent(std::shared_ptr<MeshComponent> modele) {
	modeleList.push_back(modele);
}

//void RenderSystem::RemoveMeshComponent(std::shared_ptr<MeshComponent> modele) {
//	for (std::shared_ptr<MeshComponent> currentModel : modeleList) {
//
//	}
//}

void RenderSystem::RenderMesh() {
	//glCullFace(GL_FRONT);
	//DrawShadowMap();
	//DrawShadowPoint();
	//glCullFace(GL_BACK);

	for (std::shared_ptr<MeshComponent> currentModel : modeleList) {
		std::shared_ptr<Shader> shader = currentModel->GetShader();

		if (currentModel->haveToBeDraw) {
			UpdateLight(shader);  // Oui je sais, les light sont recalculer pour chaque modele
			currentModel->modelMesh->Draw(shader.get());  // get a retirer
		}

	}

}