#include "shadow.h"

#include "vulkan_helpers.h"

#include "config.h"

#include "memory.h"
#include "global_descriptor_sets.h"
#include "vertex.h"

#include <glm/gtc/type_ptr.hpp>

Shadow gShadow;

void Shadow::SetupShaderStages()
{
	vertexModule = create_VkShaderModule_fromFile(SHADERS_FOLDER_PATH "shadow_vert.spv");

	auto vertexStageCreateInfo = vkstruct<VkPipelineShaderStageCreateInfo>();
	vertexStageCreateInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
	vertexStageCreateInfo.module = vertexModule;
	vertexStageCreateInfo.pName = "main";

	shaderStageCreateInfo = vertexStageCreateInfo;
};

void Shadow::SetupVertexInput()
{
	vertexInputStateCreateInfo = Vertex::GetPipelineVertexInputStateCreateInfo();
}

void Shadow::SetupInputAssembly()
{
	inputAssemblyStateCreateInfo = vkstruct<VkPipelineInputAssemblyStateCreateInfo>();
	inputAssemblyStateCreateInfo.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
	inputAssemblyStateCreateInfo.primitiveRestartEnable = VK_FALSE;
}

void Shadow::SetupViewport()
{
	viewport.x = 0.f;
	viewport.y = 0.f;
	viewport.width = float{ SHADOWMAP_RES };
	viewport.height = float{ SHADOWMAP_RES };
	viewport.minDepth = 0.f;
	viewport.maxDepth = 1.f;

	scissor.offset = { 0, 0 };
	scissor.extent = { u32{ SHADOWMAP_RES }, u32{ SHADOWMAP_RES } };

	viewportStateCreateInfo = vkstruct<VkPipelineViewportStateCreateInfo>();
	viewportStateCreateInfo.viewportCount = 1u;
	viewportStateCreateInfo.pViewports = &viewport;
	viewportStateCreateInfo.scissorCount = 1u;
	viewportStateCreateInfo.pScissors = &scissor;
}

void Shadow::SetupRasterization()
{
	rasterizationStateCreateInfo = vkstruct<VkPipelineRasterizationStateCreateInfo>();
	rasterizationStateCreateInfo.depthClampEnable = VK_FALSE;
	rasterizationStateCreateInfo.rasterizerDiscardEnable = VK_FALSE;
	rasterizationStateCreateInfo.polygonMode = VK_POLYGON_MODE_FILL;
	rasterizationStateCreateInfo.cullMode = VK_CULL_MODE_BACK_BIT;
	rasterizationStateCreateInfo.frontFace = VK_FRONT_FACE_CLOCKWISE;
	rasterizationStateCreateInfo.depthBiasEnable = VK_FALSE;
	rasterizationStateCreateInfo.depthBiasConstantFactor = 0.f;
	rasterizationStateCreateInfo.depthBiasClamp = 0.0f;
	rasterizationStateCreateInfo.depthBiasSlopeFactor = 0.f;
	rasterizationStateCreateInfo.lineWidth = 1.f;

	// TMP
	//rasterizationStateCreateInfo.depthBiasEnable = VK_TRUE;
	//rasterizationStateCreateInfo.depthBiasConstantFactor = 50.f;
	//rasterizationStateCreateInfo.depthBiasClamp = 1.f;
	//rasterizationStateCreateInfo.depthBiasSlopeFactor = 5.f;
	// TMP
}

void Shadow::SetupMultisampling()
{
	multisampleStateCreateInfo = vkstruct<VkPipelineMultisampleStateCreateInfo>();
	multisampleStateCreateInfo.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
	multisampleStateCreateInfo.sampleShadingEnable = VK_FALSE;
	multisampleStateCreateInfo.minSampleShading = 0.f; // ignored here (no sampleRateShading)
	multisampleStateCreateInfo.pSampleMask = nullptr;
	multisampleStateCreateInfo.alphaToCoverageEnable = VK_FALSE;
	multisampleStateCreateInfo.alphaToOneEnable = VK_FALSE;
}

void Shadow::SetupDepthStencil()
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

void Shadow::SetupColorBlend()
{
	//VkColorComponentFlags colorWriteMask = 0; // No color output
	//noColorBlendState = create_VkPipelineColorBlendAttachmentState_disabledBlend(colorWriteMask);

	//colorblendStateCreateInfo = vkstruct<VkPipelineColorBlendStateCreateInfo>();
	//colorblendStateCreateInfo.logicOpEnable = VK_FALSE;
	//colorblendStateCreateInfo.attachmentCount = 1u;
	//colorblendStateCreateInfo.pAttachments = &noColorBlendState;
	// ignore blendConstants
}

