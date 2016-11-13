#pragma once

#include "doge_vulkan.h"

#include "custom_types.h"

#include <vector>

#include <glm/vec3.hpp>
#include <glm/glm.hpp>

struct DirLight {
	glm::vec3 m_color;
	glm::vec3 m_direction;
};

struct Vertex {
	glm::vec3 position;
	glm::vec3 normal;
	//vec2 texcoord;
	//vec3 tangent;
};

#include "dedicated_buffer.h"

struct Scene {
	void PrepareScene() {
		// VERTEX & INDEX BUFFER SETUP
		Vertex cubeVertices[] = {
			{ { -1.0f, -1.0f, 1.0f },{} },
			{ { 1.0f, -1.0f, 1.0f },{} },
			{ { 1.0f, 1.0f, 1.0f },{} },
			{ { -1.0f, 1.0f, 1.0f },{} },
			{ { -1.0f, -1.0f, -1.0f },{} },
			{ { 1.0f, -1.0f, -1.0f },{} },
			{ { 1.0f, 1.0f, -1.0f },{} },
			{ { -1.0f, 1.0f, -1.0f },{} }
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
		const u32 cubeInstanceCount = 12u;
		const u32 cubeIndexCount = cubeInstanceCount * 3u;

		modelPrimitiveTopology = cubePrimitiveTopology;
		modelInstanceCount = cubeInstanceCount;

		modelVertexCount = cubeVertexCount;
		Vertex* modelVertices = &(cubeVertices[0]);

		modelIndexCount = cubeIndexCount;
		u32* modelIndices = cubeIndices;

		sizeofVertex = sizeof(Vertex);

		const VkDeviceSize vertexBufferSize = modelVertexCount * sizeofVertex;
		const VkDeviceSize indexBufferSize = modelIndexCount * sizeof(u32);

		// BUG BUG BUG ? Don't know why, maybe I did something wrong, but the last vertex is zeroed out if not adding at least 4 to the buffer creation size
		vertexBuffer.Create(VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, vertexBufferSize + 4, DedicatedBuffer::eViaHostAccess);
		indexBuffer.Create(VK_BUFFER_USAGE_INDEX_BUFFER_BIT, indexBufferSize, DedicatedBuffer::eViaHostAccess);

		vertexBuffer.UploadData(VkDeviceSize{ 0 }, vertexBufferSize, modelVertices);
		indexBuffer.UploadData(VkDeviceSize{ 0 }, indexBufferSize, modelIndices);
	}

	void Destroy() {
		vertexBuffer.Destroy();
		indexBuffer.Destroy();
	}

	// Contains geometry, materials
	DedicatedBuffer vertexBuffer;
	DedicatedBuffer indexBuffer;

	VkPrimitiveTopology modelPrimitiveTopology;
	u32 modelInstanceCount;
	u32 modelIndexCount;
	u32 modelVertexCount;
	VkDeviceSize sizeofVertex;

	// Contains light
	std::vector<DirLight> dirLights;
};

struct Forward {
private:
	void SetupShaderStages();
	void SetupVertexInput();
	void SetupInputAssembly();
	void SetupViewport();
	void SetupRasterization();
	void SetupMultisampling();
	void SetupDepthStencil();
	void SetupColorBlend();
	void SetupRenderPass();
	void SetupPipelineLayout();
	void SetupGraphicsPipeline();
	void SetupRenderTargets();
	void SetupFrameRenderResources();

	void CleanupSetupOnlyResources();

public:

	void SetupPipeline() {
		SetupShaderStages();
		SetupVertexInput();
		SetupInputAssembly();
		SetupViewport();
		SetupRasterization();
		SetupMultisampling();
		SetupDepthStencil();
		SetupColorBlend();
		SetupRenderPass();
		SetupPipelineLayout();
		SetupGraphicsPipeline();
		SetupRenderTargets();
		SetupFrameRenderResources();

		CleanupSetupOnlyResources();
	}

	VkShaderModule vertexModule;
	VkShaderModule fragmentModule;
	VkPipelineShaderStageCreateInfo shaderStagesCreateInfo[2];

	VkVertexInputBindingDescription inputBindingDesc;
	VkVertexInputAttributeDescription vertexInputAttribDescs[2];
	VkPipelineVertexInputStateCreateInfo vertexInputStateCreateInfo;

	VkPipelineInputAssemblyStateCreateInfo inputAssemblyStateCreateInfo;

	VkViewport viewport;
	VkRect2D scissor;
	VkPipelineViewportStateCreateInfo viewportStateCreateInfo;

	VkPipelineRasterizationStateCreateInfo rasterizationStateCreateInfo;
	VkPipelineMultisampleStateCreateInfo multisampleStateCreateInfo;
	VkPipelineDepthStencilStateCreateInfo depthstencilStateCreateInfo;

	VkPipelineColorBlendAttachmentState singleColorBlendState;
	VkPipelineColorBlendStateCreateInfo colorblendStateCreateInfo;

	VkRenderPass renderPass;

	VkPipelineLayout pipelineLayout;

	VkPipeline pipeline;

	std::vector<bool> aIsFirstSwapImageUse;
	bool isFirstDepthStencilImageUse;

	VkImage gDepthStencilImage;
	VkDeviceMemory gDepthStencilImageMemory;
	VkImageView gDepthStencilView;

	VkSemaphore imageAcquiredSemaphore;
	VkSemaphore renderingCompleteSemaphore;

	VkFramebuffer framebuffer;
	VkCommandPool cmdPool;

	void SubmitFrameRender(const Scene & scene, VkQueue graphicQueue, VkQueue presentQueue);
	void CleanupAfterFrame();

	// Render geometry from viewpoint

	// In fragment shader, for each light, accumulate lighting

	void Destroy();
};

extern Forward gForward;
