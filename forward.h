#pragma once

#include "doge_vulkan.h"

#include "custom_types.h"

#include <vector>

#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

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
#include "global_descriptor_sets.h"

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

		modelInstanceCount = 3;
	}

	void UpdateScene(float time) {
		const float timeFactor = 0.1f;

		modelWorld = glm::translate(glm::mat4(1.f), glm::vec3(0.f, glm::sin(time*timeFactor), 0.f));
		modelWorld2 = glm::translate(glm::mat4(1.f), glm::vec3(1.3f*glm::sin(time*timeFactor), 0.f, 1.3f*glm::cos(time*timeFactor)));

		PerObjectCB& perObjectCB = GlobalDescriptorSets::GetMutableCBuffer<PerObjectCB>();
		perObjectCB.g_world[0] = modelWorld;
		perObjectCB.g_world[1] = modelWorld2;
		GlobalDescriptorSets::CommitCBuffer<PerObjectCB>();
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

	glm::mat4 modelWorld;
	glm::mat4 modelWorld2;

	// Contains light
	std::vector<DirLight> dirLights;
};

struct Image {
	VkImage image;
	VkImageView view;
	VkDeviceMemory memory;

	void Destroy() {
		vkFreeMemory(gDevice.VkHandle(), memory, nullptr);
		vkDestroyImageView(gDevice.VkHandle(), view, nullptr);
		vkDestroyImage(gDevice.VkHandle(), image, nullptr);
	}
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
	void SetupCmdBufferPool();
	void SetupFrameRenderResources();

	void TransitionInitiallyUndefinedImages();
	void CookSpawChainCmdBuffers(const Scene& scene);

	void CleanupSetupOnlyResources();

public:

	void SetupPipeline(const Scene& scene) {
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
		SetupCmdBufferPool();
		SetupFrameRenderResources();

		TransitionInitiallyUndefinedImages();
		CookSpawChainCmdBuffers(scene);

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

	VkRenderPass mRenderPass;

	VkPipelineLayout mPipelineLayout;
	VkPipeline mPipeline;

	VkCommandPool mCmdPool;

	std::vector<Image> mDepthImages;
	std::vector<VkCommandBuffer> mDrawTriangleCmdBuffers;
	std::vector<VkFramebuffer> mFramebuffers;

	//VkImage gDepthStencilImage;
	//VkDeviceMemory gDepthStencilImageMemory;
	//VkImageView gDepthStencilView;

	VkSemaphore imageAcquiredSemaphore;
	VkSemaphore renderingCompleteSemaphore;


	void SubmitFrameRender(const Scene & scene, VkQueue graphicQueue, VkQueue presentQueue);

	// Render geometry from viewpoint

	// In fragment shader, for each light, accumulate lighting

	void Destroy();
};

extern Forward gForward;
