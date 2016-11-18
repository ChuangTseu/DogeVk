#pragma once

#include "doge_vulkan.h"

#include "device.h"

#define UNINITIALIZED_VKFLAGBITS(VkFlagType) ((VkFlagType)-1)
#define UNINITIALIZED_VKFLAGS ((VkFlags)-1)
#define UNINITIALIZED_VKENUM(VkEnumType) ((VkEnumType)-1)
#define UNINITIALIZED_VKBOOL ((VkBool32)-1)
#define UNINITIALIZED_VKFLOAT (0.f)
#define UNINITIALIZED_VKSTRUCT {}
#define UNINITIALIZED_VKSIZE VkDeviceSize{0}
#define UNINITIALIZED_VKINDEX ((uint32_t)~0)

// Return the asked Vk struct with any default (or ignored parameter) set
// Warning! Any non default fields will be uninitialized and must be manually set
template <class T> 
inline T vkstruct();

template <> inline VkSemaphoreCreateInfo vkstruct() {
	VkSemaphoreCreateInfo semCreateInfo;
	semCreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
	semCreateInfo.pNext = nullptr;
	semCreateInfo.flags = 0; // reserved for future use

	return semCreateInfo;
}

template <> inline VkSubpassDescription vkstruct() {
	VkSubpassDescription subpassDesc;
	subpassDesc.flags = 0;
	subpassDesc.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
	subpassDesc.inputAttachmentCount = 0u;
	subpassDesc.pInputAttachments = nullptr;
	subpassDesc.colorAttachmentCount = 0u;
	subpassDesc.pColorAttachments = nullptr;
	subpassDesc.pResolveAttachments = nullptr;
	subpassDesc.pDepthStencilAttachment = nullptr;
	subpassDesc.preserveAttachmentCount = 0u;
	subpassDesc.pPreserveAttachments = nullptr;

	return subpassDesc;
}

template <> inline VkRenderPassCreateInfo vkstruct() {
	VkRenderPassCreateInfo renderPassCreateInfo;
	renderPassCreateInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
	renderPassCreateInfo.pNext = nullptr;
	renderPassCreateInfo.flags = 0; // reserved for future use
	renderPassCreateInfo.attachmentCount = 0u;
	renderPassCreateInfo.pAttachments = nullptr;
	renderPassCreateInfo.subpassCount = 0u;
	renderPassCreateInfo.pSubpasses = nullptr;
	renderPassCreateInfo.dependencyCount = 0u;
	renderPassCreateInfo.pDependencies = nullptr;

	return renderPassCreateInfo;
}

template <> inline VkShaderModuleCreateInfo vkstruct() {
	VkShaderModuleCreateInfo shaderModuleCreateInfo;
	shaderModuleCreateInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	shaderModuleCreateInfo.pNext = nullptr;
	shaderModuleCreateInfo.flags = 0; // reserved for future use
	shaderModuleCreateInfo.codeSize = 0u;
	shaderModuleCreateInfo.pCode = nullptr;

	return shaderModuleCreateInfo;
}

template <> inline VkPipelineShaderStageCreateInfo vkstruct() {
	VkPipelineShaderStageCreateInfo shaderStageCreateInfo;
	shaderStageCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	shaderStageCreateInfo.pNext = nullptr;
	shaderStageCreateInfo.flags = 0; // reserved for future use
	shaderStageCreateInfo.stage = UNINITIALIZED_VKFLAGBITS(VkShaderStageFlagBits);
	shaderStageCreateInfo.module = VK_NULL_HANDLE;
	shaderStageCreateInfo.pName = nullptr;
	shaderStageCreateInfo.pSpecializationInfo = nullptr;

	return shaderStageCreateInfo;
}

template <> inline VkPipelineVertexInputStateCreateInfo vkstruct() {
	VkPipelineVertexInputStateCreateInfo vertexInputStateCreateInfo;
	vertexInputStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
	vertexInputStateCreateInfo.pNext = nullptr;
	vertexInputStateCreateInfo.flags = 0; // reserved for future use
	vertexInputStateCreateInfo.vertexBindingDescriptionCount = 0u;
	vertexInputStateCreateInfo.pVertexBindingDescriptions = nullptr;
	vertexInputStateCreateInfo.vertexAttributeDescriptionCount = 0u;
	vertexInputStateCreateInfo.pVertexAttributeDescriptions = nullptr;

	return vertexInputStateCreateInfo;
}

