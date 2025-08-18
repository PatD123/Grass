#pragma once

#include <iostream>
#include <stdlib.h>
#include <random>
#include <cmath>
#include <chrono>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

static glm::vec3 bezierDerivative(const glm::vec3& p0, const glm::vec3& p1, const glm::vec3& p2, float t)
{
    return glm::normalize(2.0f * (1.0f - t) * (p1 - p0) + 2.0f * t * (p2 - p1));
}

static std::pair<glm::vec3, glm::vec3> bezier(glm::vec3& p0, glm::vec3& p1, glm::vec3& p2, float t) {
    glm::vec3 a = glm::mix(p0, p1, t);
    glm::vec3 b = glm::mix(p1, p2, t);
    return std::pair<glm::vec3, glm::vec3>(glm::mix(a, b, t), bezierDerivative(p0, p1, p2, t));
}

static void printVector(const glm::vec3& v) {
    std::cout << v[0] << " " << v[1] << " " << v[2] << std::endl;
}

static void printMatrix(const glm::mat4& m) {
    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 4; ++j) {
            std::cout << m[i][j] << " ";
        }
        std::cout << std::endl;
    }
}

static unsigned getSeed() {
    return static_cast<unsigned> (std::chrono::system_clock::now().time_since_epoch().count());
}

static void transformQuad(__m128 dest[4], const __m128 x, const __m128 y, const __m128 z, const glm::mat4& transform)
{
    for (size_t i = 0; i < 4; ++i) {
        __m128 res = _mm_broadcast_ss(&transform[3][i]);
        res = _mm_fmadd_ps(_mm_broadcast_ss(&transform[0][i]), x, res);
        res = _mm_fmadd_ps(_mm_broadcast_ss(&transform[1][i]), y, res);
        res = _mm_fmadd_ps(_mm_broadcast_ss(&transform[2][i]), z, res);
        dest[i] = res;
    }
}

static void makePersistentLength(glm::vec3& v0, glm::vec3& v1, glm::vec3& v2, float height)
{
    //Persistent length
    glm::vec3 v01 = v1 - v0;
    glm::vec3 v12 = v2 - v1;
    float lv01 = glm::length(v01);
    float lv12 = glm::length(v12);

    float L1 = lv01 + lv12;
    float L0 = glm::length(v2 - v0);
    float L = (2.0f * L0 + L1) / 3.0f; //http://steve.hollasch.net/cgindex/curves/cbezarclen.html

    float ldiff = height / L;
    v01 = v01 * ldiff;
    v12 = v12 * ldiff;
    v1 = v0 + v01;
    v2 = v1 + v12;
}