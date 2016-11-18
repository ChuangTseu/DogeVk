#pragma once

#include <vector>

#include <glm/mat4x4.hpp>

#include "doge_vulkan.h"
#include "dedicated_buffer.h"
#include "light.h"

struct Scene {
	void PrepareScene();

	void UpdateScene(float time);

	void BindDrawingToCmdBuffer(VkCommandBuffer cmdBuffer) const;

	void Destroy();

	// Contains geometry, materials
	DedicatedBuffer vertexBuffer;
	DedicatedBuffer indexBuffer;

	VkPrimitiveTopology modelPrimitiveTopology;
	u32 modelInstanceCount;
	u32 modelIndexCount;
	u32 modelVertexCount;
	VkDeviceSize sizeofVertex;

	glm::mat4 modelWorld;
	glm::mat4 modelWorld2;
	glm::mat4 modelWorld4;

	// Contains light
	std::vector<DirLight> dirLights;
};

