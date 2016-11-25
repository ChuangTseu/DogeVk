#pragma once

#include <map>
#include <vector>
#include <algorithm>
#include <numeric>
#include <assert.h>

#include "doge_vulkan.h"
#include "vulkan_helpers.h"
#include "memory.h"
#include "basic_allocators.h"

// IGNORE FOR NOW, WE CERTAINLY *ALWAYS* STAGE INTO GPU ANYWAY
enum HostTranferType {
	eViaHostAccess,
	eViaStaging,
	eDeviceOnly
};

enum EDeviceBufferType {
	kVertex,
	kIndex,
	kUniformStatic,

	MAX_COUNT
};

static const VkBufferUsageFlags AsVkFlag_EGeometryBufferType[EDeviceBufferType::MAX_COUNT] = {
	VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
	VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
	VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
};

// TODO Log mechanism for logging standard resources usage in real conditions

struct DeviceBufferAllocator {
	struct Handle {
		u32 id;
		VkBuffer buffer;
		EDeviceBufferType eGeometryBufferType;
		u32 bankIdx;
		u64 bankOffset;
		u32 queueFamilyIndex;
	};

	DeviceBufferAllocator();

	Handle GetNewBufferHandle(EDeviceBufferType eDeviceBufferType, size_t size, u32 queueFamilyIndex);

	void UploadData(Handle handle, VkDeviceSize offset, VkDeviceSize size, const void* data);

private:
	void CreateBuffer(VkBufferUsageFlags usage, VkDeviceSize size, u32 queueFamilyIndex, VkBuffer* pOutBuffer);

	static constexpr u32 USAGE_COUNT = EDeviceBufferType::MAX_COUNT;

	static constexpr u32 USAGE_MEMORY_BANK_SIZE[USAGE_COUNT] = {
		32u * 1024u * 1024u, // VERTEX: 32MB
		16u * 1024u * 1024u, // INDEX: 16MB
		8u * 1024u * 1024u, // UNIFORM STATIC: 8MB
	};

	// Per spec, all VkBuffer created with the same 'flag' and 'usage' have the same 'alignment' and 'memoryTypeBits'
	VkDeviceSize mPerUsageAlignement[USAGE_COUNT];

	void* mPerUsageHostBanks[USAGE_COUNT];
	void* mPerUsageDeviceBanks[USAGE_COUNT];

	struct Bank {
		VkDeviceMemory memory;
		u64 size;

		PagedAlignedFreeList allocator;
	};

	// I know, put that map down ! Should be done in due time.
	std::map<u32, std::vector<Bank>> mFamilyBanks[USAGE_COUNT];
};

extern DeviceBufferAllocator gDeviceBufferAllocator;
