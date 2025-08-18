#pragma once

#include <iostream>
#include <stdlib.h>
#include <random>
#include <cmath>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "../shapes/Grass.h"
#include "../common/ShaderHelper.h"
#include "../camera/Camera.h"

class Tile {
public:
	Tile(
		const int bladesPerTile,
		glm::vec3 tilePos,
		glm::vec3 tileNorm,
		float tileMinHeight,
		float tileMaxHeight,
		float tileMaxLean
	);

	void generateGrass();

	void renderGrass(
		const Camera& cam,
		const glm::mat4& proj_view,
		ShaderHelper& sh,
		GLuint shaderProgram,
		GLuint VAO
	);

	void animateGrass(const Camera& cam);

	int m_bladesPerTile;
	const glm::vec3 m_tilePos;
	glm::vec3 m_tileNorm;
	float m_tileMinHeight;
	float m_tileMaxHeight;
	float m_tileMaxLean;

	// Number of blades drawn per frame, after culling.
	int m_numBladesDrawn = 0;

	std::vector<Grass> m_blades;

	// For moving to GPU
	std::vector<glm::vec3> m_transforms;
	std::vector<float> m_bladeDirs;
	std::vector<float> m_bladeScalings;
};