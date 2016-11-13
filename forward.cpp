#include "forward.h"

#include "vulkan_helpers.h"

#include "config.h"

#include "swapchain.h"
#include "dedicated_buffer.h"
#include "memory.h"
#include "global_descriptor_sets.h"

Forward gForward;

void Forward::SetupShaderStages()
{
	vertexModule = create_VkShaderModule_fromFile(SHADERS_FOLDER_PATH "simple_vert.spv");
	fragmentModule = create_VkShaderModule_fromFile(SHADERS_FOLDER_PATH "simple_frag.spv");

	auto vertexStageCreateInfo = vkstruct<VkPipelineShaderStageCreateInfo>();
	vertexStageCreateInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
	vertexStageCreateInfo.module = vertexModule;
	vertexStageCreateInfo.pName = "main";

	auto fragmentStageCreateInfo = vkstruct<VkPipelineShaderStageCreateInfo>();
	fragmentStageCreateInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
	fragmentStageCreateInfo.module = fragmentModule;
	fragmentStageCreateInfo.pName = "main";

	shaderStagesCreateInfo[0] = vertexStageCreateInfo;
	shaderStagesCreateInfo[1] = fragmentStageCreateInfo;
};

void Forward::SetupVertexInput()
{
	inputBindingDesc.binding = 0u;
	inputBindingDesc.stride = sizeof(Vertex); // Attributes are interleaved for now
	inputBindingDesc.inputRate = VK_VERTEX_INPUT_RATE_VERTEX; // TODO check the real meaning of this

	VkVertexInputAttributeDescription positionInputAttribDesc;
	positionInputAttribDesc.location = 0u;
	positionInputAttribDesc.binding = 0u;
	positionInputAttribDesc.format = VK_FORMAT_R32G32B32_SFLOAT;
	positionInputAttribDesc.offset = 0u;

	VkVertexInputAttributeDescription normalInputAttribDesc;
	normalInputAttribDesc.location = 1u;
	normalInputAttribDesc.binding = 0u;
	normalInputAttribDesc.format = VK_FORMAT_R32G32B32_SFLOAT;
	normalInputAttribDesc.offset = 12u;

	vertexInputAttribDescs[0] = positionInputAttribDesc; 
	vertexInputAttribDescs[1] = normalInputAttribDesc;

	vertexInputStateCreateInfo = vkstruct<VkPipelineVertexInputStateCreateInfo>();
	vertexInputStateCreateInfo.vertexBindingDescriptionCount = 1u;
	vertexInputStateCreateInfo.pVertexBindingDescriptions = &inputBindingDesc;
	vertexInputStateCreateInfo.vertexAttributeDescriptionCount = 2u;
	vertexInputStateCreateInfo.pVertexAttributeDescriptions = vertexInputAttribDescs;
}

void Forward::SetupInputAssembly()
{
	inputAssemblyStateCreateInfo = vkstruct<VkPipelineInputAssemblyStateCreateInfo>();
	inputAssemblyStateCreateInfo.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
	inputAssemblyStateCreateInfo.primitiveRestartEnable = VK_FALSE;
}

void Forward::SetupViewport()
{
	viewport.x = 0.f;
	viewport.y = 0.f;
	viewport.width = float{ WINDOW_WIDTH };
	viewport.height = float{ WINDOW_HEIGHT };
	viewport.minDepth = 0.f;
	viewport.maxDepth = 1.f;

	scissor.offset = { 0, 0 };
	scissor.extent = { u32{ WINDOW_WIDTH }, u32{ WINDOW_HEIGHT } };

	viewportStateCreateInfo = vkstruct<VkPipelineViewportStateCreateInfo>();
	viewportStateCreateInfo.viewportCount = 1u;
	viewportStateCreateInfo.pViewports = &viewport;
	viewportStateCreateInfo.scissorCount = 1u;
	viewportStateCreateInfo.pScissors = &scissor;
}

