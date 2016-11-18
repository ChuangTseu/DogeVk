#include "global_descriptor_sets.h"

GlobalDescriptorSets::UsedCStructs GlobalDescriptorSets::usedCStructs;
VkDescriptorSetLayout GlobalDescriptorSets::descriptorSetLayouts[GlobalDescriptorSets::CB_COUNT];
VkDescriptorPool GlobalDescriptorSets::descriptorPool;
VkDescriptorSet GlobalDescriptorSets::descriptorSets[GlobalDescriptorSets::CB_COUNT];
DedicatedBuffer GlobalDescriptorSets::uniformBuffers[GlobalDescriptorSets::CB_COUNT];

void GlobalDescriptorSets::Initialize()
{
	VkDescriptorSetLayoutBinding descripterSetLayoutBinding;
	descripterSetLayoutBinding.binding = 0u;
	descripterSetLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	descripterSetLayoutBinding.descriptorCount = 1u;
	descripterSetLayoutBinding.stageFlags = VK_SHADER_STAGE_ALL_GRAPHICS;
	descripterSetLayoutBinding.pImmutableSamplers = nullptr;

	auto descriptorSetLayoutCreateInfo = vkstruct<VkDescriptorSetLayoutCreateInfo>();
	descriptorSetLayoutCreateInfo.bindingCount = 1u;
	descriptorSetLayoutCreateInfo.pBindings = &descripterSetLayoutBinding;

	for (u32 i = 0; i < CB_COUNT; ++i) {
		gVkLastRes = vkCreateDescriptorSetLayout(gDevice.VkHandle(), &descriptorSetLayoutCreateInfo, nullptr, &descriptorSetLayouts[i]);
		VKFN_LAST_RES_SUCCESS_OR_QUIT(vkCreateDescriptorSetLayout);
	}

	// Setup global descriptor pool and allocate all descriptor sets

	VkDescriptorPoolSize descriptorPoolSize;
	descriptorPoolSize.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	descriptorPoolSize.descriptorCount = CB_COUNT;

	auto descriptorPoolCreateInfo = vkstruct<VkDescriptorPoolCreateInfo>();
	descriptorPoolCreateInfo.flags = 0;
	descriptorPoolCreateInfo.maxSets = CB_COUNT;
	descriptorPoolCreateInfo.poolSizeCount = 1u;
	descriptorPoolCreateInfo.pPoolSizes = &descriptorPoolSize;

	gVkLastRes = vkCreateDescriptorPool(gDevice.VkHandle(), &descriptorPoolCreateInfo, nullptr, &descriptorPool);
	VKFN_LAST_RES_SUCCESS_OR_QUIT(vkCreateDescriptorPool);

	auto descriptorSetAllocateInfo = vkstruct<VkDescriptorSetAllocateInfo>();
	descriptorSetAllocateInfo.descriptorPool = descriptorPool;
	descriptorSetAllocateInfo.descriptorSetCount = CB_COUNT;
	descriptorSetAllocateInfo.pSetLayouts = descriptorSetLayouts;

	gVkLastRes = vkAllocateDescriptorSets(gDevice.VkHandle(), &descriptorSetAllocateInfo, descriptorSets);
	VKFN_LAST_RES_SUCCESS_OR_QUIT(vkAllocateDescriptorSets);

	// Update once and for all the descriptor set bindings with the correct underlying storage for all the CBuffers

	VkDescriptorBufferInfo descriptorBufferInfos[CB_COUNT];

	for (u32 i = 0; i < CB_COUNT; ++i) {
		uniformBuffers[i].Create(VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, USED_CSTRUCT_SIZES[i], DedicatedBuffer::eViaHostAccess);

		descriptorBufferInfos[i].buffer = uniformBuffers[i].m_buffer;
		descriptorBufferInfos[i].offset = VkDeviceSize{ 0 };
		descriptorBufferInfos[i].range = VK_WHOLE_SIZE;
	}	

	VkWriteDescriptorSet descriptorWrites[CB_COUNT];

	for (u32 i = 0; i < CB_COUNT; ++i) {
		descriptorWrites[i] = vkstruct<VkWriteDescriptorSet>();
		descriptorWrites[i].dstSet = descriptorSets[i];
		descriptorWrites[i].dstBinding = 0u;
		descriptorWrites[i].dstArrayElement = 0u;
		descriptorWrites[i].descriptorCount = 1u;
		descriptorWrites[i].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		descriptorWrites[i].pBufferInfo = &descriptorBufferInfos[i];
	}

	vkUpdateDescriptorSets(gDevice.VkHandle(), CB_COUNT, descriptorWrites, 0u, nullptr);
}
