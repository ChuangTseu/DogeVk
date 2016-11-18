#include "forward.h"

#include <array>

#include <glm/gtc/type_ptr.hpp>

#include "vulkan_helpers.h"

#include "config.h"

#include "swapchain.h"
#include "dedicated_buffer.h"
#include "memory.h"
#include "global_descriptor_sets.h"

#include "shadow.h"

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

	mRenderPass = create_VkRenderPass(&renderPassCreateInfo);
}

void Forward::PrepareShadowMapsDescriptorSet()
{
	VkSamplerCreateInfo shadowMapSamplerCreateInfo;
	shadowMapSamplerCreateInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
	shadowMapSamplerCreateInfo.pNext = nullptr;
	shadowMapSamplerCreateInfo.flags = 0; // reserved for future use
	shadowMapSamplerCreateInfo.magFilter = VK_FILTER_NEAREST;
	shadowMapSamplerCreateInfo.minFilter = VK_FILTER_NEAREST;
	shadowMapSamplerCreateInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_NEAREST;
	shadowMapSamplerCreateInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER;
	shadowMapSamplerCreateInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER;
	shadowMapSamplerCreateInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER;
	shadowMapSamplerCreateInfo.mipLodBias = 0.f;
	shadowMapSamplerCreateInfo.anisotropyEnable = VK_FALSE;
	shadowMapSamplerCreateInfo.maxAnisotropy = 0.f;
	shadowMapSamplerCreateInfo.compareEnable = VK_TRUE;
	shadowMapSamplerCreateInfo.compareOp = VK_COMPARE_OP_LESS_OR_EQUAL;
	shadowMapSamplerCreateInfo.minLod = 0.f;
	shadowMapSamplerCreateInfo.maxLod = 0.f;
	shadowMapSamplerCreateInfo.borderColor = VK_BORDER_COLOR_FLOAT_OPAQUE_BLACK;
	shadowMapSamplerCreateInfo.unnormalizedCoordinates = VK_FALSE;

	gVkLastRes = vkCreateSampler(gDevice.VkHandle(), &shadowMapSamplerCreateInfo, nullptr, &shadowMapSampler);
	VKFN_LAST_RES_SUCCESS_OR_QUIT(vkCreateSampler);

	std::array<VkSampler, MAX_SHADER_LIGHTS> immutableSamplers;
	immutableSamplers.fill(shadowMapSampler);

	VkDescriptorSetLayoutBinding descripterSetLayoutBinding;
	descripterSetLayoutBinding.binding = 0u;
	descripterSetLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	descripterSetLayoutBinding.descriptorCount = MAX_SHADER_LIGHTS;
	descripterSetLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
	descripterSetLayoutBinding.pImmutableSamplers = immutableSamplers.data();

	// TMP
	//descripterSetLayoutBinding.descriptorCount = 1u;
	//descripterSetLayoutBinding.pImmutableSamplers = &shadowMapSampler;

	//descripterSetLayoutBinding.pImmutableSamplers = nullptr;
	// TMP

	auto descriptorSetLayoutCreateInfo = vkstruct<VkDescriptorSetLayoutCreateInfo>();
	descriptorSetLayoutCreateInfo.bindingCount = 1u;
	descriptorSetLayoutCreateInfo.pBindings = &descripterSetLayoutBinding;

	gVkLastRes = vkCreateDescriptorSetLayout(gDevice.VkHandle(), &descriptorSetLayoutCreateInfo, nullptr, &shadowMapsDescriptorSetLayout);
	VKFN_LAST_RES_SUCCESS_OR_QUIT(vkCreateDescriptorSetLayout);

	VkDescriptorPoolSize descriptorPoolSize;
	descriptorPoolSize.type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	descriptorPoolSize.descriptorCount = MAX_SHADER_LIGHTS;

	// TMP
	//descriptorPoolSize.descriptorCount = 1u;
	// TMP

	auto descriptorPoolCreateInfo = vkstruct<VkDescriptorPoolCreateInfo>();
	descriptorPoolCreateInfo.flags = 0;
	descriptorPoolCreateInfo.maxSets = 1u;
	descriptorPoolCreateInfo.poolSizeCount = 1u;
	descriptorPoolCreateInfo.pPoolSizes = &descriptorPoolSize;

	gVkLastRes = vkCreateDescriptorPool(gDevice.VkHandle(), &descriptorPoolCreateInfo, nullptr, &shadowMapsDescriptorPool);
	VKFN_LAST_RES_SUCCESS_OR_QUIT(vkCreateDescriptorPool);

	auto descriptorSetAllocateInfo = vkstruct<VkDescriptorSetAllocateInfo>();
	descriptorSetAllocateInfo.descriptorPool = shadowMapsDescriptorPool;
	descriptorSetAllocateInfo.descriptorSetCount = 1u;
	descriptorSetAllocateInfo.pSetLayouts = &shadowMapsDescriptorSetLayout;

	gVkLastRes = vkAllocateDescriptorSets(gDevice.VkHandle(), &descriptorSetAllocateInfo, &shadowMapsDescriptorSet);
	VKFN_LAST_RES_SUCCESS_OR_QUIT(vkAllocateDescriptorSets);

	VkDescriptorImageInfo descriptorImageInfo;
	descriptorImageInfo.sampler = VK_NULL_HANDLE; // Nothing ! Using immutable sampler
	descriptorImageInfo.imageView = gShadow.mDepthImage.view;
	descriptorImageInfo.imageLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL;

	auto descriptorWrite = vkstruct<VkWriteDescriptorSet>();
	descriptorWrite.dstSet = shadowMapsDescriptorSet;
	descriptorWrite.dstBinding = 0u;
	descriptorWrite.dstArrayElement = 0u;
	descriptorWrite.descriptorCount = 1u;
	descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	descriptorWrite.pImageInfo = &descriptorImageInfo;

	vkUpdateDescriptorSets(gDevice.VkHandle(), 1u, &descriptorWrite, 0u, nullptr);
}

