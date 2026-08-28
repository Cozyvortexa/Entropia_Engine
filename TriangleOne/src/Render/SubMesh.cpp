#include <Render/SubMesh.h>

namespace Render = Engine::Render;

static GLenum ToGLBaseType(Render::Format format)
{
	switch (format)
	{
	case Render::Format::Float1:
	case Render::Format::Float2:
	case Render::Format::Float3:
	case Render::Format::Float4:
		return GL_FLOAT;

	case Render::Format::UInt1:
	case Render::Format::UInt2:
	case Render::Format::UInt3:
	case Render::Format::UInt4:
		return GL_UNSIGNED_INT;
	}

	return GL_FLOAT;
}
static uint32_t ToGLCount(Render::Format format)
{
	switch (format)
	{
	case Render::Format::Float1: return 1;
	case Render::Format::Float2: return 2;
	case Render::Format::Float3: return 3;
	case Render::Format::Float4: return 4;

	case Render::Format::UInt1: return 1;
	case Render::Format::UInt2: return 2;
	case Render::Format::UInt3: return 3;
	case Render::Format::UInt4: return 4;

	}

	return 0;
}

SubMesh::SubMesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::shared_ptr<Material> materialPtr)
{
	this->vertices = vertices;
	this->indices = indices;
	this->materialPtr = materialPtr;

	//Position
	Render::VertexAttribute posAttribute;
	posAttribute.format = Render::Format::Float3;
	posAttribute.location = 0;
	posAttribute.offset = 0;
	layout.attributes.push_back(posAttribute);

	//Normal
	Render::VertexAttribute normalAttribute;
	normalAttribute.format = Render::Format::Float3;
	normalAttribute.location = 1;
	normalAttribute.offset = offsetof(Vertex, Normal);
	layout.attributes.push_back(normalAttribute);

	//Texture/UV 
	Render::VertexAttribute texCoordsAttribute;
	texCoordsAttribute.format = Render::Format::Float2;
	texCoordsAttribute.location = 2;
	texCoordsAttribute.offset = offsetof(Vertex, TexCoords);
	layout.attributes.push_back(texCoordsAttribute);

	//Tangent
	Render::VertexAttribute tangentAttribute;
	tangentAttribute.format = Render::Format::Float3;
	tangentAttribute.location = 3;
	tangentAttribute.offset = offsetof(Vertex, Tangent);
	layout.attributes.push_back(tangentAttribute);

	layout.stride = sizeof(Vertex);
}

///////////////OpenGL

OpenGL_SubMesh::OpenGL_SubMesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::shared_ptr<Material> materialPtr) : SubMesh(vertices, indices, materialPtr) {
	//glGenVertexArrays(1, &VAO);
	//glGenBuffers(1, &VBO);
	//glGenBuffers(1, &EBO);

	//glBindVertexArray(VAO);
	//glBindBuffer(GL_ARRAY_BUFFER, VBO);

	//glBufferData(GL_ARRAY_BUFFER, vertices.size() * layout.stride, &vertices[0], GL_STATIC_DRAW);
	//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	//glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);


	//for (auto& currentAttribute : layout.attributes) {
	//	glEnableVertexAttribArray(currentAttribute.location);
	//	glVertexAttribPointer(currentAttribute.location, ToGLCount(currentAttribute.format), ToGLBaseType(currentAttribute.format), GL_FALSE, layout.stride, (void*)currentAttribute.offset);
	//}

	//glBindVertexArray(0);

}