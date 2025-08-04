#include <iostream>
#include <stdlib.h>
#include <random>
#include <cmath>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "../common/utils.hpp"

// CONSTS
const static glm::vec3 YAXIS = glm::vec3(0.0, 1.0, 0.0);
const static int NUM_BEZIER_VERTS = 5; // Vertices per side of bezier.

class Grass {
public:
	Grass(glm::vec3 localPos = glm::vec3(0.0));

	void generateBlade(
		glm::vec3 grassPatchPos,
		glm::vec3 grassPatchNorm,
		float grassPatchMinHeight,
		float grassPatchMaxHeight,
		float grassPatchMaxLean,
		float grassPatchRadius
	);

	glm::vec3 m_localPos;
	//glm::vec3 m_vertices[18];
	std::vector<glm::vec3> m_vertices;
	std::vector<glm::vec3> m_normals;
	glm::mat4 m_transform;
	float m_bladeHeight;
};