void Forward::SetupPipelineLayout()
{
	VkPushConstantRange vertexPushConstantRange;
	vertexPushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
	vertexPushConstantRange.offset = 0u;
	vertexPushConstantRange.size = 64u; // mat4

	// Put custom descriptor set layout after the GlobalDescriptorSets base
	VkDescriptorSetLayout descriptorSetLayouts[GlobalDescriptorSets::CB_COUNT + 1];
	std::copy_n(GlobalDescriptorSets::descriptorSetLayouts, GlobalDescriptorSets::CB_COUNT, descriptorSetLayouts);
	descriptorSetLayouts[GlobalDescriptorSets::CB_COUNT] = shadowMapsDescriptorSetLayout;

	auto pipelineLayoutCreateInfo = vkstruct<VkPipelineLayoutCreateInfo>();
	pipelineLayoutCreateInfo.setLayoutCount = len32(descriptorSetLayouts);
	pipelineLayoutCreateInfo.pSetLayouts = descriptorSetLayouts;
	pipelineLayoutCreateInfo.pushConstantRangeCount = 1u;
	pipelineLayoutCreateInfo.pPushConstantRanges = &vertexPushConstantRange;

	gVkLastRes = vkCreatePipelineLayout(gDevice.VkHandle(), &pipelineLayoutCreateInfo, nullptr, &mPipelineLayout);
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
	pipelineCreateInfo.layout = mPipelineLayout;
	pipelineCreateInfo.renderPass = mRenderPass;
	pipelineCreateInfo.subpass = 0u;

	gVkLastRes = vkCreateGraphicsPipelines(gDevice.VkHandle(), VK_NULL_HANDLE, 1u, &pipelineCreateInfo, nullptr, &mPipeline);
	VKFN_LAST_RES_SUCCESS_OR_QUIT(vkCreateGraphicsPipelines);
}

void Forward::SetupRenderTargets()
{
	mDepthImages.resize(gSwapChain.ImageCount());

	for (u32 i = 0; i < gSwapChain.ImageCount(); ++i) {
		CreateDepthImageAndView(&mDepthImages[i].image, &mDepthImages[i].view, &mDepthImages[i].memory, WINDOW_WIDTH, WINDOW_HEIGHT, 0u, false);
	}
}

void Forward::SetupFrameRenderResources()
{
	imageAcquiredSemaphore = create_VkSemaphore();
	renderingCompleteSemaphore = create_VkSemaphore();
}

void Forward::TransitionInitiallyUndefinedImages()
{
	// WARNING ! No transitioning is done currently !
	// Since I use a Nvidia card, this is not a problem for me as it is totally ignored.
	// Will do once the 3D rendering part is cleanly separated from the present part.
}