void Forward::SetupRasterization()
{
	rasterizationStateCreateInfo = vkstruct<VkPipelineRasterizationStateCreateInfo>();
	rasterizationStateCreateInfo.depthClampEnable = VK_FALSE;
	rasterizationStateCreateInfo.rasterizerDiscardEnable = VK_FALSE;
	rasterizationStateCreateInfo.polygonMode = VK_POLYGON_MODE_FILL;
	//rasterizationStateCreateInfo.polygonMode = VK_POLYGON_MODE_LINE;
	rasterizationStateCreateInfo.cullMode = VK_CULL_MODE_BACK_BIT;
	rasterizationStateCreateInfo.frontFace = VK_FRONT_FACE_CLOCKWISE;
	rasterizationStateCreateInfo.depthBiasEnable = VK_FALSE;
	rasterizationStateCreateInfo.depthBiasConstantFactor = 0.f;
	rasterizationStateCreateInfo.depthBiasClamp = 0.0f;
	rasterizationStateCreateInfo.depthBiasSlopeFactor = 0.f;
	rasterizationStateCreateInfo.lineWidth = 1.f;
}

void Forward::SetupMultisampling()
{
	multisampleStateCreateInfo = vkstruct<VkPipelineMultisampleStateCreateInfo>();
	multisampleStateCreateInfo.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
	multisampleStateCreateInfo.sampleShadingEnable = VK_FALSE;
	multisampleStateCreateInfo.minSampleShading = 0.f; // ignored here (no sampleRateShading)
	multisampleStateCreateInfo.pSampleMask = nullptr;
	multisampleStateCreateInfo.alphaToCoverageEnable = VK_FALSE;
	multisampleStateCreateInfo.alphaToOneEnable = VK_FALSE;
}

void Forward::SetupDepthStencil()
{
	depthstencilStateCreateInfo = vkstruct<VkPipelineDepthStencilStateCreateInfo>();
	depthstencilStateCreateInfo.depthTestEnable = VK_TRUE;
	depthstencilStateCreateInfo.depthWriteEnable = VK_TRUE;
	depthstencilStateCreateInfo.depthCompareOp = VK_COMPARE_OP_LESS;
	depthstencilStateCreateInfo.depthBoundsTestEnable = VK_FALSE;
	depthstencilStateCreateInfo.stencilTestEnable = VK_FALSE;
	depthstencilStateCreateInfo.minDepthBounds = 0.f;
	depthstencilStateCreateInfo.maxDepthBounds = 0.f;
}

void Forward::SetupColorBlend()
{
	VkColorComponentFlags colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
	singleColorBlendState = create_VkPipelineColorBlendAttachmentState_disabledBlend(colorWriteMask);

	colorblendStateCreateInfo = vkstruct<VkPipelineColorBlendStateCreateInfo>();
	colorblendStateCreateInfo.logicOpEnable = VK_FALSE;
	colorblendStateCreateInfo.attachmentCount = 1u;
	colorblendStateCreateInfo.pAttachments = &singleColorBlendState;
	// ignore blendConstants
}

void Forward::SetupRenderPass()
{
	VkAttachmentDescription attachementDescs[2];

	// Swap chain Render target attachment
	attachementDescs[0].flags = 0;
	attachementDescs[0].format = gSwapChain.Format();
	attachementDescs[0].samples = VK_SAMPLE_COUNT_1_BIT;
	attachementDescs[0].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	attachementDescs[0].storeOp = VK_ATTACHMENT_STORE_OP_STORE;
	attachementDescs[0].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	attachementDescs[0].stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	attachementDescs[0].initialLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL; // TODO check these
	attachementDescs[0].finalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL; // TODO check these

	// DepthStencil attachment
	attachementDescs[1].flags = 0;
	attachementDescs[1].format = VK_FORMAT_D32_SFLOAT;
	attachementDescs[1].samples = VK_SAMPLE_COUNT_1_BIT;
	attachementDescs[1].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	attachementDescs[1].storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	attachementDescs[1].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	attachementDescs[1].stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	attachementDescs[1].initialLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
	attachementDescs[1].finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

	VkAttachmentReference simpleOutColorAttachementReference;
	simpleOutColorAttachementReference.attachment = 0u;
	simpleOutColorAttachementReference.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

	VkAttachmentReference depthStencilAttachementReference;
	depthStencilAttachementReference.attachment = 1u;
	depthStencilAttachementReference.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

	auto subpassDesc = vkstruct<VkSubpassDescription>();
	subpassDesc.colorAttachmentCount = 1u;
	subpassDesc.pColorAttachments = &simpleOutColorAttachementReference;
	subpassDesc.pDepthStencilAttachment = &depthStencilAttachementReference;

	auto renderPassCreateInfo = vkstruct<VkRenderPassCreateInfo>();
	renderPassCreateInfo.attachmentCount = 2u;
	renderPassCreateInfo.pAttachments = attachementDescs;
	renderPassCreateInfo.subpassCount = 1u;
	renderPassCreateInfo.pSubpasses = &subpassDesc;

	renderPass = create_VkRenderPass(&renderPassCreateInfo);
}

