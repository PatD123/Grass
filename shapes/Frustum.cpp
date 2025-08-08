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

// Update frustum based on position.

bool Frustum::check(const glm::vec3& p, const glm::mat4& modelTransform) {

	// Transform point
	const glm::vec3 model_p = modelTransform * glm::vec4(p, 1);

	// If all these return true, we're good.
	return
		checkPlane(model_p, m_nearPlane) &&
		checkPlane(model_p, m_farPlane) &&
		checkPlane(model_p, m_leftPlane) &&
		checkPlane(model_p, m_rightPlane) &&
		checkPlane(model_p, m_botPlane) &&
		checkPlane(model_p, m_topPlane);
		
}

bool Frustum::checkPlane(const glm::vec3& p, const Plane& plane) {

	// If the dot product is negative, which means it will point inward into frustum.
	return glm::dot(p - plane.m_normPos, plane.m_norm) < 0;
}