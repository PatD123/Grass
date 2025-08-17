#include "Grass.h";

static std::mt19937 GENERATOR(getSeed());

Grass::Grass(glm::vec3 localPos) : m_localPos(localPos) 
{
	for (int i = 0; i <= NUM_BEZIER_VERTS - 1; i++)
		m_tSeq.push_back(static_cast<float>(i) / static_cast<float>(NUM_BEZIER_VERTS - 1));
}

void Grass::generateBlade(
	const glm::vec3& grassPatchPos,
	const glm::vec3& grassPatchNorm,
	const float grassPatchMinHeight,
	const float grassPatchMaxHeight,
	const float grassPatchMaxLean
) {

	// Get new grass pos;
	std::uniform_real_distribution<> dis(-0.5, 0.5);
	m_bladeWorldPosition = glm::vec3(dis(GENERATOR), 0.0f, dis(GENERATOR)) + grassPatchPos;
	m_transform = glm::translate(glm::mat4(), m_bladeWorldPosition);

	// Some random values for our grass at the moment.
	dis = std::uniform_real_distribution<>(-1.0, 1.0);
	m_bladeDir = glm::vec3(1.0f, 0.0f, 0.0f);
	dis = std::uniform_real_distribution<>(grassPatchMinHeight, grassPatchMaxHeight);
	m_bladeHeight = dis(GENERATOR);

	// A single blade of grass is a bezier curve, out of 3 points.
	glm::vec3 p0 = m_localPos;
	glm::vec3 p1 = m_localPos + glm::vec3(0.0f, m_bladeHeight, 0.0f);
	glm::vec3 p2 = p1 + m_bladeDir * m_bladeLean * m_bladeHeight; // Think about bladeHeight here --> Taller blades, bend more

	makePersistentLength(p0, p1, p2, m_bladeHeight);

	// Calculate and apply width vectors
	glm::vec3 bladeSideDir = glm::normalize(glm::cross(m_bladeDir, YAXIS));
	glm::vec3 p0_neg = p0 - bladeSideDir * m_bladeP0Width;
	glm::vec3 p0_pos = p0 + bladeSideDir * m_bladeP0Width;
	glm::vec3 p1_neg = p1 - bladeSideDir * m_bladeP1Width;
	glm::vec3 p1_pos = p1 + bladeSideDir * m_bladeP1Width;
	glm::vec3 p2_neg = p2 - bladeSideDir * m_bladeP2Width;
	glm::vec3 p2_pos = p2 + bladeSideDir * m_bladeP1Width;

	for (int i = 0; i < NUM_BEZIER_VERTS; i++) {
		float t = m_tSeq[i];
		m_negBezier[i] = bezier(p0_neg, p1_neg, p2_neg, t);
		m_posBezier[i] = bezier(p0_pos, p1_pos, p2_pos, t);

		m_bladeHeight = glm::max(m_bladeHeight, m_posBezier[i].first[1]);
	}

	// CCW order
	for (int i = 0; i < NUM_BEZIER_VERTS - 1; i++) {
		// Neg tri
		m_vertices.push_back(m_negBezier[i].first);
		m_normals.push_back(glm::cross(bladeSideDir, m_negBezier[i].second));
		m_vertices.push_back(m_posBezier[i].first);
		m_normals.push_back(glm::cross(bladeSideDir, m_posBezier[i].second));
		m_vertices.push_back(m_negBezier[i + 1].first);
		m_normals.push_back(glm::cross(bladeSideDir, m_negBezier[i + 1].second));

		// Pos tri
		m_vertices.push_back(m_posBezier[i].first);
		m_normals.push_back(glm::cross(bladeSideDir, m_posBezier[i].second));
		m_vertices.push_back(m_posBezier[i + 1].first);
		m_normals.push_back(glm::cross(bladeSideDir, m_posBezier[i + 1].second));
		m_vertices.push_back(m_negBezier[i + 1].first);
		m_normals.push_back(glm::cross(bladeSideDir, m_negBezier[i + 1].second));
	}

	// We need the bounding quad for frustum culling.
	// Previously, I stored the vertices in a vector called boundinQuad, but
	// bc we want to vectorize this, we might as well just store them as
	// Structures of Arrays (SOA) beforehand.

	m_x[0] = m_negBezier[0].first[0]; // First vertex in bounding quad
	m_y[0] = m_negBezier[0].first[1];
	m_z[0] = m_negBezier[0].first[2];

	m_x[1] = m_posBezier[0].first[0]; // Second vertex in bounding quad
	m_y[1] = m_posBezier[0].first[1];
	m_z[1] = m_posBezier[0].first[2];

	m_x[2] = m_negBezier[NUM_BEZIER_VERTS - 1].first[0]; // Third vertex in bounding quad
	m_y[2] = m_negBezier[NUM_BEZIER_VERTS - 1].first[1];
	m_z[2] = m_negBezier[NUM_BEZIER_VERTS - 1].first[2];

	m_x[3] = m_posBezier[NUM_BEZIER_VERTS - 1].first[0]; // Third vertex in bounding quad
	m_y[3] = m_posBezier[NUM_BEZIER_VERTS - 1].first[1];
	m_z[3] = m_posBezier[NUM_BEZIER_VERTS - 1].first[2];
}

