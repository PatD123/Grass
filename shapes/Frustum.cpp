#include "Frustum.h"

Frustum::Frustum(float aspectRatio, float fov, float zNear, float zFar)
	: m_aspectRatio(aspectRatio), m_fov(fov), m_zNear(zNear), m_zFar(zFar), m_halfVSide(zFar* glm::tan(fov / 2.0)), m_halfHSide(aspectRatio* m_halfVSide)
{}

void Frustum::update(const glm::vec3& camFront, const glm::vec3& camRight, const glm::vec3& camUp, const glm::vec3& camPos) {
	const glm::vec3 middleVec = camFront * m_zFar;

	// MY NORMALS POINT OUT.
	m_nearPlane = Plane(camPos + camFront * m_zNear, -camFront);
	m_farPlane = Plane(camPos + camFront * m_zFar, camFront);
	m_rightPlane = Plane(camPos, glm::cross(middleVec + camRight * m_halfHSide, camUp));
	m_leftPlane = Plane(camPos, -glm::cross(middleVec - camRight * m_halfHSide, camUp));
	m_topPlane = Plane(camPos, glm::cross(camRight, middleVec + camUp * m_halfVSide));
	m_botPlane = Plane(camPos, glm::cross(-camRight, middleVec - camUp * m_halfVSide));
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

// This has got to be SIMD'd
bool Frustum::checkPlane(const glm::vec3& p, const Plane& plane) {

	// If the dot product is negative, which means it will point inward into frustum.
	return glm::dot(p - plane.m_normPos, plane.m_norm) < 0;
}