template <> inline VkPipelineInputAssemblyStateCreateInfo vkstruct() {
	VkPipelineInputAssemblyStateCreateInfo inputAssemblyStateCreateInfo;
	inputAssemblyStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
	inputAssemblyStateCreateInfo.pNext = nullptr;
	inputAssemblyStateCreateInfo.flags = 0; // reserved for future use
	inputAssemblyStateCreateInfo.topology = UNINITIALIZED_VKENUM(VkPrimitiveTopology);
	inputAssemblyStateCreateInfo.primitiveRestartEnable = UNINITIALIZED_VKBOOL;

	return inputAssemblyStateCreateInfo;
}

template <> inline VkPipelineViewportStateCreateInfo vkstruct() {
	VkPipelineViewportStateCreateInfo viewportStateCreateInfo;
	viewportStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
	viewportStateCreateInfo.pNext = nullptr;
	viewportStateCreateInfo.flags = 0; // reserved for future use
	viewportStateCreateInfo.viewportCount = 0u;
	viewportStateCreateInfo.pViewports = nullptr;
	viewportStateCreateInfo.scissorCount = 0u;
	viewportStateCreateInfo.pScissors = nullptr;

	return viewportStateCreateInfo;
}

template <> inline VkPipelineRasterizationStateCreateInfo vkstruct() {
	VkPipelineRasterizationStateCreateInfo rasterizationStateCreateInfo;
	rasterizationStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
	rasterizationStateCreateInfo.pNext = nullptr;
	rasterizationStateCreateInfo.flags = 0; // reserved for future use
	rasterizationStateCreateInfo.depthClampEnable = UNINITIALIZED_VKBOOL;
	rasterizationStateCreateInfo.rasterizerDiscardEnable = UNINITIALIZED_VKBOOL;
	rasterizationStateCreateInfo.polygonMode = UNINITIALIZED_VKENUM(VkPolygonMode);
	rasterizationStateCreateInfo.cullMode = UNINITIALIZED_VKFLAGS;
	rasterizationStateCreateInfo.frontFace = UNINITIALIZED_VKENUM(VkFrontFace);
	rasterizationStateCreateInfo.depthBiasEnable = UNINITIALIZED_VKBOOL;
	rasterizationStateCreateInfo.depthBiasConstantFactor = UNINITIALIZED_VKFLOAT;
	rasterizationStateCreateInfo.depthBiasClamp = UNINITIALIZED_VKFLOAT;
	rasterizationStateCreateInfo.depthBiasSlopeFactor = UNINITIALIZED_VKFLOAT;
	rasterizationStateCreateInfo.lineWidth = UNINITIALIZED_VKFLOAT;

	return rasterizationStateCreateInfo;
}

template <> inline VkPipelineMultisampleStateCreateInfo vkstruct() {
	VkPipelineMultisampleStateCreateInfo multisampleStateCreateInfo;
	multisampleStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
	multisampleStateCreateInfo.pNext = nullptr;
	multisampleStateCreateInfo.flags = 0; // reserved for future use
	multisampleStateCreateInfo.rasterizationSamples = UNINITIALIZED_VKFLAGBITS(VkSampleCountFlagBits);
	multisampleStateCreateInfo.sampleShadingEnable = UNINITIALIZED_VKBOOL;
	multisampleStateCreateInfo.minSampleShading = UNINITIALIZED_VKFLOAT;
	multisampleStateCreateInfo.pSampleMask = nullptr;
	multisampleStateCreateInfo.alphaToCoverageEnable = UNINITIALIZED_VKBOOL;
	multisampleStateCreateInfo.alphaToOneEnable = UNINITIALIZED_VKBOOL;

	return multisampleStateCreateInfo;
}

template <> inline VkPipelineDepthStencilStateCreateInfo vkstruct() {
	VkPipelineDepthStencilStateCreateInfo depthstencilStateCreateInfo;
	depthstencilStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
	depthstencilStateCreateInfo.pNext = nullptr;
	depthstencilStateCreateInfo.flags = 0; // reserved for future use
	depthstencilStateCreateInfo.depthTestEnable = UNINITIALIZED_VKBOOL;
	depthstencilStateCreateInfo.depthWriteEnable = UNINITIALIZED_VKBOOL;
	depthstencilStateCreateInfo.depthCompareOp = UNINITIALIZED_VKENUM(VkCompareOp);
	depthstencilStateCreateInfo.depthBoundsTestEnable = UNINITIALIZED_VKBOOL;
	depthstencilStateCreateInfo.stencilTestEnable = UNINITIALIZED_VKBOOL;
	depthstencilStateCreateInfo.front = UNINITIALIZED_VKSTRUCT;
	depthstencilStateCreateInfo.back = UNINITIALIZED_VKSTRUCT;
	depthstencilStateCreateInfo.minDepthBounds = UNINITIALIZED_VKFLOAT;
	depthstencilStateCreateInfo.maxDepthBounds = UNINITIALIZED_VKFLOAT;

	return depthstencilStateCreateInfo;
}

