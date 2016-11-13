#include "camera.h"

#include <glm/gtc/matrix_transform.hpp>

Camera::Camera()
{
}

void Camera::SetPositionProperties(glm::vec3 position, glm::vec3 forward, glm::vec3 up) {
    m_position = position;
    m_forward = forward;
    m_up = up;
}

void Camera::SetProjectionProperties(float fovY, float ratio, float near, float far)
{
	m_proj = glm::perspective(fovY, ratio, near, far);
}

void Camera::UpdateLookAt()
{
	glm::vec3 f = normalize(m_forward);
	glm::vec3 s = normalize(cross(normalize(m_up), f));
	glm::vec3 u = normalize(cross(f, s));

	m_view = glm::lookAt(m_position, m_position + m_forward, glm::vec3(0.f, 1.f, 0.f));

	m_viewProj = m_proj * m_view;
}

const glm::vec3& Camera::GetPosition()
{
	return m_position;
}

const glm::mat4& Camera::GetView() {
    return m_view;
}

const glm::mat4& Camera::GetProjection() {
	return m_proj;
}

const glm::mat4& Camera::GetViewProjection() {
	return m_viewProj;
}

void Camera::StepAdvance(EStepAdvance eStepAdvance)
{
	glm::vec3 kright = normalize(cross(m_forward, m_up));
	glm::vec3 kdown = normalize(cross(m_forward, kright));

	if (eStepAdvance == EStepAdvance_FORWARD) {
		m_position += m_forward*0.1f;
	}
	if (eStepAdvance == EStepAdvance_BACKWARD) {
		m_position -= m_forward*0.1f;
	}
	if (eStepAdvance == EStepAdvance_LEFT) {
		m_position -= kright*0.1f;
	}
	if (eStepAdvance == EStepAdvance_RIGHT) {
		m_position += kright*0.1f;
	}
	if (eStepAdvance == EStepAdvance_UP) {
		m_position += m_up*0.1f;
	}
	if (eStepAdvance == EStepAdvance_DOWN) {
		m_position -= m_up*0.1f;
	}
}

void Camera::RotateByMouseRelCoords(int mouse_x_rel, int mouse_y_rel)
{
	glm::vec3 kright = normalize(cross(m_forward, m_up));
	glm::vec3 kdown = normalize(cross(m_forward, kright));

	m_forward += kright*(mouse_x_rel / 100.f);
	m_forward += kdown*(mouse_y_rel / 100.f);

	m_forward = normalize(m_forward);

	//UpdateLookAt();
}
