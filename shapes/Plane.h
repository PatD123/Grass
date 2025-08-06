#pragma once

#include <iostream>
#include <stdlib.h>
#include <random>
#include <cmath>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

// Have a class per sides of our Frustum
class Plane {
public:
	Plane(glm::vec3 pos, glm::vec3 norm);

	glm::vec3 m_normPos;
	glm::vec3 m_norm;
};