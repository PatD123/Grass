#include "Tile.h"

Tile::Tile(
	int bladesPerTile,
	glm::vec3 tilePos,
	glm::vec3 tileNorm,
	float tileMinHeight,
	float tileMaxHeight,
	float tileMaxLean
) :
	m_bladesPerTile(bladesPerTile), m_tilePos(tilePos), m_tileNorm(tileNorm),
	m_tileMinHeight(tileMinHeight), m_tileMaxHeight(tileMaxHeight), m_tileMaxLean(tileMaxLean)
{}

void Tile::generateGrass() {
	for (int i = 0; i < m_bladesPerTile; ++i) {
		Grass g;
		g.generateBlade(m_tilePos, m_tileNorm, m_tileMinHeight, m_tileMaxHeight, m_tileMaxLean);
		m_blades.push_back(g);
	}
}

void Tile::renderGrass(
	const Camera& cam,
	const glm::mat4& proj_view,
	ShaderHelper& sh, 
	GLuint shaderProgram,
	GLuint VAO
) {
	glBindVertexArray(VAO);

	// LODing
	int increment = 1;
	float dis = glm::length(cam.m_pos - m_tilePos);
	if (dis < 7.0f)
		increment = 1;
	else if (7.0f <= dis && dis <= 10.f)
		increment = 3;
	else if (10.0f < dis <= 15)
		increment = 6;
	else increment = NUM_BEZIER_VERTS;

	sh.setUniformMat4fv(shaderProgram, "Proj_View", glm::value_ptr(proj_view));
	sh.setUniform1f(shaderProgram, "Time", glfwGetTime());

	// Draw
	sh.useShaderProgram(shaderProgram);
	glDrawArraysInstanced(GL_TRIANGLES, 0, 84, m_numBladesDrawn);

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void Tile::animateGrass(const Camera& cam) {
	PerlinNoise2D pn;

	m_numBladesDrawn = 0;

	// LODing
	int increment = 1;
	float dis = glm::length(cam.m_pos - m_tilePos);
	if (dis < 7.0f)
		increment = 1;
	else if (7.0f <= dis && dis <= 10.f)
		increment = 3;
	else if (10.0f < dis <= 15)
		increment = 6;
	else increment = NUM_BEZIER_VERTS;

	m_bladeScalings.clear();
	m_bladeDirs.clear();
	m_transforms.clear();

	for (int i = 0; i < m_bladesPerTile; i += increment) {
		Grass& g = m_blades[i];

		// SIMD matrix transforms
		//__m128 results[4];
		//__m128 x = _mm_load_ps(g.m_x);
		//__m128 y = _mm_load_ps(g.m_y);
		//__m128 z = _mm_load_ps(g.m_z);
		//transformQuad(results, x, y, z, g.m_transform);

		//// Extract out results
		//float x_result[4], y_result[4], z_result[4];
		//_mm_store_ps(x_result, results[0]);
		//_mm_store_ps(y_result, results[1]);
		//_mm_store_ps(z_result, results[2]);

		//if (!cam.m_frustum->check(glm::vec3(x_result[0], y_result[0], z_result[0])))
		//	continue;
		//if (!cam.m_frustum->check(glm::vec3(x_result[1], y_result[1], z_result[1])))
		//	continue;
		//if (!cam.m_frustum->check(glm::vec3(x_result[2], y_result[2], z_result[2])))
		//	continue;
		//if (!cam.m_frustum->check(glm::vec3(x_result[3], y_result[3], z_result[3])))
		//	continue;

		m_numBladesDrawn++;

		// Computing direction of grass
		float rot = pn.eval(
			glm::vec2(
				g.m_bladeWorldPosition.x * 0.4f,
				g.m_bladeWorldPosition.z * 0.4f + glfwGetTime() * 0.5f
			)
		);
		g.m_bladeDir = (rot + 1.0f) * std::_Pi_val;

		m_transforms.push_back(g.m_bladeWorldPosition);
		m_bladeScalings.push_back(g.m_bladeScaling);
		m_bladeDirs.push_back(g.m_bladeDir);
	}
}