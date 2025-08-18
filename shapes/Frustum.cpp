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

	// Load norm positions
	x2[0] = m_nearPlane.m_normPos[0]; y2[0] = m_nearPlane.m_normPos[1]; z2[0] = m_nearPlane.m_normPos[2];
	x2[1] = m_farPlane.m_normPos[0]; y2[1] = m_farPlane.m_normPos[1]; z2[1] = m_farPlane.m_normPos[2];
	x2[2] = m_leftPlane.m_normPos[0]; y2[2] = m_leftPlane.m_normPos[1]; z2[2] = m_leftPlane.m_normPos[2];
	x2[3] = m_rightPlane.m_normPos[0]; y2[3] = m_rightPlane.m_normPos[1]; z2[3] = m_rightPlane.m_normPos[2];
	x2[4] = m_botPlane.m_normPos[0]; y2[4] = m_botPlane.m_normPos[1]; z2[4] = m_botPlane.m_normPos[2];
	x2[5] = m_topPlane.m_normPos[0]; y2[5] = m_topPlane.m_normPos[1]; z2[5] = m_topPlane.m_normPos[2];
	vx_planeNormPos = _mm256_load_ps(x2);
	vy_planeNormPos = _mm256_load_ps(y2);
	vz_planeNormPos = _mm256_load_ps(z2);

	// Load norm vectors
	x3[0] = m_nearPlane.m_norm[0];  y3[0] = m_nearPlane.m_norm[1];  z3[0] = m_nearPlane.m_norm[2];
	x3[1] = m_farPlane.m_norm[0];   y3[1] = m_farPlane.m_norm[1];   z3[1] = m_farPlane.m_norm[2];
	x3[2] = m_leftPlane.m_norm[0];  y3[2] = m_leftPlane.m_norm[1];  z3[2] = m_leftPlane.m_norm[2];
	x3[3] = m_rightPlane.m_norm[0]; y3[3] = m_rightPlane.m_norm[1]; z3[3] = m_rightPlane.m_norm[2];
	x3[4] = m_botPlane.m_norm[0];   y3[4] = m_botPlane.m_norm[1];   z3[4] = m_botPlane.m_norm[2];
	x3[5] = m_topPlane.m_norm[0];   y3[5] = m_topPlane.m_norm[1];   z3[5] = m_topPlane.m_norm[2];
	vx_planeNorms = _mm256_load_ps(x3);
	vy_planeNorms = _mm256_load_ps(y3);
	vz_planeNorms = _mm256_load_ps(z3);

	vx_planeDiff = _mm256_mul_ps(vx_planeNormPos, vx_planeNorms);
	vy_planeDiff = _mm256_mul_ps(vy_planeNormPos, vy_planeNorms);
	vz_planeDiff = _mm256_mul_ps(vz_planeNormPos, vz_planeNorms);
}


// Update frustum based on position.

bool Frustum::check(const glm::vec3& model_p) const {

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

	// Broadcast model across all lanes
	__m256 vx_model = _mm256_broadcast_ss(&model_p[0]);
	__m256 vy_model = _mm256_broadcast_ss(&model_p[1]);
	__m256 vz_model = _mm256_broadcast_ss(&model_p[2]);

	// Sub normPos from model_p
	__m256 result_x = _mm256_fmsub_ps(vx_model, vx_planeNorms, vx_planeDiff);
	__m256 result_y = _mm256_fmsub_ps(vy_model, vy_planeNorms, vy_planeDiff);
	__m256 result_z = _mm256_fmsub_ps(vz_model, vz_planeNorms, vz_planeDiff);

	__m256 result = _mm256_add_ps(result_x, result_y);
	result = _mm256_add_ps(result, result_z);

	__m256 cmp = _mm256_cmp_ps(result, _mm256_setzero_ps(), _CMP_GT_OQ);

	// Early-out: if any dot > 0, the point is outside
	if (!_mm256_testz_ps(cmp, cmp)) {
		return false;
	}
	return true;
		
}

bool Frustum::checkPlane(const glm::vec3& p, const Plane& plane) const {

	// If the dot product is negative, which means it will point inward into frustum.
	return glm::dot(p - plane.m_normPos, plane.m_norm) < 0;
}