void Shadow::SetupRenderPass()
{
	// DepthStencil attachment
	VkAttachmentDescription attachementDesc;
	attachementDesc.flags = 0;
	attachementDesc.format = VK_FORMAT_D32_SFLOAT;
	attachementDesc.samples = VK_SAMPLE_COUNT_1_BIT;
	attachementDesc.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	attachementDesc.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	attachementDesc.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	attachementDesc.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	attachementDesc.initialLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
	attachementDesc.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

	VkAttachmentReference depthStencilAttachementReference;
	depthStencilAttachementReference.attachment = 0u;
	depthStencilAttachementReference.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

	auto subpassDesc = vkstruct<VkSubpassDescription>();
	subpassDesc.colorAttachmentCount = 0u;
	subpassDesc.pColorAttachments = nullptr;
	subpassDesc.pDepthStencilAttachment = &depthStencilAttachementReference;

	auto renderPassCreateInfo = vkstruct<VkRenderPassCreateInfo>();
	renderPassCreateInfo.attachmentCount = 1u;
	renderPassCreateInfo.pAttachments = &attachementDesc;
	renderPassCreateInfo.subpassCount = 1u;
	renderPassCreateInfo.pSubpasses = &subpassDesc;

	mRenderPass = create_VkRenderPass(&renderPassCreateInfo);
}

void Shadow::SetupPipelineLayout()
{
	//VkPushConstantRange vertexPushConstantRange;
	//vertexPushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
	//vertexPushConstantRange.offset = 0u;
	//vertexPushConstantRange.size = 64u; // mat4

	auto pipelineLayoutCreateInfo = vkstruct<VkPipelineLayoutCreateInfo>();
	pipelineLayoutCreateInfo.setLayoutCount = GlobalDescriptorSets::CB_COUNT;
	pipelineLayoutCreateInfo.pSetLayouts = GlobalDescriptorSets::descriptorSetLayouts;
	pipelineLayoutCreateInfo.pushConstantRangeCount = 0u;
	pipelineLayoutCreateInfo.pPushConstantRanges = nullptr;

	gVkLastRes = vkCreatePipelineLayout(gDevice.VkHandle(), &pipelineLayoutCreateInfo, nullptr, &mPipelineLayout);
	VKFN_LAST_RES_SUCCESS_OR_QUIT(vkCreatePipelineLayout);
}

void Shadow::SetupGraphicsPipeline()
{
	auto pipelineCreateInfo = vkstruct<VkGraphicsPipelineCreateInfo>();
	pipelineCreateInfo.flags = 0;
	pipelineCreateInfo.stageCount = 1u;
	pipelineCreateInfo.pStages = &shaderStageCreateInfo;
	pipelineCreateInfo.pVertexInputState = &vertexInputStateCreateInfo;
	pipelineCreateInfo.pInputAssemblyState = &inputAssemblyStateCreateInfo;
	pipelineCreateInfo.pTessellationState = nullptr;
	pipelineCreateInfo.pViewportState = &viewportStateCreateInfo;
	pipelineCreateInfo.pRasterizationState = &rasterizationStateCreateInfo;
	pipelineCreateInfo.pMultisampleState = &multisampleStateCreateInfo;
	pipelineCreateInfo.pDepthStencilState = &depthstencilStateCreateInfo;
	pipelineCreateInfo.pColorBlendState = nullptr; // No color attachment
	pipelineCreateInfo.pDynamicState = nullptr;
	pipelineCreateInfo.layout = mPipelineLayout;
	pipelineCreateInfo.renderPass = mRenderPass;
	pipelineCreateInfo.subpass = 0u;

	gVkLastRes = vkCreateGraphicsPipelines(gDevice.VkHandle(), VK_NULL_HANDLE, 1u, &pipelineCreateInfo, nullptr, &mPipeline);
	VKFN_LAST_RES_SUCCESS_OR_QUIT(vkCreateGraphicsPipelines);
}

void Shadow::SetupRenderTargets()
{
	CreateDepthImageAndView(&mDepthImage.image, &mDepthImage.view, &mDepthImage.memory, SHADOWMAP_RES, SHADOWMAP_RES, 0u, true);
}

void Shadow::SetupFrameRenderResources()
{
	shadowMapCompleteSemaphore = create_VkSemaphore();
}


void Shadow::TransitionInitiallyUndefinedImages()
{
	// WARNING ! No transitioning is done currently !
	// Since I use a Nvidia card, this is not a problem for me as it is totally ignored.
	// Will do once the 3D rendering part is cleanly separated from the present part.
}

void Shadow::CleanupSetupOnlyResources()
{
	// From this point, the shader modules are consumed by the API and no longer needed
	vkDestroyShaderModule(gDevice.VkHandle(), vertexModule, nullptr);
}

void Shadow::SetupCmdBufferPool()
{
	// COMMAND BUFFER (POOL)

	auto cmdPoolCreateInfo = vkstruct<VkCommandPoolCreateInfo>();
	cmdPoolCreateInfo.flags = 0;
	cmdPoolCreateInfo.queueFamilyIndex = 0u;

	gVkLastRes = vkCreateCommandPool(gDevice.VkHandle(), &cmdPoolCreateInfo, nullptr, &mCmdPool);
	VKFN_LAST_RES_SUCCESS_OR_QUIT(vkCreateCommandPool);
}

