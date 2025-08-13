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

	// SIMD here
	// 6 of the same points.
	// You need to do
	//	a = p - plane.pos
	//  a . plane.normal
	//
	// Array of 6 vectors.
	// Array of 6 plane.pos.
	// 
	// float x1[], y1[], z1[] --> model_p
	// float x2[], y2[], z2[] --> plane.pos
	// 
	// Sub x1 - x2, y1 - y2, z1 - z2 --> model_p - plane.pos = result
	// result = { x[], y[], z[] }
	// plane.normals = { x[], y[], z[] }
	// Fused Multiply-Add.
	// 
	// CURRENT IMPLEMENTATION (simd_0)
	// 1 Loading 6 models
	// 1 Loading 6 norm_pos
	// 1 Vec subtract
	// 1 Loading 6 norms
	// 1 FMA
	// 
	// OTHER IMPLEMENTATION (simd_1)
	// Prevectorize norm pos and norm --> Load both already
	// 1 Loading 6 models
	// 1 Vec subtract
	// 1 FMA
	// 
	// OTHER IMPLMENTATION (simd_2)
	// (x_model - x_plane_norm_pos) x_plane_norm = x_model * x_plane_norm - x_plane_norm_pos * x_plane_norm
	// Prevectorize x_plane_norm_pos * x_plane_norm
	// Prevectorize x_plane_nrom
	// 1 Loading 6 models
	// 1 Mult
	// 1 Sub

	
		
}

bool Frustum::checkPlane(const glm::vec3& p, const Plane& plane) {

	// If the dot product is negative, which means it will point inward into frustum.
	return glm::dot(p - plane.m_normPos, plane.m_norm) < 0;
}