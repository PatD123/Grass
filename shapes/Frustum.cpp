#include "Frustum.h"

Frustum::Frustum(const Camera& cam, float aspectRatio, float fov, float zNear, float zFar) {
	const float halfVSide = zFar * glm::tan(fov / 2.0);
	const float halfHSide = aspectRatio * halfVSide;

	const glm::vec3 middleVec = cam.m_front * zFar;

	// MY NORMALS POINT OUT.
	m_nearPlane = Plane(cam.m_pos + cam.m_front * zNear, -cam.m_front);
	m_farPlane = Plane(cam.m_pos + cam.m_front * zFar, cam.m_front);
	m_rightPlane = Plane(cam.m_pos, glm::cross(middleVec + cam.m_right * halfHSide, cam.m_up));
	m_leftPlane = Plane(cam.m_pos, -glm::cross(middleVec - cam.m_right * halfHSide, cam.m_up));
	m_topPlane = Plane(cam.m_pos, glm::cross(cam.m_right, middleVec + cam.m_up * halfVSide));
	m_botPlane = Plane(cam.m_pos, glm::cross(-cam.m_right, middleVec - cam.m_up * halfVSide));
}