#include "Grass.h";

static std::mt19937 GENERATOR(getSeed());

Grass::Grass(glm::vec3 localPos) : m_localPos(localPos) {}

void Grass::generateBlade(
	const glm::vec3& grassPatchPos,
	const glm::vec3& grassPatchNorm,
	const float grassPatchMinHeight,
	const float grassPatchMaxHeight,
	const float grassPatchMaxLean,
	const float grassPatchRadius
) {

	// Get new grass pos;
	std::uniform_real_distribution<> dis(-0.5, 0.5);
	glm::vec3 bladeWorldPosition = glm::vec3(dis(GENERATOR), 0.0f, dis(GENERATOR)) + grassPatchPos;
	m_transform = glm::translate(glm::mat4(), bladeWorldPosition);

	// Some random values for our grass at the moment.
	dis = std::uniform_real_distribution<>(-1.0, 1.0);
	glm::vec3 bladeDir = glm::normalize(glm::vec3(dis(GENERATOR), 0.0f, dis(GENERATOR)));
	dis = std::uniform_real_distribution<>(grassPatchMinHeight, grassPatchMaxHeight);
	float bladeHeight = dis(GENERATOR);
	//dis = std::uniform_real_distribution<>(0.0, grassPatchMaxLean);
	float bladeLean = 0.2;
	float bladeP0Width = 0.08;
	float bladeP1Width = 0.05;
	float bladeP2Width = 0.0;

	// A single blade of grass is a bezier curve, out of 3 points.
	glm::vec3 p0 = m_localPos;
	glm::vec3 p1 = m_localPos + glm::vec3(0.0f, bladeHeight, 0.0f);
	glm::vec3 p2 = p1 + bladeDir * bladeLean * bladeHeight; // Think about bladeHeight here --> Taller blades, bend more

	// Calculate and apply width vectors
	glm::vec3 bladeSideDir = glm::normalize(glm::cross(bladeDir, YAXIS));
	glm::vec3 p0_neg = p0 - bladeSideDir * bladeP0Width;
	glm::vec3 p0_pos = p0 + bladeSideDir * bladeP0Width;
	glm::vec3 p1_neg = p1 - bladeSideDir * bladeP1Width;
	glm::vec3 p1_pos = p1 + bladeSideDir * bladeP1Width;
	glm::vec3 p2_neg = p2 - bladeSideDir * bladeP2Width;
	glm::vec3 p2_pos = p2 + bladeSideDir * bladeP1Width;

	std::vector<float> tSeq;
	std::pair<glm::vec3, glm::vec3> negBezier[NUM_BEZIER_VERTS];
	std::pair<glm::vec3, glm::vec3> posBezier[NUM_BEZIER_VERTS];

	for (int i = 0; i <= NUM_BEZIER_VERTS - 1; i++)
		tSeq.push_back(static_cast<float>(i) / static_cast<float>(NUM_BEZIER_VERTS - 1));

	for (int i = 0; i < NUM_BEZIER_VERTS; i++) {
		float t = tSeq[i];
		negBezier[i] = bezier(p0_neg, p1_neg, p2_neg, t);
		posBezier[i] = bezier(p0_pos, p1_pos, p2_pos, t);

		m_bladeHeight = glm::max(m_bladeHeight, posBezier[i].first[1]);
	}

	// CCW order
	for (int i = 0; i < NUM_BEZIER_VERTS - 1; i++) {
		// Neg tri
		m_vertices.push_back(negBezier[i].first);
		m_normals.push_back(glm::cross(bladeSideDir, negBezier[i].second));
		m_vertices.push_back(posBezier[i].first);
		m_normals.push_back(glm::cross(bladeSideDir, posBezier[i].second));
		m_vertices.push_back(negBezier[i + 1].first);
		m_normals.push_back(glm::cross(bladeSideDir, negBezier[i + 1].second));

		// Pos tri
		m_vertices.push_back(posBezier[i].first);
		m_normals.push_back(glm::cross(bladeSideDir, posBezier[i].second));
		m_vertices.push_back(posBezier[i + 1].first);
		m_normals.push_back(glm::cross(bladeSideDir, posBezier[i + 1].second));
		m_vertices.push_back(negBezier[i + 1].first);
		m_normals.push_back(glm::cross(bladeSideDir, negBezier[i + 1].second));

		//std::cout << m_vertices.size() << std::endl;
	}
}