void Forward::CleanupSetupOnlyResources()
{
	// From this point, the shader modules are consumed by the API and no longer needed
	vkDestroyShaderModule(gDevice.VkHandle(), vertexModule, nullptr);
	vkDestroyShaderModule(gDevice.VkHandle(), fragmentModule, nullptr);
}

void Forward::SetupCmdBufferPool()
{
	// COMMAND BUFFER (POOL)

	auto cmdPoolCreateInfo = vkstruct<VkCommandPoolCreateInfo>();
	cmdPoolCreateInfo.flags = 0;
	cmdPoolCreateInfo.queueFamilyIndex = 0u;

	gVkLastRes = vkCreateCommandPool(gDevice.VkHandle(), &cmdPoolCreateInfo, nullptr, &mCmdPool);
	VKFN_LAST_RES_SUCCESS_OR_QUIT(vkCreateCommandPool);
}

void Forward::CookSpawChainCmdBuffers(const Scene& scene)
{
	// COMMAND BUFFER
	mDrawTriangleCmdBuffers.resize(gSwapChain.ImageCount());
	mFramebuffers.resize(gSwapChain.ImageCount());

	auto drawTriangleCmdBufferAllocInfo = vkstruct<VkCommandBufferAllocateInfo>();
	drawTriangleCmdBufferAllocInfo.commandPool = mCmdPool;
	drawTriangleCmdBufferAllocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	drawTriangleCmdBufferAllocInfo.commandBufferCount = gSwapChain.ImageCount();

	gVkLastRes = vkAllocateCommandBuffers(gDevice.VkHandle(), &drawTriangleCmdBufferAllocInfo, mDrawTriangleCmdBuffers.data());
	VKFN_LAST_RES_SUCCESS_OR_QUIT(vkAllocateCommandBuffers);

	VkClearValue clearColorValue;
	clearColorValue.color = { 0.f, 0.f, 0.f, 1.f };
	VkClearValue clearDepthStencilValue;
	clearDepthStencilValue.depthStencil = { 1.f, 0 };

	VkClearValue clearValues[2] = { clearColorValue , clearDepthStencilValue };

	auto renderPassBeginInfo = vkstruct<VkRenderPassBeginInfo>();
	renderPassBeginInfo.renderPass = mRenderPass;
	renderPassBeginInfo.renderArea = { { 0, 0 },{ WINDOW_WIDTH, WINDOW_HEIGHT } };
	renderPassBeginInfo.clearValueCount = 2u;
	renderPassBeginInfo.pClearValues = clearValues;

	auto framebufferCreateInfo = vkstruct<VkFramebufferCreateInfo>();
	framebufferCreateInfo.flags = 0;
	framebufferCreateInfo.renderPass = mRenderPass;
	framebufferCreateInfo.attachmentCount = 2u;
	framebufferCreateInfo.width = static_cast<u32>(WINDOW_WIDTH);
	framebufferCreateInfo.height = static_cast<u32>(WINDOW_HEIGHT);
	framebufferCreateInfo.layers = 1u;

	for (u32 i = 0; i < gSwapChain.ImageCount(); ++i) {
		VkImageView framebufferAttachmentsViews[2] = { gSwapChain.Views()[i], mDepthImages[i].view };
		framebufferCreateInfo.pAttachments = framebufferAttachmentsViews;

		gVkLastRes = vkCreateFramebuffer(gDevice.VkHandle(), &framebufferCreateInfo, nullptr, &mFramebuffers[i]);
		VKFN_LAST_RES_SUCCESS_OR_QUIT(vkCreateFramebuffer);

		renderPassBeginInfo.framebuffer = mFramebuffers[i];

		auto drawTriangleCmdBufferBeginInfo = vkstruct<VkCommandBufferBeginInfo>();
		drawTriangleCmdBufferBeginInfo.flags = 0;

		// REGISTER COMMAND BUFFER

		gVkLastRes = vkBeginCommandBuffer(mDrawTriangleCmdBuffers[i], &drawTriangleCmdBufferBeginInfo);
		VKFN_LAST_RES_SUCCESS_OR_QUIT(vkBeginCommandBuffer);

		// TRANSITION RENDERTARGET FROM PRESENTABLE TO RENDERABLE LAYOUT + Initial transition from UNDEFINED layout

		VkImageSubresourceRange acquireImageSubresourceRange;
		acquireImageSubresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		acquireImageSubresourceRange.baseMipLevel = 0u;
		acquireImageSubresourceRange.levelCount = 1u;
		acquireImageSubresourceRange.baseArrayLayer = 0u;
		acquireImageSubresourceRange.layerCount = 1u;

		auto acquireImageBarrier = vkstruct<VkImageMemoryBarrier>();
		acquireImageBarrier.srcAccessMask = VK_ACCESS_MEMORY_READ_BIT;
		acquireImageBarrier.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
		acquireImageBarrier.oldLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
		acquireImageBarrier.newLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
		acquireImageBarrier.image = gSwapChain.Images()[i];
		acquireImageBarrier.subresourceRange = acquireImageSubresourceRange;

		vkCmdPipelineBarrier(mDrawTriangleCmdBuffers[i], VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
			0, 0u, nullptr, 0u, nullptr, 1u, &acquireImageBarrier);

		vkCmdBeginRenderPass(mDrawTriangleCmdBuffers[i], &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);

		vkCmdBindPipeline(mDrawTriangleCmdBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, mPipeline);

		// Put custom descriptor set after the GlobalDescriptorSets base
		VkDescriptorSet descriptorSets[GlobalDescriptorSets::CB_COUNT + 1];
		std::copy_n(GlobalDescriptorSets::descriptorSets, GlobalDescriptorSets::CB_COUNT, descriptorSets);
		descriptorSets[GlobalDescriptorSets::CB_COUNT] = shadowMapsDescriptorSet;

		vkCmdBindDescriptorSets(mDrawTriangleCmdBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, mPipelineLayout,
			0u, len32(descriptorSets), descriptorSets, 0u, nullptr);

		scene.BindDrawingToCmdBuffer(mDrawTriangleCmdBuffers[i]);

		vkCmdEndRenderPass(mDrawTriangleCmdBuffers[i]);


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
		presentImageBarrier.image = gSwapChain.Images()[i];
		presentImageBarrier.subresourceRange = presentImageSubresourceRange;

		vkCmdPipelineBarrier(mDrawTriangleCmdBuffers[i], VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT,
			0, 0u, nullptr, 0u, nullptr, 1u, &presentImageBarrier);


		gVkLastRes = vkEndCommandBuffer(mDrawTriangleCmdBuffers[i]);
		VKFN_LAST_RES_SUCCESS_OR_QUIT(vkEndCommandBuffer);
	}
}

