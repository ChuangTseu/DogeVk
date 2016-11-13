#pragma once

#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>

#define MAX_NUM_VIEWPOINTPLANES_BOB 6

struct ViewPointPlane
{
	glm::mat4 View;
};

class ViewPoint
{
public:
	//void Bind() const;

	glm::mat4 m_view[MAX_NUM_VIEWPOINTPLANES_BOB];
	glm::mat4 m_proj[MAX_NUM_VIEWPOINTPLANES_BOB];
	glm::mat4 m_viewProj[MAX_NUM_VIEWPOINTPLANES_BOB];

	glm::vec3 m_eyePosition;
};

