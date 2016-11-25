#include "scene.h"

#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/gtc/matrix_transform.hpp>

#include "vertex.h"
#include "global_descriptor_sets.h"
#include "global_descriptor_sets.h"

void Scene::PrepareScene()
{
	// VERTEX & INDEX BUFFER SETUP
	Vertex cubeVertices[] = {
		{ { -1.0f, -1.0f, 1.0f },{},{},{} },
		{ { 1.0f, -1.0f, 1.0f },{},{},{} },
		{ { 1.0f, 1.0f, 1.0f },{},{},{} },
		{ { -1.0f, 1.0f, 1.0f },{},{},{} },
		{ { -1.0f, -1.0f, -1.0f },{},{},{} },
		{ { 1.0f, -1.0f, -1.0f },{},{},{} },
		{ { 1.0f, 1.0f, -1.0f },{},{},{} },
		{ { -1.0f, 1.0f, -1.0f },{},{},{} }
	};

	for (Vertex& v : cubeVertices) {
		v.normal = glm::normalize(v.position);
	}

	for (Vertex& v : cubeVertices)
		for (int i = 0; i < 3; ++i)
			v.position[i] *= 0.5f;

	const size_t cubeVertexCount = std::size(cubeVertices);

	u32 cubeIndices[] = {
		// front
		0, 1, 2,
		2, 3, 0,
		// top
		1, 5, 6,
		6, 2, 1,
		// back
		7, 6, 5,
		5, 4, 7,
		// bottom
		4, 0, 3,
		3, 7, 4,
		// left
		4, 5, 1,
		1, 0, 4,
		// right
		3, 2, 6,
		6, 7, 3,
	};
	const VkPrimitiveTopology cubePrimitiveTopology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
	const u32 cubeFaceCount = 12u;
	const u32 cubeIndexCount = cubeFaceCount * 3u;

	modelPrimitiveTopology = cubePrimitiveTopology;

	modelVertexCount = cubeVertexCount;
	Vertex* modelVertices = &(cubeVertices[0]);

	modelIndexCount = cubeIndexCount;
	u32* modelIndices = cubeIndices;

	sizeofVertex = sizeof(Vertex);

	const VkDeviceSize vertexBufferSize = modelVertexCount * sizeofVertex;
	const VkDeviceSize indexBufferSize = modelIndexCount * sizeof(u32);

	vertexBufferHandle = gDeviceBufferAllocator.GetNewBufferHandle(EDeviceBufferType::kVertex, vertexBufferSize, 0u);
	indexBufferHandle = gDeviceBufferAllocator.GetNewBufferHandle(EDeviceBufferType::kIndex, indexBufferSize, 0u);

	// BUG BUG BUG ? Don't know why, maybe I did something wrong, but the last vertex is zeroed out if not adding at least 4 to the buffer creation size
	// Might be 'fixed'

	gDeviceBufferAllocator.UploadData(vertexBufferHandle, VkDeviceSize{ 0 }, vertexBufferSize, modelVertices);
	gDeviceBufferAllocator.UploadData(indexBufferHandle, VkDeviceSize{ 0 }, indexBufferSize, modelIndices);

	modelInstanceCount = 4;

	//cubenorm.loadFromFile(MODELS_FOLDER_PATH "cubenorm.obj");
	cubenorm.loadFromFile(MODELS_FOLDER_PATH "Internet/cube/cube.obj");
	//cubenorm.loadFromFile(MODELS_FOLDER_PATH "Internet/teapot/teapot.obj");
	//cubenorm.loadFromFile(MODELS_FOLDER_PATH "Internet/buddha/buddha.obj");
}

void Scene::UpdateScene(float time)
{
	const float timeFactor = 0.1f;

	modelWorld = glm::translate(glm::mat4(1.f), glm::vec3(0.f, glm::sin(time*timeFactor), 0.f));
	modelWorld2 = glm::translate(glm::mat4(1.f), glm::vec3(1.3f*glm::sin(time*timeFactor), 0.f, 1.3f*glm::cos(time*timeFactor)));

	modelWorld4 = glm::scale(glm::mat4(1.f), glm::vec3(30.f, 30.f, 1.f));
	modelWorld4 = glm::translate(modelWorld4, glm::vec3(0.f, 0.f, 10.f));

	PerObjectCB& perObjectCB = GlobalDescriptorSets::GetMutableCBuffer<PerObjectCB>();
	perObjectCB.g_world[0] = modelWorld;
	perObjectCB.g_world[1] = modelWorld2;
	perObjectCB.g_world[3] = modelWorld4;
	GlobalDescriptorSets::CommitCBuffer<PerObjectCB>();
}

void Scene::BindDrawingToCmdBuffer(VkCommandBuffer cmdBuffer) const
{
	cubenorm.m_meshes[0].BindDrawingToCmdBuffer(cmdBuffer);
	return;

	////////////////////////////////

	vkCmdBindIndexBuffer(cmdBuffer, indexBufferHandle.buffer, 0u, VK_INDEX_TYPE_UINT32);

	VkDeviceSize vertexBufferOffsetArray[] = { 0 };
	vkCmdBindVertexBuffers(cmdBuffer, 0u, 1u, &vertexBufferHandle.buffer, vertexBufferOffsetArray);

	vkCmdDrawIndexed(cmdBuffer, modelIndexCount, modelInstanceCount, 0u, 0u, 0u);

	// OR, ONLY IF REBUILDING THE BUFFER EACH FRAME, MIGHT UPLOAD VIA PUSH CONSTANT
	//for (u32 modelIdx = 0; modelIdx < scene.modelInstanceCount; ++modelIdx) {
	//	vkCmdPushConstants(mCmdBuffer, mPipelineLayout, VK_SHADER_STAGE_VERTEX_BIT, 0u, 64u,
	//		glm::value_ptr(GlobalDescriptorSets::GetMutableCBuffer<PerObjectCB>().g_world[modelIdx]));
	//	vkCmdDrawIndexed(mCmdBuffer, scene.modelIndexCount, 1, 0u, 0u, 0u);
	//}
}

void Scene::Destroy()
{
	//vertexBuffer.Destroy();
	//indexBuffer.Destroy();
}
