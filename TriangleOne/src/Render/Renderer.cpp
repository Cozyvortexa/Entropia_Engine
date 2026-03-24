#include "Render/Renderer.h"

void Renderer::DrawMesh(Mesh& currentMesh) {
	for (SubMesh& subMesh : currentMesh.subMeshs) {
		assert(glIsVertexArray(subMesh.VAO));
		assert(glIsBuffer(subMesh.EBO));

		Material& currentMat = assetStore->Get_Material(subMesh.material_Handle);
		Shader* shader = &currentMat.shader;
		bool haveSpecular = true;

		unsigned int diffuse_Text = 0;
		unsigned int specular_Text = 0;
		unsigned int normal_Text = 0;

		// Get texture
		// 
		//Diffuse
		Texture* diffuse = assetStore->Get_Texture(currentMat.diffuse_Text_Handle);
		if (diffuse == nullptr) {
			diffuse_Text = Shader::GetDefaultText();
		}
		else { diffuse_Text = diffuse->id; }

		//Specular
		Texture* specular = assetStore->Get_Texture(currentMat.specular_Text_Handle);
		if (specular == nullptr) {
			haveSpecular = false;
			specular_Text = Shader::GetDefaultText(); // Just to put something in the slot
		}
		else { specular_Text = specular->id; }

		//Normal
		Texture* normal = assetStore->Get_Texture(currentMat.normal_Text_Handle);
		if (normal == nullptr) {
			normal_Text = Shader::GetNeutralNormalText();
		}
		else { normal_Text = normal->id; }

		shader->Use();
		// Bind
		int i = 0;
		glActiveTexture(GL_TEXTURE0 + i);
		glBindTexture(GL_TEXTURE_2D, diffuse_Text);
		shader->setInt("material.diffuseText", i);
		i++;

		glActiveTexture(GL_TEXTURE0 + i);
		glBindTexture(GL_TEXTURE_2D, specular_Text);
		shader->setInt("material.specularText", i);
		shader->setBool("have_Specular", haveSpecular);
		i++;

		glActiveTexture(GL_TEXTURE0 + i);
		glBindTexture(GL_TEXTURE_2D, normal_Text);
		shader->setInt("material.normalText", i);
		i++;

		glActiveTexture(GL_TEXTURE0);


		// draw SubMesh
		glBindVertexArray(subMesh.VAO);
		glDrawElements(GL_TRIANGLES, subMesh.indices.size(), GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);
	}
}

void Renderer::DrawMesh_Without_Texture(Mesh& currentMesh) {
	for (SubMesh& subMesh : currentMesh.subMeshs) {
		glBindVertexArray(subMesh.VAO);
		glDrawElements(GL_TRIANGLES, subMesh.indices.size(), GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);
	}
}