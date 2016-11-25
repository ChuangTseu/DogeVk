#pragma once

#include "doge_vulkan.h"

#include "custom_types.h"

#include <vector>

#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "global_descriptor_sets.h"
#include "image.h"
#include "vertex.h"
#include "scene.h"
#include "light.h"

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
	void PrepareShadowMapsDescriptorSet();
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
		PrepareShadowMapsDescriptorSet();
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

	VkSemaphore imageAcquiredSemaphore;
	VkSemaphore renderingCompleteSemaphore;

	VkSampler shadowMapSampler;
	VkDescriptorSetLayout shadowMapsDescriptorSetLayout;
	VkDescriptorPool shadowMapsDescriptorPool;
	VkDescriptorSet shadowMapsDescriptorSet;



	void SubmitFrameRender(VkQueue graphicQueue, VkQueue presentQueue);

	// Render geometry from viewpoint

	// In fragment shader, for each light, accumulate lighting

	void Destroy();
};

extern Forward gForward;
