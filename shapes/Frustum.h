#pragma once

#include <iostream>
#include <stdlib.h>
#include <random>
#include <cmath>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "Plane.h"

class Frustum {
public:
	Frustum(float aspectRatio, float fov, float zNear, float zFar);

	bool check(const glm::vec3& p, const glm::mat4& modelTransform) const;

	void update(const glm::vec3& camFront, const glm::vec3& camRight, const glm::vec3& camUp, const glm::vec3& camPos);

private:

	bool checkPlane(const glm::vec3& p, const Plane& plane);

	const float m_aspectRatio;
	const float m_fov;
	const float m_zNear;
	const float m_zFar;
	const float m_halfVSide;
	const float m_halfHSide;

	Plane m_nearPlane;
	Plane m_farPlane;
	Plane m_leftPlane;
	Plane m_rightPlane;
	Plane m_topPlane;
	Plane m_botPlane;

	// For simd_1
	alignas(32) float x2[6], y2[6], z2[6];
	__m256 vx_planeNormPos;
	__m256 vy_planeNormPos;
	__m256 vz_planeNormPos;
	alignas(32) float x3[6], y3[6], z3[6];
	__m256 vx_planeNorms;
	__m256 vy_planeNorms;
	__m256 vz_planeNorms;
	__m256 vx_planeDiff;
	__m256 vy_planeDiff;
	__m256 vz_planeDiff;
};