#ifndef CAMERA_H
#define CAMERA_H

#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>

#include "viewpoint.h"

class Camera
{
public:
    Camera();

	void SetPositionProperties(glm::vec3 position, glm::vec3 forward, glm::vec3 up);

	void SetProjectionProperties(float fovY, float ratio, float near, float far);

	void UpdateLookAt();

	ViewPoint ComputeViewPoint() const
	{
		ViewPoint cameraViewPoint;

		cameraViewPoint.m_view[0] = m_view;
		cameraViewPoint.m_proj[0] = m_proj;
		cameraViewPoint.m_viewProj[0] = m_viewProj;

		cameraViewPoint.m_eyePosition = m_position;

		return cameraViewPoint;
	}

	const glm::vec3& GetPosition();
	const glm::mat4& GetView();
	const glm::mat4& GetProjection();
	const glm::mat4& GetViewProjection();

	enum EStepAdvance {
		EStepAdvance_FORWARD,
		EStepAdvance_BACKWARD,
		EStepAdvance_LEFT,
		EStepAdvance_RIGHT,
		EStepAdvance_DOWN,
		EStepAdvance_UP
	};

	void StepAdvance(EStepAdvance eStepAdvance);

	void RotateByMouseRelCoords(int mouse_x_rel, int mouse_y_rel);

private:
	glm::mat4 m_view;
	glm::mat4 m_proj;
	glm::mat4 m_viewProj;

	glm::vec3 m_position;
	glm::vec3 m_forward;
	glm::vec3 m_up;
};

#endif // CAMERA_H