void Grass::animate() {
	//
	// A couple of things
	// - We have to change the amount we lean by.
	// - Perlin dictates Wind, Wind dictates blade direction.
	// - Each tile gets it's own Wind Direction, so blades in that
	//   tile get this as its blade direction.
	// 
	// Have Perlin determine blade rotation angle.
	// 
	// Have that swaying look from the video
	// - It says you'll need a per-blade hash.
	//

	PerlinNoise2D pn;
	float noise = pn.eval(glm::vec2(glfwGetTime() * 0.35) + glm::vec2(m_bladeWorldPosition.x, m_bladeWorldPosition.z));
	float rot = pn.eval(glm::vec2(glfwGetTime()) * 0.05f + 0.05f * glm::vec2(m_bladeWorldPosition.x, m_bladeWorldPosition.z));
	rot = (rot + 1.0f) * std::_Pi_val;

	m_bladeDir = glm::vec3(glm::cos(rot), 0.0, glm::sin(rot));

	// A single blade of grass is a bezier curve, out of 3 points.
	glm::vec3 p0 = m_localPos;
	glm::vec3 p1 = m_localPos + glm::vec3(0.0f, m_bladeHeight, 0.0f);
	glm::vec3 p2 = p1 + m_bladeDir * (m_bladeLean + noise * 0.8f) * m_bladeHeight; // Think about bladeHeight here --> Taller blades, bend more

	makePersistentLength(p0, p1, p2, m_bladeHeight);

	// Calculate and apply width vectors
	glm::vec3 bladeSideDir = glm::normalize(glm::cross(m_bladeDir, YAXIS));
	glm::vec3 p0_neg = p0 - bladeSideDir * m_bladeP0Width;
	glm::vec3 p0_pos = p0 + bladeSideDir * m_bladeP0Width;
	glm::vec3 p1_neg = p1 - bladeSideDir * m_bladeP1Width;
	glm::vec3 p1_pos = p1 + bladeSideDir * m_bladeP1Width;
	glm::vec3 p2_neg = p2 - bladeSideDir * m_bladeP2Width;
	glm::vec3 p2_pos = p2 + bladeSideDir * m_bladeP1Width;

	for (int i = 0; i < NUM_BEZIER_VERTS; i++) {
		float t = m_tSeq[i];
		m_negBezier[i] = bezier(p0_neg, p1_neg, p2_neg, t);
		m_posBezier[i] = bezier(p0_pos, p1_pos, p2_pos, t);

		m_bladeHeight = glm::max(m_bladeHeight, m_posBezier[i].first[1]);
	}

	// CCW order
	for (int i = 0; i < NUM_BEZIER_VERTS - 1; i++) {
		// Neg tri
		m_vertices[6 * i] = m_negBezier[i].first;
		m_normals[6 * i] = glm::cross(bladeSideDir, m_negBezier[i].second);
		m_vertices[6 * i + 1] = m_posBezier[i].first;
		m_normals[6 * i + 1] = glm::cross(bladeSideDir, m_posBezier[i].second);
		m_vertices[6 * i + 2] = m_negBezier[i + 1].first;
		m_normals[6 * i + 2] = glm::cross(bladeSideDir, m_negBezier[i + 1].second);

		// Pos tri
		m_vertices[6 * i + 3] = m_posBezier[i].first;
		m_normals[6 * i + 3] = glm::cross(bladeSideDir, m_posBezier[i].second);
		m_vertices[6 * i + 4] = m_posBezier[i + 1].first;
		m_normals[6 * i + 4] = glm::cross(bladeSideDir, m_posBezier[i + 1].second);
		m_vertices[6 * i + 5] = m_negBezier[i + 1].first;
		m_normals[6 * i + 5] = glm::cross(bladeSideDir, m_negBezier[i + 1].second);
	}

	// We need the bounding quad for frustum culling.
	// Previously, I stored the vertices in a vector called boundinQuad, but
	// bc we want to vectorize this, we might as well just store them as
	// Structures of Arrays (SOA) beforehand.

	//m_x[0] = m_negBezier[0].first[0]; // First vertex in bounding quad
	//m_y[0] = m_negBezier[0].first[1];
	//m_z[0] = m_negBezier[0].first[2];

	//m_x[1] = m_posBezier[0].first[0]; // Second vertex in bounding quad
	//m_y[1] = m_posBezier[0].first[1];
	//m_z[1] = m_posBezier[0].first[2];

	//m_x[2] = m_negBezier[NUM_BEZIER_VERTS - 1].first[0]; // Third vertex in bounding quad
	//m_y[2] = m_negBezier[NUM_BEZIER_VERTS - 1].first[1];
	//m_z[2] = m_negBezier[NUM_BEZIER_VERTS - 1].first[2];

	//m_x[3] = m_posBezier[NUM_BEZIER_VERTS - 1].first[0]; // Third vertex in bounding quad
	//m_y[3] = m_posBezier[NUM_BEZIER_VERTS - 1].first[1];
	//m_z[3] = m_posBezier[NUM_BEZIER_VERTS - 1].first[2];

}