template <> inline VkPipelineColorBlendAttachmentState vkstruct() {
	VkPipelineColorBlendAttachmentState colorBlendState;
	colorBlendState.blendEnable = UNINITIALIZED_VKBOOL;
	colorBlendState.srcColorBlendFactor = UNINITIALIZED_VKENUM(VkBlendFactor);
	colorBlendState.dstColorBlendFactor = UNINITIALIZED_VKENUM(VkBlendFactor);
	colorBlendState.colorBlendOp = UNINITIALIZED_VKENUM(VkBlendOp);
	colorBlendState.srcAlphaBlendFactor = UNINITIALIZED_VKENUM(VkBlendFactor);
	colorBlendState.dstAlphaBlendFactor = UNINITIALIZED_VKENUM(VkBlendFactor);
	colorBlendState.alphaBlendOp = UNINITIALIZED_VKENUM(VkBlendOp);
	colorBlendState.colorWriteMask = UNINITIALIZED_VKFLAGS;

	return colorBlendState;
}

template <> inline VkPipelineColorBlendStateCreateInfo vkstruct() {
	VkPipelineColorBlendStateCreateInfo colorblendStateCreateInfo;
	colorblendStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
	colorblendStateCreateInfo.pNext = nullptr;
	colorblendStateCreateInfo.flags = 0; // reserved for future use
	colorblendStateCreateInfo.logicOpEnable = UNINITIALIZED_VKBOOL;
	colorblendStateCreateInfo.logicOp = UNINITIALIZED_VKENUM(VkLogicOp);
	colorblendStateCreateInfo.attachmentCount = 0u;
	colorblendStateCreateInfo.pAttachments = nullptr;
	colorblendStateCreateInfo.blendConstants[0] = UNINITIALIZED_VKFLOAT;
	colorblendStateCreateInfo.blendConstants[1] = UNINITIALIZED_VKFLOAT;
	colorblendStateCreateInfo.blendConstants[2] = UNINITIALIZED_VKFLOAT;
	colorblendStateCreateInfo.blendConstants[3] = UNINITIALIZED_VKFLOAT;

	return colorblendStateCreateInfo;
}

template <> inline VkDescriptorSetLayoutCreateInfo vkstruct() {
	VkDescriptorSetLayoutCreateInfo descriptorSetLayoutCreateInfo;
	descriptorSetLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	descriptorSetLayoutCreateInfo.pNext = nullptr;
	descriptorSetLayoutCreateInfo.flags = 0; // reserved for future use
	descriptorSetLayoutCreateInfo.bindingCount = 0u;
	descriptorSetLayoutCreateInfo.pBindings = nullptr;

	return descriptorSetLayoutCreateInfo;
}

template <> inline VkPipelineLayoutCreateInfo vkstruct() {
	VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo;
	pipelineLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	pipelineLayoutCreateInfo.pNext = nullptr;
	pipelineLayoutCreateInfo.flags = 0; // reserved for future use
	pipelineLayoutCreateInfo.setLayoutCount = 0u;
	pipelineLayoutCreateInfo.pSetLayouts = nullptr;
	pipelineLayoutCreateInfo.pushConstantRangeCount = 0u;
	pipelineLayoutCreateInfo.pPushConstantRanges = nullptr;

	return pipelineLayoutCreateInfo;
}

template <> inline VkGraphicsPipelineCreateInfo vkstruct() {
	VkGraphicsPipelineCreateInfo pipelineCreateInfo;
	pipelineCreateInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
	pipelineCreateInfo.pNext = nullptr;
	pipelineCreateInfo.flags = UNINITIALIZED_VKFLAGS;
	pipelineCreateInfo.stageCount = 0u;
	pipelineCreateInfo.pStages = nullptr;
	pipelineCreateInfo.pVertexInputState = nullptr;
	pipelineCreateInfo.pInputAssemblyState = nullptr;
	pipelineCreateInfo.pTessellationState = nullptr;
	pipelineCreateInfo.pViewportState = nullptr;
	pipelineCreateInfo.pRasterizationState = nullptr;
	pipelineCreateInfo.pMultisampleState = nullptr;
	pipelineCreateInfo.pDepthStencilState = nullptr;
	pipelineCreateInfo.pColorBlendState = nullptr;
	pipelineCreateInfo.pDynamicState = nullptr;
	pipelineCreateInfo.layout = VK_NULL_HANDLE;
	pipelineCreateInfo.renderPass = VK_NULL_HANDLE;
	pipelineCreateInfo.subpass = UNINITIALIZED_VKINDEX;
	pipelineCreateInfo.basePipelineHandle = VK_NULL_HANDLE;
	pipelineCreateInfo.basePipelineIndex = -1;

	return pipelineCreateInfo;
}

