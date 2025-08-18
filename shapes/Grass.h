#pragma once

#include <iostream>
#include <stdlib.h>
#include <random>
#include <cmath>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "../common/utils.hpp"
#include "../common/PerlinNoise.hpp"

// CONSTS
const static glm::vec3 YAXIS = glm::vec3(0.0, 1.0, 0.0);
const static int NUM_BEZIER_VERTS = 15; // Vertices per side of bezier.

class Grass {
public:
	Grass(glm::vec3 localPos = glm::vec3(0.0));

	void generateBlade(
		const glm::vec3& grassPatchPos,
		const glm::vec3& grassPatchNorm,
		const float grassPatchMinHeight,
		const float grassPatchMaxHeight,
		const float grassPatchMaxLean
	);

	glm::vec3 m_localPos;
	glm::vec3 m_bladeWorldPosition;
	glm::mat4 m_bladeDir;
	float m_bladeScaling;
	float m_bladeHeight = 0.7;
	float m_bladeLean = 0.3;
	float m_bladeP0Width = 0.08;
	float m_bladeP1Width = 0.05;
	float m_bladeP2Width = 0.0;

	std::vector<float> m_tSeq;
	std::pair<glm::vec3, glm::vec3> m_negBezier[NUM_BEZIER_VERTS];
	std::pair<glm::vec3, glm::vec3> m_posBezier[NUM_BEZIER_VERTS];
	std::vector<glm::vec3> m_vertices;
	std::vector<glm::vec3> m_normals;
	glm::mat4 m_transform;

	// For bounding quad SIMD
	alignas(32) float m_x[4];
	alignas(32) float m_y[4];
	alignas(32) float m_z[4];
};