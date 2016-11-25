#pragma once

#include <vector>
#include <iostream>

#include "doge_types.h"

#include "vertex.h"
#include "device_buffer_allocator.h"

struct aiMesh;

struct Mesh {
    std::vector<Vertex> m_vertices;
    std::vector<u32> m_indices;

	DeviceBufferAllocator::Handle vertexBufferHandle;
	DeviceBufferAllocator::Handle indexBufferHandle;

    //VAO m_vao; ~~== CmdBuffer ??

    unsigned int m_materialIndex;

    bool loadFromAssimpMesh(const aiMesh* mesh);

	void BindDrawingToCmdBuffer(VkCommandBuffer cmdBuffer) const;

    void draw() const;

    void drawAsTriangles() const;

    void drawAsPatch();

    bool loadFullscreenQuad();
};
