#include "vulkan_helpers.h"



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
