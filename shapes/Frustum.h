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
#include "../camera/Camera.h"

class Frustum {
public:
	Frustum(const Camera& cam, float aspectRatio, float fov, float zNear, float zFar);

	bool check(const glm::vec3& p, const glm::mat4& modelTransform);

private:

	bool checkPlane(const glm::vec3& p, const Plane& plane);

	Plane m_nearPlane;
	Plane m_farPlane;
	Plane m_leftPlane;
	Plane m_rightPlane;
	Plane m_topPlane;
	Plane m_botPlane;
};