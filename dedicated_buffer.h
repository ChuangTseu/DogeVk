#pragma once

#include "doge_vulkan.h"

void createSimpleBuffer(VkBufferUsageFlags usage, VkDeviceSize size, VkBuffer* pOutBuffer);
void allocateNewBufferMemory(const VkBuffer* pBuffer, VkDeviceMemory* pOutBufferMemory, VkMemoryPropertyFlags desiredMemTypeFlags);

struct DedicatedBuffer {
	enum HostTranferType {
		eViaHostAccess,
		eViaStaging,
		eDeviceOnly
	};

	void Create(VkBufferUsageFlags usage, VkDeviceSize size, HostTranferType eTransferType);

	void Destroy();

	void UploadData(VkDeviceSize offset, VkDeviceSize size, const void* data);

	VkBuffer m_buffer;
	VkDeviceMemory m_memory;
	HostTranferType m_eTransferType;
};
