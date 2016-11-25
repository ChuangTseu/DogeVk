#pragma once

#include <vector>

#include <glm/mat4x4.hpp>

#include "doge_vulkan.h"
#include "device_buffer_allocator.h"
#include "light.h"
#include "model.h"

struct Scene {
	void PrepareScene();

	void UpdateScene(float time);

	void BindDrawingToCmdBuffer(VkCommandBuffer cmdBuffer) const;

	void Destroy();

	// Contains geometry, materials
	DeviceBufferAllocator::Handle vertexBufferHandle;
	DeviceBufferAllocator::Handle indexBufferHandle;

	VkPrimitiveTopology modelPrimitiveTopology;
	u32 modelInstanceCount;
	u32 modelIndexCount;
	u32 modelVertexCount;
	VkDeviceSize sizeofVertex;

	glm::mat4 modelWorld;
	glm::mat4 modelWorld2;
	glm::mat4 modelWorld4;

	Model cubenorm;

	// Contains light
	std::vector<DirLight> dirLights;
};