template <> inline VkDescriptorPoolCreateInfo vkstruct() {
	VkDescriptorPoolCreateInfo descriptorPoolCreateInfo;
	descriptorPoolCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	descriptorPoolCreateInfo.pNext = nullptr;
	descriptorPoolCreateInfo.flags = UNINITIALIZED_VKFLAGS;
	descriptorPoolCreateInfo.maxSets = 0u;
	descriptorPoolCreateInfo.poolSizeCount = 0u;
	descriptorPoolCreateInfo.pPoolSizes = nullptr;

	return descriptorPoolCreateInfo;
}

template <> inline VkDescriptorSetAllocateInfo vkstruct() {
	VkDescriptorSetAllocateInfo descriptorSetAllocateInfo;
	descriptorSetAllocateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	descriptorSetAllocateInfo.pNext = nullptr;
	descriptorSetAllocateInfo.descriptorPool = VK_NULL_HANDLE;
	descriptorSetAllocateInfo.descriptorSetCount = 0u;
	descriptorSetAllocateInfo.pSetLayouts = nullptr;

	return descriptorSetAllocateInfo;
}

template <> inline VkImageMemoryBarrier vkstruct() {
	VkImageMemoryBarrier imageBarrier;
	imageBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
	imageBarrier.pNext = nullptr;
	imageBarrier.srcAccessMask = UNINITIALIZED_VKFLAGS;
	imageBarrier.dstAccessMask = UNINITIALIZED_VKFLAGS;
	imageBarrier.oldLayout = UNINITIALIZED_VKENUM(VkImageLayout);
	imageBarrier.newLayout = UNINITIALIZED_VKENUM(VkImageLayout);
	imageBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	imageBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	imageBarrier.image = VK_NULL_HANDLE;
	imageBarrier.subresourceRange = UNINITIALIZED_VKSTRUCT;

	return imageBarrier;
}

template <> inline VkMappedMemoryRange vkstruct() {
	VkMappedMemoryRange mappedMemoryRange;
	mappedMemoryRange.sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;
	mappedMemoryRange.pNext = nullptr;
	mappedMemoryRange.memory = VK_NULL_HANDLE;
	mappedMemoryRange.offset = UNINITIALIZED_VKSIZE;
	mappedMemoryRange.size = UNINITIALIZED_VKSIZE;

	return mappedMemoryRange;
}

template <> inline VkWriteDescriptorSet vkstruct() {
	VkWriteDescriptorSet descriptorWrite;
	descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	descriptorWrite.pNext = nullptr;
	descriptorWrite.dstSet = VK_NULL_HANDLE;
	descriptorWrite.dstBinding = UNINITIALIZED_VKINDEX;
	descriptorWrite.dstArrayElement = UNINITIALIZED_VKINDEX;
	descriptorWrite.descriptorCount = 0u;
	descriptorWrite.descriptorType = UNINITIALIZED_VKENUM(VkDescriptorType);
	descriptorWrite.pImageInfo = nullptr;
	descriptorWrite.pBufferInfo = nullptr;
	descriptorWrite.pTexelBufferView = nullptr;

	return descriptorWrite;
}

template <> inline VkImageCreateInfo vkstruct() {
	VkImageCreateInfo imageCreateInfo;
	imageCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
	imageCreateInfo.pNext = nullptr;
	imageCreateInfo.flags = UNINITIALIZED_VKFLAGS;
	imageCreateInfo.imageType = UNINITIALIZED_VKENUM(VkImageType);
	imageCreateInfo.format = UNINITIALIZED_VKENUM(VkFormat);
	imageCreateInfo.extent = UNINITIALIZED_VKSTRUCT;
	imageCreateInfo.mipLevels = 0u;
	imageCreateInfo.arrayLayers = 0u;
	imageCreateInfo.samples = UNINITIALIZED_VKFLAGBITS(VkSampleCountFlagBits);
	imageCreateInfo.tiling = UNINITIALIZED_VKENUM(VkImageTiling);
	imageCreateInfo.usage = UNINITIALIZED_VKFLAGS;
	imageCreateInfo.sharingMode = UNINITIALIZED_VKENUM(VkSharingMode);
	imageCreateInfo.queueFamilyIndexCount = 0u;
	imageCreateInfo.pQueueFamilyIndices = nullptr;
	imageCreateInfo.initialLayout = UNINITIALIZED_VKENUM(VkImageLayout);

	return imageCreateInfo;
}

