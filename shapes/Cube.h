#pragma once

#include <vector>

#include <glm/glm.hpp>

class Cube {
public:

    // All cubes have these vertices
    static const float vertices[216];

    glm::mat4 m_transform;
    glm::vec3 m_color;
    float m_phase;
    bool m_isWater;

    Cube(glm::mat4 transform, glm::vec3 color, float phase, bool isWater);

    virtual void updateTransform(const glm::mat4& trsnfm);
};