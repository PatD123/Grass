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

	// Load 6 model_p points into a 256 bit register (8 floats)
	float x1[6], y1[6], z1[6];
	for (int i = 0; i < 6; i++) {
		x1[i] = model_p[0];
		y1[i] = model_p[1];
		z1[i] = model_p[2];
	}
	__m256 vx_model = _mm256_loadu_ps(x1);
	__m256 vy_model = _mm256_loadu_ps(y1);
	__m256 vz_model = _mm256_loadu_ps(z1);

	// Load each planes norm_pos into a 256 bit register (8 floats)
	float x2[6], y2[6], z2[6];
	x2[0] = m_nearPlane.m_normPos[0]; y2[0] = m_nearPlane.m_normPos[1]; z2[0] = m_nearPlane.m_normPos[2];
	x2[1] = m_farPlane.m_normPos[0]; y2[1] = m_farPlane.m_normPos[1]; z2[1] = m_farPlane.m_normPos[2];
	x2[2] = m_leftPlane.m_normPos[0]; y2[2] = m_leftPlane.m_normPos[1]; z2[2] = m_leftPlane.m_normPos[2];
	x2[3] = m_rightPlane.m_normPos[0]; y2[3] = m_rightPlane.m_normPos[1]; z2[3] = m_rightPlane.m_normPos[2];
	x2[4] = m_botPlane.m_normPos[0]; y2[4] = m_botPlane.m_normPos[1]; z2[4] = m_botPlane.m_normPos[2];
	x2[5] = m_topPlane.m_normPos[0]; y2[5] = m_topPlane.m_normPos[1]; z2[5] = m_topPlane.m_normPos[2];
	__m256 vx_planeNormPos = _mm256_loadu_ps(x2);
	__m256 vy_planeNormPos = _mm256_loadu_ps(y2);
	__m256 vz_planeNormPos = _mm256_loadu_ps(z2);

	// Sub normPos from model_p
	__m256 result_x = _mm256_sub_ps(vx_model, vx_planeNormPos);
	__m256 result_y = _mm256_sub_ps(vy_model, vy_planeNormPos);
	__m256 result_z = _mm256_sub_ps(vz_model, vz_planeNormPos);

	// Load 6 plane normals
	float x3[6], y3[6], z3[6];
	x3[0] = m_nearPlane.m_norm[0];  y3[0] = m_nearPlane.m_norm[1];  z3[0] = m_nearPlane.m_norm[2];
	x3[1] = m_farPlane.m_norm[0];   y3[1] = m_farPlane.m_norm[1];   z3[1] = m_farPlane.m_norm[2];
	x3[2] = m_leftPlane.m_norm[0];  y3[2] = m_leftPlane.m_norm[1];  z3[2] = m_leftPlane.m_norm[2];
	x3[3] = m_rightPlane.m_norm[0]; y3[3] = m_rightPlane.m_norm[1]; z3[3] = m_rightPlane.m_norm[2];
	x3[4] = m_botPlane.m_norm[0];   y3[4] = m_botPlane.m_norm[1];   z3[4] = m_botPlane.m_norm[2];
	x3[5] = m_topPlane.m_norm[0];   y3[5] = m_topPlane.m_norm[1];   z3[5] = m_topPlane.m_norm[2];
	__m256 vx_planeNorms = _mm256_loadu_ps(x3);
	__m256 vy_planeNorms = _mm256_loadu_ps(y3);
	__m256 vz_planeNorms = _mm256_loadu_ps(z3);

	// Fuse Multiply Add (FMA)
	__m256 result = _mm256_mul_ps(result_x, vx_planeNorms);                  // x1*x2
	result = _mm256_fmadd_ps(result_y, vy_planeNorms, result);               // x1*x2 + y1*y2
	result = _mm256_fmadd_ps(result_z, vz_planeNorms, result);

	float checkResults[8];
	_mm256_storeu_ps(checkResults, result);

	for (int i = 2; i < 8; i++) {
		if (checkResults[i] > 0) return false;
	}

	return true;

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