template <> inline VkImageViewCreateInfo vkstruct() {
	VkImageViewCreateInfo viewCreateInfo;
	viewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
	viewCreateInfo.pNext = nullptr;
	viewCreateInfo.flags = 0; // reserved for future use
	viewCreateInfo.image = VK_NULL_HANDLE;
	viewCreateInfo.viewType = UNINITIALIZED_VKENUM(VkImageViewType);
	viewCreateInfo.format = UNINITIALIZED_VKENUM(VkFormat);
	viewCreateInfo.components = identityComponentMapping;
	viewCreateInfo.subresourceRange = UNINITIALIZED_VKSTRUCT;

	return viewCreateInfo;
}

template <> inline VkFramebufferCreateInfo vkstruct() {
	VkFramebufferCreateInfo framebufferCreateInfo;
	framebufferCreateInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
	framebufferCreateInfo.pNext = nullptr;
	framebufferCreateInfo.flags = UNINITIALIZED_VKFLAGS;
	framebufferCreateInfo.renderPass = VK_NULL_HANDLE;
	framebufferCreateInfo.attachmentCount = 0u;
	framebufferCreateInfo.pAttachments = nullptr;
	framebufferCreateInfo.width = 0u;
	framebufferCreateInfo.height = 0u;
	framebufferCreateInfo.layers = 0u;

	return framebufferCreateInfo;
}

template <> inline VkCommandPoolCreateInfo vkstruct() {
	VkCommandPoolCreateInfo cmdPoolCreateInfo;
	cmdPoolCreateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	cmdPoolCreateInfo.pNext = nullptr;
	cmdPoolCreateInfo.flags = UNINITIALIZED_VKFLAGS;
	cmdPoolCreateInfo.queueFamilyIndex = 0u;


	return cmdPoolCreateInfo;
}

template <> inline VkCommandBufferAllocateInfo vkstruct() {
	VkCommandBufferAllocateInfo cmdBufferAllocInfo;
	cmdBufferAllocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	cmdBufferAllocInfo.pNext = nullptr;
	cmdBufferAllocInfo.commandPool = VK_NULL_HANDLE;
	cmdBufferAllocInfo.level = UNINITIALIZED_VKENUM(VkCommandBufferLevel);
	cmdBufferAllocInfo.commandBufferCount = 0u;

	return cmdBufferAllocInfo;
}

template <> inline VkCommandBufferBeginInfo vkstruct() {
	VkCommandBufferBeginInfo cmdBufferBeginInfo;
	cmdBufferBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	cmdBufferBeginInfo.pNext = nullptr;
	cmdBufferBeginInfo.flags = UNINITIALIZED_VKFLAGS;
	cmdBufferBeginInfo.pInheritanceInfo = nullptr;

	return cmdBufferBeginInfo;
}

template <> inline VkRenderPassBeginInfo vkstruct() {
	VkRenderPassBeginInfo renderPassBeginInfo;
	renderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
	renderPassBeginInfo.pNext = nullptr;
	renderPassBeginInfo.renderPass = VK_NULL_HANDLE;
	renderPassBeginInfo.framebuffer = VK_NULL_HANDLE;
	renderPassBeginInfo.renderArea = UNINITIALIZED_VKSTRUCT;
	renderPassBeginInfo.clearValueCount = 0u;
	renderPassBeginInfo.pClearValues = nullptr;

	return renderPassBeginInfo;
}

template <> inline VkSubmitInfo vkstruct() {
	VkSubmitInfo submitInfo;
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submitInfo.pNext = nullptr;
	submitInfo.waitSemaphoreCount = 0u;
	submitInfo.pWaitSemaphores = nullptr;
	submitInfo.pWaitDstStageMask = nullptr;
	submitInfo.commandBufferCount = 0u;
	submitInfo.pCommandBuffers = nullptr;
	submitInfo.signalSemaphoreCount = 0u;
	submitInfo.pSignalSemaphores = nullptr;

	return submitInfo;
}

