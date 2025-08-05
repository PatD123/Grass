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

class Tile {
public:
	Tile(
		const int bladesPerTile,
		glm::vec3 tilePos,
		glm::vec3 tileNorm,
		float tileMinHeight,
		float tileMaxHeight,
		float tileMaxLean,
		float tileRadius
	);

	void generateGrass();

	void renderGrass(
		const glm::mat4& proj_view,
		const glm::mat4& model,
		ShaderHelper& sh,
		GLuint shaderProgram,
		GLuint VAO,
		GLuint vertVBO,
		GLuint normVBO,
		glm::vec3 camPos
	);

	int m_bladesPerTile;
	const glm::vec3 m_tilePos;
	glm::vec3 m_tileNorm;
	float m_tileMinHeight;
	float m_tileMaxHeight;
	float m_tileMaxLean;
	float m_tileRadius;

	std::vector<Grass> m_blades;
};