void Forward::SubmitFrameRender(VkQueue graphicQueue, VkQueue presentQueue)
{
	u32 currentSwapImageIndex = UINT32_MAX;
	gVkLastRes = vkAcquireNextImageKHR(gDevice.VkHandle(), gSwapChain.VkHandle(), UINT64_MAX, imageAcquiredSemaphore, VK_NULL_HANDLE, &currentSwapImageIndex);
	VKFN_LAST_RES_SUCCESS_OR_QUIT(vkAcquireNextImageKHR);

	VkSemaphore waitSemaphores[] = { imageAcquiredSemaphore, gShadow.shadowMapCompleteSemaphore };
	VkPipelineStageFlags waitForStages[] = { VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT };

	auto submitInfo = vkstruct<VkSubmitInfo>();
	submitInfo.waitSemaphoreCount = 2u;
	submitInfo.pWaitSemaphores = waitSemaphores;
	submitInfo.pWaitDstStageMask = waitForStages;
	submitInfo.commandBufferCount = 1u;
	submitInfo.pCommandBuffers = &mDrawTriangleCmdBuffers[currentSwapImageIndex];
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
}

void Forward::Destroy()
{
	vkDestroySemaphore(gDevice.VkHandle(), imageAcquiredSemaphore, nullptr);
	vkDestroySemaphore(gDevice.VkHandle(), renderingCompleteSemaphore, nullptr);

	for (Image& image : mDepthImages) image.Destroy();

	vkDestroyPipelineLayout(gDevice.VkHandle(), mPipelineLayout, nullptr);

	vkDestroyRenderPass(gDevice.VkHandle(), mRenderPass, nullptr);

	vkDestroyPipeline(gDevice.VkHandle(), mPipeline, nullptr);
}