template <> inline VkPresentInfoKHR vkstruct() {
	VkPresentInfoKHR presentInfo;
	presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
	presentInfo.pNext = nullptr;
	presentInfo.waitSemaphoreCount = 0u;
	presentInfo.pWaitSemaphores = nullptr;
	presentInfo.swapchainCount = 0u;
	presentInfo.pSwapchains = nullptr;
	presentInfo.pImageIndices = nullptr;
	presentInfo.pResults = nullptr;

	return presentInfo;
}

template <> inline VkBufferCreateInfo vkstruct() {
	VkBufferCreateInfo bufferCreateInfo;
	bufferCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	bufferCreateInfo.pNext = nullptr;
	bufferCreateInfo.flags = UNINITIALIZED_VKFLAGS;
	bufferCreateInfo.size = UNINITIALIZED_VKSIZE;
	bufferCreateInfo.usage = UNINITIALIZED_VKFLAGS;
	bufferCreateInfo.sharingMode = UNINITIALIZED_VKENUM(VkSharingMode);
	bufferCreateInfo.queueFamilyIndexCount = 0u;
	bufferCreateInfo.pQueueFamilyIndices = nullptr;

	return bufferCreateInfo;
}

template <> inline VkMemoryAllocateInfo vkstruct() {
	VkMemoryAllocateInfo memAllocInfo;
	memAllocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	memAllocInfo.pNext = nullptr;
	memAllocInfo.allocationSize = UNINITIALIZED_VKSIZE;
	memAllocInfo.memoryTypeIndex = UNINITIALIZED_VKINDEX;

	return memAllocInfo;
}

//template <> inline VkStruct vkstruct() {
//
//
//	return theCreated;
//}

#define APPLY_2STEPS_QUERY_TO_COUNT_AND_VECTOR_0PARAM(fnVkQuery, countVar, vectorVar) \
	gVkLastRes = fnVkQuery(&countVar, nullptr); \
	VKFN_LAST_RES_SUCCESS_OR_QUIT(fnVkQuery);\
	if (countVar > 0) { \
		vectorVar.resize(countVar); \
		gVkLastRes = fnVkQuery(&countVar, &vectorVar[0]); \
		VKFN_LAST_RES_SUCCESS_OR_QUIT(fnVkQuery); \
	}

#define APPLY_2STEPS_QUERY_TO_COUNT_AND_VECTOR_1PARAM(fnVkQuery, param1, countVar, vectorVar) \
	gVkLastRes = fnVkQuery(param1, &countVar, nullptr); \
	VKFN_LAST_RES_SUCCESS_OR_QUIT(fnVkQuery);\
	if (countVar > 0) { \
		vectorVar.resize(countVar); \
		gVkLastRes = fnVkQuery(param1, &countVar, &vectorVar[0]); \
		VKFN_LAST_RES_SUCCESS_OR_QUIT(fnVkQuery); \
	}

#define APPLY_2STEPS_QUERY_TO_COUNT_AND_VECTOR_2PARAM(fnVkQuery, param1, param2, countVar, vectorVar) \
	gVkLastRes = fnVkQuery(param1, param2, &countVar, nullptr); \
	VKFN_LAST_RES_SUCCESS_OR_QUIT(fnVkQuery);\
	if (countVar > 0) { \
		vectorVar.resize(countVar); \
		gVkLastRes = fnVkQuery(param1, param2, &countVar, &vectorVar[0]); \
		VKFN_LAST_RES_SUCCESS_OR_QUIT(fnVkQuery); \
	}

VkSemaphore create_VkSemaphore();

VkRenderPass create_VkRenderPass(const VkRenderPassCreateInfo* renderPassCreateInfo);

VkShaderModule create_VkShaderModule_fromFile(const char* filepath);

VkPipelineColorBlendAttachmentState create_VkPipelineColorBlendAttachmentState_disabledBlend(VkColorComponentFlags colorWriteMask);

void CreateDepthImageAndView(VkImage* image, VkImageView* view, VkDeviceMemory* memory, u32 w, u32 h, u32 queueFamilyIndex, bool isSampled);
void cmdTransitionSimpleImageFromInitialState(VkCommandBuffer cmdBuffer, VkImage image, VkImageAspectFlagBits aspectMask, VkImageLayout initialLayout, VkImageLayout newLayout, VkAccessFlags firstMemoryAccessMask, VkPipelineStageFlags firstDependentStage);
