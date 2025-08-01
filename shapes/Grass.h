#include <iostream>
#include <stdlib.h>
#include <random>
#include <cmath>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

// CONSTS
const static glm::vec3 YAXIS = glm::vec3(0.0, 1.0, 0.0);
const static int NUM_BEZIER_VERTS = 4; // Vertices per side of bezier.

class Grass {
public:
	Grass();

	void generateBlade(glm::vec3 grassPatchPos, glm::vec3 grassPatchNorm, float grassPatchMaxHeight, float grassPatchRadius);

	glm::vec3 m_position;
	//glm::vec3 m_vertices[18];
	std::vector<glm::vec3> m_vertices;
	glm::mat4 m_transform;
};