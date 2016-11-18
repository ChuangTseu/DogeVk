#include "vulkan_helpers.h"

#include <assert.h>

#include "memory.h"

VkPipelineColorBlendAttachmentState create_VkPipelineColorBlendAttachmentState_disabledBlend(VkColorComponentFlags colorWriteMask)
{
	VkPipelineColorBlendAttachmentState colorBlendState;
	colorBlendState.blendEnable = VK_FALSE;
	colorBlendState.colorWriteMask = colorWriteMask;
	// Putting them to 0 as they are required to be valid enum even when blendEnable is false...
	// Allows user to ignore those fields when disabling blending and not caring
	colorBlendState.srcColorBlendFactor = (VkBlendFactor)0;
	colorBlendState.dstColorBlendFactor = (VkBlendFactor)0;
	colorBlendState.colorBlendOp = (VkBlendOp)0;
	colorBlendState.srcAlphaBlendFactor = (VkBlendFactor)0;
	colorBlendState.dstAlphaBlendFactor = (VkBlendFactor)0;
	colorBlendState.alphaBlendOp = (VkBlendOp)0;

	return colorBlendState;
}

void CreateDepthImageAndView(VkImage* image, VkImageView* view, VkDeviceMemory* memory, u32 w, u32 h, u32 queueFamilyIndex, bool isSampled)
{
	auto depthImageCreateInfo = vkstruct<VkImageCreateInfo>();
	depthImageCreateInfo.flags = 0;
	depthImageCreateInfo.imageType = VK_IMAGE_TYPE_2D;
	depthImageCreateInfo.format = VK_FORMAT_D32_SFLOAT;
	depthImageCreateInfo.extent = { w, h, 1 };
	depthImageCreateInfo.mipLevels = 1u;
	depthImageCreateInfo.arrayLayers = 1u;
	depthImageCreateInfo.samples = VK_SAMPLE_COUNT_1_BIT;
	depthImageCreateInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
	depthImageCreateInfo.usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
	if (isSampled) {
		depthImageCreateInfo.usage |= VK_IMAGE_USAGE_SAMPLED_BIT;
	}
	depthImageCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	depthImageCreateInfo.queueFamilyIndexCount = 1u;
	depthImageCreateInfo.pQueueFamilyIndices = &queueFamilyIndex;
	depthImageCreateInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;

	gVkLastRes = vkCreateImage(gDevice.VkHandle(), &depthImageCreateInfo, nullptr, image);
	VKFN_LAST_RES_SUCCESS_OR_QUIT(vkCreateImage);

	allocateNewImageMemory(image, memory, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

	gVkLastRes = vkBindImageMemory(gDevice.VkHandle(), *image, *memory, 0);
	VKFN_LAST_RES_SUCCESS_OR_QUIT(vkBindImageMemory);

	VkImageSubresourceRange depthImageSubresourceRange;
	depthImageSubresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
	depthImageSubresourceRange.baseMipLevel = 0;
	depthImageSubresourceRange.levelCount = 1;
	depthImageSubresourceRange.baseArrayLayer = 0;
	depthImageSubresourceRange.layerCount = 1;

	VkImageViewCreateInfo depthViewCreateInfo = vkstruct<VkImageViewCreateInfo>();
	depthViewCreateInfo.image = *image;
	depthViewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
	depthViewCreateInfo.format = VK_FORMAT_D32_SFLOAT;
	depthViewCreateInfo.components = identityComponentMapping;
	depthViewCreateInfo.subresourceRange = depthImageSubresourceRange;

	gVkLastRes = vkCreateImageView(gDevice.VkHandle(), &depthViewCreateInfo, nullptr, view);
	VKFN_LAST_RES_SUCCESS_OR_QUIT(vkCreateImageView);
}

VkSemaphore create_VkSemaphore()
{
	static const auto default_semCreateInfo = vkstruct<VkSemaphoreCreateInfo>();

	VkSemaphore semaphore;
	gVkLastRes = vkCreateSemaphore(gDevice.VkHandle(), &default_semCreateInfo, nullptr, &semaphore);
	VKFN_LAST_RES_SUCCESS_OR_QUIT(vkCreateSemaphore);

	return semaphore;
}

VkRenderPass create_VkRenderPass(const VkRenderPassCreateInfo* renderPassCreateInfo)
{
	VkRenderPass renderPass;
	gVkLastRes = vkCreateRenderPass(gDevice.VkHandle(), renderPassCreateInfo, nullptr, &renderPass);
	VKFN_LAST_RES_SUCCESS_OR_QUIT(vkCreateRenderPass);

	return renderPass;
}

VkShaderModule create_VkShaderModule_fromFile(const char* filepath)
{
	size_t shaderModuleCodeSize = size_t{ 0 };
	const u32* shaderModuleCode = nullptr;

	std::unique_ptr<char> shaderFileBuffer = readFileToCharBuffer(filepath, &shaderModuleCodeSize);
	shaderModuleCode = (u32*)shaderFileBuffer.get();

	auto shaderModuleCreateInfo = vkstruct<VkShaderModuleCreateInfo>();
	shaderModuleCreateInfo.codeSize = shaderModuleCodeSize;
	shaderModuleCreateInfo.pCode = shaderModuleCode;

	VkShaderModule shaderModule;
	gVkLastRes = vkCreateShaderModule(gDevice.VkHandle(), &shaderModuleCreateInfo, nullptr, &shaderModule);
	VKFN_LAST_RES_SUCCESS_OR_QUIT(vkCreateShaderModule);

	return shaderModule;
}

void cmdTransitionSimpleImageFromInitialState(
	VkCommandBuffer cmdBuffer, VkImage image,
	VkImageAspectFlagBits aspectMask, VkImageLayout initialLayout, VkImageLayout newLayout,
	VkAccessFlags firstMemoryAccessMask, VkPipelineStageFlags firstDependentStage) {

	assert(initialLayout == VK_IMAGE_LAYOUT_UNDEFINED || initialLayout == VK_IMAGE_LAYOUT_PREINITIALIZED);

	VkImageSubresourceRange imageSubresourceRange;
	imageSubresourceRange.aspectMask = aspectMask;
	imageSubresourceRange.baseMipLevel = 0u;
	imageSubresourceRange.levelCount = 1u;
	imageSubresourceRange.baseArrayLayer = 0u;
	imageSubresourceRange.layerCount = 1u;

	auto imageBarrier = vkstruct<VkImageMemoryBarrier>();
	imageBarrier.srcAccessMask = 0;
	imageBarrier.dstAccessMask = firstMemoryAccessMask;
	imageBarrier.oldLayout = initialLayout;
	imageBarrier.newLayout = newLayout;
	imageBarrier.image = image;
	imageBarrier.subresourceRange = imageSubresourceRange;

	vkCmdPipelineBarrier(cmdBuffer, firstDependentStage, firstDependentStage,
		0, 0u, nullptr, 0u, nullptr, 1u, &imageBarrier);
};
