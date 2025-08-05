#include "Tile.h"

Tile::Tile(
	int bladesPerTile,
	glm::vec3 tilePos,
	glm::vec3 tileNorm,
	float tileMinHeight,
	float tileMaxHeight,
	float tileMaxLean,
	float tileRadius
) :
	m_bladesPerTile(bladesPerTile), m_tilePos(tilePos), m_tileNorm(tileNorm),
	m_tileMinHeight(tileMinHeight), m_tileMaxHeight(tileMaxHeight), m_tileMaxLean(tileMaxLean),
	m_tileRadius(tileRadius)
{}

void Tile::generateGrass() {
	for (int i = 0; i < m_bladesPerTile; ++i) {
		Grass g;
		g.generateBlade(m_tilePos, m_tileNorm, m_tileMinHeight, m_tileMaxHeight, m_tileMaxLean, m_tileRadius);
		m_blades.push_back(g);
	}
}

void Tile::renderGrass(
	const glm::mat4& proj_view, 
	const glm::mat4& model, 
	ShaderHelper& sh, 
	GLuint shaderProgram,
	GLuint VAO,
	GLuint vertVBO,
	GLuint normVBO
) {
	glBindVertexArray(VAO);

	for (Grass& g : m_blades)
	{
		glm::mat4 transform = proj_view * g.m_transform * model;
		sh.setUniformMat4fv(shaderProgram, "Transform", glm::value_ptr(transform));
		sh.setUniform1f(shaderProgram, "BladeHeight", g.m_bladeHeight);

		// Draw
		sh.useShaderProgram(shaderProgram);
		glBindBuffer(GL_ARRAY_BUFFER, vertVBO);
		glBufferData(GL_ARRAY_BUFFER, g.m_vertices.size() * sizeof(glm::vec3), g.m_vertices.data(), GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, normVBO);
		glBufferData(GL_ARRAY_BUFFER, g.m_normals.size() * sizeof(glm::vec3), g.m_normals.data(), GL_STATIC_DRAW);
		glDrawArrays(GL_TRIANGLES, 0, g.m_vertices.size());
	}

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}



