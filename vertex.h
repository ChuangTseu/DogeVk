#pragma once

#include <glm/glm.hpp>

#include "doge_vulkan.h"

struct Vertex {
	glm::vec3 position;
	glm::vec3 normal;
	//vec2 texcoord;
	//vec3 tangent;

	static const VkPipelineVertexInputStateCreateInfo& GetPipelineVertexInputStateCreateInfo() {
		static const VkVertexInputBindingDescription inputBindingDesc = {
			0u, // binding
			sizeof(Vertex), // stride
			VK_VERTEX_INPUT_RATE_VERTEX // inputRate
		};

		static const VkVertexInputAttributeDescription positionInputAttribDesc = {
			0u, // location
			0u, // binding
			VK_FORMAT_R32G32B32_SFLOAT, // format
			0u, // offset
		};

		static const VkVertexInputAttributeDescription normalInputAttribDesc = {
			1u, // location
			0u, // binding
			VK_FORMAT_R32G32B32_SFLOAT, // format
			12u, // offset
		};

		static const VkVertexInputAttributeDescription vertexInputAttribDescs[2] = {
			positionInputAttribDesc,
			normalInputAttribDesc
		};

		static const VkPipelineVertexInputStateCreateInfo vertexInputStateCreateInfo = {
			VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO, // sType
			nullptr, // pNext
			0, // flags - reserved for future use

			1u, // vertexBindingDescriptionCount
			&inputBindingDesc, // pVertexBindingDescriptions
			2u, // vertexAttributeDescriptionCount
			vertexInputAttribDescs // pVertexAttributeDescriptions
		};

		return vertexInputStateCreateInfo;
	}
};