void Forward::SetupPipelineLayout()
{
	auto pipelineLayoutCreateInfo = vkstruct<VkPipelineLayoutCreateInfo>();
	pipelineLayoutCreateInfo.setLayoutCount = GlobalDescriptorSets::CB_COUNT;
	pipelineLayoutCreateInfo.pSetLayouts = GlobalDescriptorSets::descriptorSetLayouts;

	gVkLastRes = vkCreatePipelineLayout(gDevice.VkHandle(), &pipelineLayoutCreateInfo, nullptr, &pipelineLayout);
	VKFN_LAST_RES_SUCCESS_OR_QUIT(vkCreatePipelineLayout);
}

void Forward::SetupGraphicsPipeline()
{
	auto pipelineCreateInfo = vkstruct<VkGraphicsPipelineCreateInfo>();
	pipelineCreateInfo.flags = 0;
	pipelineCreateInfo.stageCount = 2u;
	pipelineCreateInfo.pStages = shaderStagesCreateInfo;
	pipelineCreateInfo.pVertexInputState = &vertexInputStateCreateInfo;
	pipelineCreateInfo.pInputAssemblyState = &inputAssemblyStateCreateInfo;
	pipelineCreateInfo.pTessellationState = nullptr;
	pipelineCreateInfo.pViewportState = &viewportStateCreateInfo;
	pipelineCreateInfo.pRasterizationState = &rasterizationStateCreateInfo;
	pipelineCreateInfo.pMultisampleState = &multisampleStateCreateInfo;
	pipelineCreateInfo.pDepthStencilState = &depthstencilStateCreateInfo;
	pipelineCreateInfo.pColorBlendState = &colorblendStateCreateInfo;
	pipelineCreateInfo.pDynamicState = nullptr; // No dynamic state for this simple pipeline
	pipelineCreateInfo.layout = pipelineLayout;
	pipelineCreateInfo.renderPass = renderPass;
	pipelineCreateInfo.subpass = 0u;

	gVkLastRes = vkCreateGraphicsPipelines(gDevice.VkHandle(), VK_NULL_HANDLE, 1u, &pipelineCreateInfo, nullptr, &pipeline);
	VKFN_LAST_RES_SUCCESS_OR_QUIT(vkCreateGraphicsPipelines);
}