void Shadow::CookCmdBuffer(const Scene& scene)
{
	// COMMAND BUFFER
	auto drawTriangleCmdBufferAllocInfo = vkstruct<VkCommandBufferAllocateInfo>();
	drawTriangleCmdBufferAllocInfo.commandPool = mCmdPool;
	drawTriangleCmdBufferAllocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	drawTriangleCmdBufferAllocInfo.commandBufferCount = 1u;

	gVkLastRes = vkAllocateCommandBuffers(gDevice.VkHandle(), &drawTriangleCmdBufferAllocInfo, &mCmdBuffer);
	VKFN_LAST_RES_SUCCESS_OR_QUIT(vkAllocateCommandBuffers);

	auto framebufferCreateInfo = vkstruct<VkFramebufferCreateInfo>();
	framebufferCreateInfo.flags = 0;
	framebufferCreateInfo.renderPass = mRenderPass;
	framebufferCreateInfo.attachmentCount = 1u;
	framebufferCreateInfo.pAttachments = &mDepthImage.view;
	framebufferCreateInfo.width = static_cast<u32>(SHADOWMAP_RES);
	framebufferCreateInfo.height = static_cast<u32>(SHADOWMAP_RES);
	framebufferCreateInfo.layers = 1u;

	gVkLastRes = vkCreateFramebuffer(gDevice.VkHandle(), &framebufferCreateInfo, nullptr, &mFramebuffer);
	VKFN_LAST_RES_SUCCESS_OR_QUIT(vkCreateFramebuffer);

	VkClearValue clearDepthStencilValue;
	clearDepthStencilValue.depthStencil = { 1.f, 0 };

	auto renderPassBeginInfo = vkstruct<VkRenderPassBeginInfo>();
	renderPassBeginInfo.renderPass = mRenderPass;
	renderPassBeginInfo.framebuffer = mFramebuffer;
	renderPassBeginInfo.renderArea = { { 0, 0 },{ SHADOWMAP_RES, SHADOWMAP_RES } };
	renderPassBeginInfo.clearValueCount = 1u;
	renderPassBeginInfo.pClearValues = &clearDepthStencilValue;

	auto cmdBufferBeginInfo = vkstruct<VkCommandBufferBeginInfo>();
	cmdBufferBeginInfo.flags = 0;

	// REGISTER COMMAND BUFFER

	gVkLastRes = vkBeginCommandBuffer(mCmdBuffer, &cmdBufferBeginInfo);
	VKFN_LAST_RES_SUCCESS_OR_QUIT(vkBeginCommandBuffer);

	// TRANSITION RENDERTARGET FROM PRESENTABLE TO RENDERABLE LAYOUT + Initial transition from UNDEFINED layout

	vkCmdBeginRenderPass(mCmdBuffer, &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);

	vkCmdBindPipeline(mCmdBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, mPipeline);

	vkCmdBindDescriptorSets(mCmdBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, mPipelineLayout,
		0u, GlobalDescriptorSets::CB_COUNT, GlobalDescriptorSets::descriptorSets, 0u, nullptr);

	scene.BindDrawingToCmdBuffer(mCmdBuffer);

	vkCmdEndRenderPass(mCmdBuffer);


	gVkLastRes = vkEndCommandBuffer(mCmdBuffer);
	VKFN_LAST_RES_SUCCESS_OR_QUIT(vkEndCommandBuffer);
}

void Shadow::SubmitFrameRender(VkQueue graphicQueue)
{
	auto submitInfo = vkstruct<VkSubmitInfo>();
	submitInfo.waitSemaphoreCount = 0u;
	submitInfo.pWaitSemaphores = nullptr;
	submitInfo.pWaitDstStageMask = nullptr;
	submitInfo.commandBufferCount = 1u;
	submitInfo.pCommandBuffers = &mCmdBuffer;
	submitInfo.signalSemaphoreCount = 1u;
	submitInfo.pSignalSemaphores = &shadowMapCompleteSemaphore;

	gVkLastRes = vkQueueSubmit(graphicQueue, 1u, &submitInfo, VK_NULL_HANDLE);
	VKFN_LAST_RES_SUCCESS_OR_QUIT(vkQueueSubmit);
}

void Shadow::Destroy()
{
	vkDestroySemaphore(gDevice.VkHandle(), shadowMapCompleteSemaphore, nullptr);

	mDepthImage.Destroy();

	vkDestroyPipelineLayout(gDevice.VkHandle(), mPipelineLayout, nullptr);

	vkDestroyRenderPass(gDevice.VkHandle(), mRenderPass, nullptr);

	vkDestroyPipeline(gDevice.VkHandle(), mPipeline, nullptr);
}
