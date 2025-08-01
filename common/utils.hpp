#include <iostream>
#include <stdlib.h>
#include <random>
#include <cmath>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

static glm::vec3 bezier(glm::vec3 p0, glm::vec3 p1, glm::vec3 p2, float t) {
    glm::vec3 a = glm::mix(p0, p1, t);
    glm::vec3 b = glm::mix(p1, p2, t);
    return glm::mix(a, b, t);
}

static glm::vec3 bezierDerivative(const glm::vec3& p0, const glm::vec3& p1, const glm::vec3& p2, float t)
{
    return 2.0f * (1.0f - t) * (p1 - p0) + 2.0f * t * (p2 - p1);
}

static void printVector(const glm::vec3& v) {
    std::cout << v[0] << " " << v[1] << " " << v[2] << std::endl;
}