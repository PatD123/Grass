#include "Plane.h"

Plane::Plane() {}

Plane::Plane(glm::vec3 pos, glm::vec3 norm)
	: m_normPos(pos), m_norm(norm)
{}
