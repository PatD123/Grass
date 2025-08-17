#include "World.h"

World::World() {

    glGenBuffers(1, &grassVBO);
    glGenBuffers(1, &grassNormVBO);
    glGenVertexArrays(1, &grassVAO);

    glBindVertexArray(grassVAO);
    glBindBuffer(GL_ARRAY_BUFFER, grassVBO);
    glEnableVertexAttribArray(0);       // aPos
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GL_FLOAT), (void*)0);
    glBindBuffer(GL_ARRAY_BUFFER, grassNormVBO);
    glEnableVertexAttribArray(1);       // aNormal
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GL_FLOAT), (void*)0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void World::generateWorld() {

    for (int i = 0; i < NUM_TILES_ROWS; i++) {
        for (int j = 0; j < NUM_TILES_COLS; j++) {
            glm::vec3 tilePos = glm::vec3(i, 0.0, j);
            Tile t(BLADES_PER_TILE, tilePos, YAXIS, TILE_MIN_HEIGHT, TILE_MAX_HEIGHT, GRASS_MAX_LEAN);
            t.generateGrass();
            m_tiles.push_back(t);
        }
    }
}

void World::renderGrass(Camera& cam, ShaderHelper& sh, GLuint shaderProgram) {

    glm::mat4 view = cam.getViewMat();
    glm::mat4 proj = glm::perspective(FOV, ASPECT_RATIO, NEAR_PLANE, FAR_PLANE);
    glm::mat4 proj_view = proj * view;

    for (Tile& t : m_tiles) {
        t.renderGrass(
            cam,
            proj_view,
            sh,
            shaderProgram,
            grassVAO,
            grassVBO,
            grassNormVBO
        );
    }
}