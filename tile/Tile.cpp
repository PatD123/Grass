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
	const Camera& cam,
	const glm::mat4& proj_view, 
	const glm::mat4& model, 
	ShaderHelper& sh, 
	GLuint shaderProgram,
	GLuint VAO,
	GLuint vertVBO,
	GLuint normVBO
) {
	glBindVertexArray(VAO);

	// LODing
	int increment = 1;
	float dis = glm::length(cam.m_pos - m_tilePos);
	if (dis < 7.0f)
		increment = 1;
	else if (7.0f <= dis && dis <= 10.f)
		increment = 2;
	else if (10.0f < dis <= 15)
		increment = 6;
	else increment = NUM_BEZIER_VERTS;

	for (int i = 0; i<m_bladesPerTile; i+=increment)
	{
		Grass& g = m_blades[i];

		glm::mat4 transfm = g.m_transform * model;

		bool flag = false;
		for (glm::vec3& p : g.m_boundingQuad) {
			if (!cam.m_frustum->check(p, transfm)) {
				flag = true;
				break;
			}
		}

		if (flag) continue;

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



