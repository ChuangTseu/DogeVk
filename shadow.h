#pragma once

#include "doge_vulkan.h"

#include "custom_types.h"

#include <vector>

#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "dedicated_buffer.h"
#include "global_descriptor_sets.h"
#include "image.h"
#include "scene.h"

struct Shadow {
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
	void CookCmdBuffer(const Scene& scene);

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
		CookCmdBuffer(scene);

		CleanupSetupOnlyResources();
	}

	static const u32 SHADOWMAP_RES = 1024u;

	VkShaderModule vertexModule;
	VkPipelineShaderStageCreateInfo shaderStageCreateInfo;

	VkPipelineVertexInputStateCreateInfo vertexInputStateCreateInfo;

	VkPipelineInputAssemblyStateCreateInfo inputAssemblyStateCreateInfo;

	VkViewport viewport;
	VkRect2D scissor;
	VkPipelineViewportStateCreateInfo viewportStateCreateInfo;

	VkPipelineRasterizationStateCreateInfo rasterizationStateCreateInfo;
	VkPipelineMultisampleStateCreateInfo multisampleStateCreateInfo;
	VkPipelineDepthStencilStateCreateInfo depthstencilStateCreateInfo;

	VkPipelineColorBlendAttachmentState noColorBlendState;
	VkPipelineColorBlendStateCreateInfo colorblendStateCreateInfo;

	VkRenderPass mRenderPass;

	VkPipelineLayout mPipelineLayout;
	VkPipeline mPipeline;

	VkCommandPool mCmdPool;

	Image mDepthImage;
	VkCommandBuffer mCmdBuffer;
	VkFramebuffer mFramebuffer;

	VkSemaphore shadowMapCompleteSemaphore;


	void SubmitFrameRender(VkQueue graphicQueue);

	void Destroy();
};

extern Shadow gShadow;