void Forward::SetupRenderTargets()
{
	aIsFirstSwapImageUse = std::vector<bool>(gSwapChain.ImageCount(), true);

	// CREATE DEPTH STENTIL IMAGE AND VIEW

	u32 depthStencilImageQueueFamilyIndex = 0;

	auto depthStencilImageCreateInfo = vkstruct<VkImageCreateInfo>();
	depthStencilImageCreateInfo.flags = 0;
	depthStencilImageCreateInfo.imageType = VK_IMAGE_TYPE_2D;
	depthStencilImageCreateInfo.format = VK_FORMAT_D32_SFLOAT;
	depthStencilImageCreateInfo.extent = { WINDOW_WIDTH, WINDOW_HEIGHT, 1 };
	depthStencilImageCreateInfo.mipLevels = 1u;
	depthStencilImageCreateInfo.arrayLayers = 1u;
	depthStencilImageCreateInfo.samples = VK_SAMPLE_COUNT_1_BIT;
	depthStencilImageCreateInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
	depthStencilImageCreateInfo.usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
	depthStencilImageCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	depthStencilImageCreateInfo.queueFamilyIndexCount = 1u;
	depthStencilImageCreateInfo.pQueueFamilyIndices = &depthStencilImageQueueFamilyIndex;
	depthStencilImageCreateInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;

	gVkLastRes = vkCreateImage(gDevice.VkHandle(), &depthStencilImageCreateInfo, nullptr, &gDepthStencilImage);
	VKFN_LAST_RES_SUCCESS_OR_QUIT(vkCreateImage);

	allocateNewImageMemory(&gDepthStencilImage, &gDepthStencilImageMemory, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

	gVkLastRes = vkBindImageMemory(gDevice.VkHandle(), gDepthStencilImage, gDepthStencilImageMemory, 0);
	VKFN_LAST_RES_SUCCESS_OR_QUIT(vkBindImageMemory);

	VkImageSubresourceRange depthStencilImageSubresourceRange;
	depthStencilImageSubresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
	depthStencilImageSubresourceRange.baseMipLevel = 0;
	depthStencilImageSubresourceRange.levelCount = 1;
	depthStencilImageSubresourceRange.baseArrayLayer = 0;
	depthStencilImageSubresourceRange.layerCount = 1;

	VkImageViewCreateInfo depthStencilViewCreateInfo = vkstruct<VkImageViewCreateInfo>();
	depthStencilViewCreateInfo.image = gDepthStencilImage;
	depthStencilViewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
	depthStencilViewCreateInfo.format = VK_FORMAT_D32_SFLOAT;
	depthStencilViewCreateInfo.components = identityComponentMapping;
	depthStencilViewCreateInfo.subresourceRange = depthStencilImageSubresourceRange;

	gVkLastRes = vkCreateImageView(gDevice.VkHandle(), &depthStencilViewCreateInfo, nullptr, &gDepthStencilView);
	VKFN_LAST_RES_SUCCESS_OR_QUIT(vkCreateImageView);

	isFirstDepthStencilImageUse = true;
}

void Forward::SetupFrameRenderResources()
{
	imageAcquiredSemaphore = create_VkSemaphore();
	renderingCompleteSemaphore = create_VkSemaphore();
}

void Forward::CleanupSetupOnlyResources()
{
	// From this point, the shader modules are consumed by the API and no longer needed
	vkDestroyShaderModule(gDevice.VkHandle(), vertexModule, nullptr);
	vkDestroyShaderModule(gDevice.VkHandle(), fragmentModule, nullptr);
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

void Forward::SubmitFrameRender(const Scene& scene, VkQueue graphicQueue, VkQueue presentQueue)
{
	/* Render here */
	u32 currentSwapImageIndex = UINT32_MAX;
	gVkLastRes = vkAcquireNextImageKHR(gDevice.VkHandle(), gSwapChain.VkHandle(), UINT64_MAX, imageAcquiredSemaphore, VK_NULL_HANDLE, &currentSwapImageIndex);
	VKFN_LAST_RES_SUCCESS_OR_QUIT(vkAcquireNextImageKHR);

	//////////////////////////////////////////////////////////////////

	// FRAMEBUFFER

	VkImageView framebufferAttachmentsViews[2] = { gSwapChain.Views()[currentSwapImageIndex], gDepthStencilView };

	auto framebufferCreateInfo = vkstruct<VkFramebufferCreateInfo>();
	framebufferCreateInfo.flags = 0;
	framebufferCreateInfo.renderPass = renderPass;
	framebufferCreateInfo.attachmentCount = 2u;
	framebufferCreateInfo.pAttachments = framebufferAttachmentsViews;
	framebufferCreateInfo.width = static_cast<u32>(WINDOW_WIDTH);
	framebufferCreateInfo.height = static_cast<u32>(WINDOW_HEIGHT);
	framebufferCreateInfo.layers = 1u;

	gVkLastRes = vkCreateFramebuffer(gDevice.VkHandle(), &framebufferCreateInfo, nullptr, &framebuffer);
	VKFN_LAST_RES_SUCCESS_OR_QUIT(vkCreateFramebuffer);

	// COMMAND BUFFER (POOL)

	auto cmdPoolCreateInfo = vkstruct<VkCommandPoolCreateInfo>();
	// TODO create diverse pools with optimal support for either pure static cmd buffers or dynamic ones
	cmdPoolCreateInfo.flags = VK_COMMAND_POOL_CREATE_TRANSIENT_BIT | VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
	cmdPoolCreateInfo.queueFamilyIndex = 0u;

	gVkLastRes = vkCreateCommandPool(gDevice.VkHandle(), &cmdPoolCreateInfo, nullptr, &cmdPool);
	VKFN_LAST_RES_SUCCESS_OR_QUIT(vkCreateCommandPool);

	// COMMAND BUFFER

	auto drawTriangleCmdBufferAllocInfo = vkstruct<VkCommandBufferAllocateInfo>();
	drawTriangleCmdBufferAllocInfo.commandPool = cmdPool;
	drawTriangleCmdBufferAllocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	drawTriangleCmdBufferAllocInfo.commandBufferCount = 1u;

	VkCommandBuffer drawTriangleCmdBuffer;
	gVkLastRes = vkAllocateCommandBuffers(gDevice.VkHandle(), &drawTriangleCmdBufferAllocInfo, &drawTriangleCmdBuffer);
	VKFN_LAST_RES_SUCCESS_OR_QUIT(vkAllocateCommandBuffers);

	auto drawTriangleCmdBufferBeginInfo = vkstruct<VkCommandBufferBeginInfo>();
	drawTriangleCmdBufferBeginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT; // TODO learn more about flags in case of a more elaborate use

	// RENDER PASS LAUNCH

	VkClearValue clearColorValue;
	clearColorValue.color = { 0.f, 0.f, 0.f, 1.f };

	VkClearValue clearDepthStencilValue;
	clearDepthStencilValue.depthStencil = { 1.f, 0 };

	VkClearValue clearValues[2] = { clearColorValue , clearDepthStencilValue };

	auto renderPassBeginInfo = vkstruct<VkRenderPassBeginInfo>();
	renderPassBeginInfo.renderPass = renderPass;
	renderPassBeginInfo.framebuffer = framebuffer;
	renderPassBeginInfo.renderArea = { { 0, 0 },{ WINDOW_WIDTH, WINDOW_HEIGHT } };
	renderPassBeginInfo.clearValueCount = 2u;
	renderPassBeginInfo.pClearValues = clearValues;

	// REGISTER COMMAND BUFFER

	gVkLastRes = vkBeginCommandBuffer(drawTriangleCmdBuffer, &drawTriangleCmdBufferBeginInfo);
	VKFN_LAST_RES_SUCCESS_OR_QUIT(vkBeginCommandBuffer);

	if (isFirstDepthStencilImageUse) {
		cmdTransitionSimpleImageFromInitialState(drawTriangleCmdBuffer, gDepthStencilImage, VK_IMAGE_ASPECT_DEPTH_BIT,
			VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
			VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT, VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT);
	}

	// TRANSITION RENDERTARGET FROM PRESENTABLE TO RENDERABLE LAYOUT + Initial transition from UNDEFINED layout

	VkImageSubresourceRange acquireImageSubresourceRange;
	acquireImageSubresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	acquireImageSubresourceRange.baseMipLevel = 0u;
	acquireImageSubresourceRange.levelCount = 1u;
	acquireImageSubresourceRange.baseArrayLayer = 0u;
	acquireImageSubresourceRange.layerCount = 1u;

	auto acquireImageBarrier = vkstruct<VkImageMemoryBarrier>();
	acquireImageBarrier.srcAccessMask = aIsFirstSwapImageUse[currentSwapImageIndex] ? 0 : VK_ACCESS_MEMORY_READ_BIT;
	acquireImageBarrier.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
	acquireImageBarrier.oldLayout = aIsFirstSwapImageUse[currentSwapImageIndex] ? VK_IMAGE_LAYOUT_UNDEFINED : VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
	acquireImageBarrier.newLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
	acquireImageBarrier.image = gSwapChain.Images()[currentSwapImageIndex];
	acquireImageBarrier.subresourceRange = acquireImageSubresourceRange;

	vkCmdPipelineBarrier(drawTriangleCmdBuffer, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
		0, 0u, nullptr, 0u, nullptr, 1u, &acquireImageBarrier);


	/* RECORD DRAW CALL CMDS HERE */
	vkCmdBeginRenderPass(drawTriangleCmdBuffer, &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);

	vkCmdBindPipeline(drawTriangleCmdBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline);

	vkCmdBindDescriptorSets(drawTriangleCmdBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, 
		0u, GlobalDescriptorSets::CB_COUNT, GlobalDescriptorSets::descriptorSets, 0u, nullptr);

	vkCmdBindIndexBuffer(drawTriangleCmdBuffer, scene.indexBuffer.m_buffer, 0u, VK_INDEX_TYPE_UINT32);

	VkDeviceSize vertexBufferOffsetArray[] = { 0 };
	vkCmdBindVertexBuffers(drawTriangleCmdBuffer, 0u, 1u, &(scene.vertexBuffer.m_buffer), vertexBufferOffsetArray);

	vkCmdDrawIndexed(drawTriangleCmdBuffer, scene.modelIndexCount, scene.modelInstanceCount, 0u, 0u, 0u);

	vkCmdEndRenderPass(drawTriangleCmdBuffer);


	// TRANSITION RENDERTARGET FROM RENDERABLE TO PRESENTABLE LAYOUT

	VkImageSubresourceRange presentImageSubresourceRange;
	presentImageSubresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	presentImageSubresourceRange.baseMipLevel = 0u;
	presentImageSubresourceRange.levelCount = 1u;
	presentImageSubresourceRange.baseArrayLayer = 0u;
	presentImageSubresourceRange.layerCount = 1u;

	auto presentImageBarrier = vkstruct<VkImageMemoryBarrier>();
	presentImageBarrier.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
	presentImageBarrier.dstAccessMask = VK_ACCESS_MEMORY_READ_BIT;
	presentImageBarrier.oldLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
	presentImageBarrier.newLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
	presentImageBarrier.image = gSwapChain.Images()[currentSwapImageIndex];
	presentImageBarrier.subresourceRange = presentImageSubresourceRange;

	vkCmdPipelineBarrier(drawTriangleCmdBuffer, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT,
		0, 0u, nullptr, 0u, nullptr, 1u, &presentImageBarrier);


	gVkLastRes = vkEndCommandBuffer(drawTriangleCmdBuffer);
	VKFN_LAST_RES_SUCCESS_OR_QUIT(vkEndCommandBuffer);

	//////////////////////////////////////////////////////////////////

	VkPipelineStageFlags waitSwapchainAcquiredAtStage[1u] = { VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT };

	auto submitInfo = vkstruct<VkSubmitInfo>();
	submitInfo.waitSemaphoreCount = 1u;
	submitInfo.pWaitSemaphores = &imageAcquiredSemaphore;
	submitInfo.pWaitDstStageMask = waitSwapchainAcquiredAtStage;
	submitInfo.commandBufferCount = 1u;
	submitInfo.pCommandBuffers = &drawTriangleCmdBuffer;
	submitInfo.signalSemaphoreCount = 1u;
	submitInfo.pSignalSemaphores = &renderingCompleteSemaphore;

	gVkLastRes = vkQueueSubmit(graphicQueue, 1u, &submitInfo, VK_NULL_HANDLE);
	VKFN_LAST_RES_SUCCESS_OR_QUIT(vkQueueSubmit);

	// Submit present operation to present queue
	auto presentInfo = vkstruct<VkPresentInfoKHR>();
	presentInfo.waitSemaphoreCount = 1u;
	presentInfo.pWaitSemaphores = &renderingCompleteSemaphore;
	presentInfo.swapchainCount = 1u;
	presentInfo.pSwapchains = gSwapChain.VkHandleAddress();
	presentInfo.pImageIndices = &currentSwapImageIndex;
	presentInfo.pResults = nullptr;


	gVkLastRes = vkQueuePresentKHR(presentQueue, &presentInfo);
	VKFN_LAST_RES_SUCCESS_OR_QUIT(vkQueuePresentKHR);

	aIsFirstSwapImageUse[currentSwapImageIndex] = false;
	isFirstDepthStencilImageUse = false;
}

void Forward::CleanupAfterFrame()
{
	vkDestroyCommandPool(gDevice.VkHandle(), cmdPool, nullptr);
	vkDestroyFramebuffer(gDevice.VkHandle(), framebuffer, nullptr);
}

void Forward::Destroy()
{
	vkDestroySemaphore(gDevice.VkHandle(), imageAcquiredSemaphore, nullptr);
	vkDestroySemaphore(gDevice.VkHandle(), renderingCompleteSemaphore, nullptr);

	vkFreeMemory(gDevice.VkHandle(), gDepthStencilImageMemory, nullptr);
	vkDestroyImageView(gDevice.VkHandle(), gDepthStencilView, nullptr);
	vkDestroyImage(gDevice.VkHandle(), gDepthStencilImage, nullptr);

	vkDestroyPipelineLayout(gDevice.VkHandle(), pipelineLayout, nullptr);

	vkDestroyRenderPass(gDevice.VkHandle(), renderPass, nullptr);

	vkDestroyPipeline(gDevice.VkHandle(), pipeline, nullptr);
}
