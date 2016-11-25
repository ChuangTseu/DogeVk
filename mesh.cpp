#include "mesh.h"

#include <assert.h>

#include <assimp/Importer.hpp> // C++ importer interface
#include <assimp/scene.h> // Output data structure
#include <assimp/postprocess.h> // Post processing flags
#include <assimp/config.h>

#include <glm/vec3.hpp>

constexpr glm::vec3 aiVector3D_to_glmVec3(const aiVector3D& vec3) {
	return glm::vec3{ vec3.x, vec3.y, vec3.z };
}

constexpr glm::vec2 aiVector2D_to_glmVec2(const aiVector2D& vec2) {
	return glm::vec2{ vec2.x, vec2.y };
}

bool Mesh::loadFromAssimpMesh(const aiMesh *mesh) {
    m_vertices.clear();
    m_indices.clear();

    m_materialIndex = mesh->mMaterialIndex;

    assert( mesh->mPrimitiveTypes == aiPrimitiveType_TRIANGLE);

    for (unsigned int idFace = 0; idFace < mesh->mNumFaces; ++idFace) {
        const aiFace& triangle = mesh->mFaces[idFace];

        for (unsigned int idIndex = 0; idIndex < 3; ++idIndex) {
            m_indices.push_back(triangle.mIndices[idIndex]);
        }
    }

    assert( mesh->HasNormals() );
    assert( mesh->HasTangentsAndBitangents() );

	m_vertices.resize(mesh->mNumVertices);

    for (unsigned int idVertex = 0; idVertex < mesh->mNumVertices; ++idVertex) {
        const aiVector3D& position = mesh->mVertices[idVertex];
        const aiVector3D& normal = mesh->mNormals[idVertex];

        Vertex& v = m_vertices[idVertex];
		v.position = aiVector3D_to_glmVec3(position);
		v.normal = aiVector3D_to_glmVec3(normal);

        if (mesh->HasTangentsAndBitangents()) {
            const aiVector3D& texcoord = (mesh->HasTextureCoords(0) ?
                                              mesh->mTextureCoords[0][idVertex] : aiVector3D{0.f, 0.f, 0.f});

            const aiVector3D& tangent = mesh->mTangents[idVertex];

			v.texcoord = aiVector3D_to_glmVec3(texcoord);
			v.tangent = aiVector3D_to_glmVec3(tangent);
        }
	}

	vertexBufferHandle = gDeviceBufferAllocator.GetNewBufferHandle(EDeviceBufferType::kVertex, m_vertices.size() * sizeof(Vertex), 0u);
	indexBufferHandle = gDeviceBufferAllocator.GetNewBufferHandle(EDeviceBufferType::kIndex, m_indices.size() * 4, 0u);

	// BUG BUG BUG ? Don't know why, maybe I did something wrong, but the last vertex is zeroed out if not adding at least 4 to the buffer creation size
	// Might be 'fixed'

	gDeviceBufferAllocator.UploadData(vertexBufferHandle, VkDeviceSize{ 0 }, m_vertices.size() * sizeof(Vertex), m_vertices.data());
	gDeviceBufferAllocator.UploadData(indexBufferHandle, VkDeviceSize{ 0 }, m_indices.size() * 4, m_indices.data());

	// Cook buffer for static ?

    return true;
}

void Mesh::BindDrawingToCmdBuffer(VkCommandBuffer cmdBuffer) const
{
	vkCmdBindIndexBuffer(cmdBuffer, indexBufferHandle.buffer, 0u, VK_INDEX_TYPE_UINT32);

	VkDeviceSize vertexBufferOffsetArray[] = { 0 };
	vkCmdBindVertexBuffers(cmdBuffer, 0u, 1u, &vertexBufferHandle.buffer, vertexBufferOffsetArray);

	vkCmdDrawIndexed(cmdBuffer, (u32)m_indices.size(), 1u, 0u, 0u, 0u);

	// OR, ONLY IF REBUILDING THE BUFFER EACH FRAME, MIGHT UPLOAD VIA PUSH CONSTANT
	//for (u32 modelIdx = 0; modelIdx < scene.modelInstanceCount; ++modelIdx) {
	//	vkCmdPushConstants(mCmdBuffer, mPipelineLayout, VK_SHADER_STAGE_VERTEX_BIT, 0u, 64u,
	//		glm::value_ptr(GlobalDescriptorSets::GetMutableCBuffer<PerObjectCB>().g_world[modelIdx]));
	//	vkCmdDrawIndexed(mCmdBuffer, scene.modelIndexCount, 1, 0u, 0u, 0u);
	//}
}

void Mesh::draw() const
{
    drawAsTriangles();
}

void Mesh::drawAsTriangles() const
{
    //m_vao.bind();

    //GL(glDrawElements(GL_TRIANGLES, m_indices.size(), GL_UNSIGNED_INT, nullptr));

    //VAO::unbind();
}

void Mesh::drawAsPatch() {
    //m_vao.bind();

    //GL(glPatchParameteri(GL_PATCH_VERTICES, 3));

    //GL(glDrawElements(GL_PATCHES, m_indices.size(), GL_UNSIGNED_INT, nullptr));

    //VAO::unbind();
}

bool Mesh::loadFullscreenQuad() {
    m_vertices.clear();
    m_indices.clear();

    std::vector<glm::vec3> quadVertices;
    std::vector<u32> quadIndices;

    quadVertices = std::vector<glm::vec3>{ glm::vec3{-1.f, -1.f, 0.f}, glm::vec3{1.f, -1.f, 0.f}, glm::vec3{1.f, 1.f, 0.f}, glm::vec3{-1.f, 1.f, 0.f}};
    quadIndices = std::vector<u32>{2, 1, 0, 3, 2, 0};

    //m_vbo.submitData(quadVertices.data(), quadVertices.size());
    //m_ibo.submitData(quadIndices.data(), quadIndices.size());

    m_indices = std::move(quadIndices);

    //m_vao.bind();

    //m_vbo.bind();
    //GL(glVertexAttribPointer(0, 3, GL_FLOAT, false, 0, 0));
    //GL(glEnableVertexAttribArray(0));

    //m_ibo.bind();

    //VAO::unbind();

    //VBO::unbind();
    //IBO::unbind();

    return true;
}
