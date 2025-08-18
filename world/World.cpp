#include "World.h"

World::World() {

    glGenBuffers(1, &GrassVBO);
    glGenBuffers(1, &GrassNormVBO);
    glGenBuffers(1, &GrassTransformVBO);
    glGenBuffers(1, &GrassBladeDirVBO);
    glGenBuffers(1, &GrassBladeScalingVBO);
    glGenVertexArrays(1, &GrassVAO);

    int posLocation = 0;
    int normLocation = 1;
    int transformLocation = 2;
    int bladeDirLocation = 6;
    int bladeScalingLocation = 10;

    glBindVertexArray(GrassVAO);
    glBindBuffer(GL_ARRAY_BUFFER, GrassVBO);
    glEnableVertexAttribArray(posLocation);       // aPos
    glVertexAttribPointer(posLocation, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GL_FLOAT), (void*)0);
    glBindBuffer(GL_ARRAY_BUFFER, GrassNormVBO);
    glEnableVertexAttribArray(normLocation);       // aNormal
    glVertexAttribPointer(normLocation, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GL_FLOAT), (void*)0);
    glBindBuffer(GL_ARRAY_BUFFER, GrassTransformVBO);
    for (int i = 0; i < 4; i++) {
        glEnableVertexAttribArray(transformLocation + i);       // aTransform
        glVertexAttribPointer(transformLocation + i, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(i * sizeof(glm::vec4)));
        glVertexAttribDivisor(transformLocation + i, 1);
    }
    glBindBuffer(GL_ARRAY_BUFFER, GrassBladeDirVBO);
    for (int i = 0; i < 4; i++) {
        glEnableVertexAttribArray(bladeDirLocation + i);       // aBladeDir
        glVertexAttribPointer(bladeDirLocation + i, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(i * sizeof(glm::vec4)));
        glVertexAttribDivisor(bladeDirLocation + i, 1);
    }
    glBindBuffer(GL_ARRAY_BUFFER, GrassBladeScalingVBO);
    glEnableVertexAttribArray(bladeScalingLocation);       // aBladeScaling
    glVertexAttribPointer(bladeScalingLocation, 1, GL_FLOAT, GL_FALSE, sizeof(GL_FLOAT), (void*)0);
    glVertexAttribDivisor(bladeScalingLocation, 1);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void World::generateWorld(ShaderHelper& sh, GLuint shaderProgram) {

    for (int i = 0; i < NUM_TILES_ROWS; i++) {
        for (int j = 0; j < NUM_TILES_COLS; j++) {
            glm::vec3 tilePos = glm::vec3(i, 0.0, j);
            Tile t(BLADES_PER_TILE, tilePos, YAXIS, TILE_MIN_HEIGHT, TILE_MAX_HEIGHT, GRASS_MAX_LEAN);
            t.generateGrass();
            m_tiles.push_back(t);
        }
    }

    // Load base instance of a blade of grass
    Grass& g = m_tiles[0].m_blades[0];
    glBindBuffer(GL_ARRAY_BUFFER, GrassVBO);
    glBufferData(GL_ARRAY_BUFFER, g.m_vertices.size() * sizeof(glm::vec3), g.m_vertices.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, GrassNormVBO);
    glBufferData(GL_ARRAY_BUFFER, g.m_normals.size() * sizeof(glm::vec3), g.m_normals.data(), GL_STATIC_DRAW);
    sh.setUniform1f(shaderProgram, "BladeHeight", g.m_bladeHeight);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void World::renderGrass(Camera& cam, ShaderHelper& sh, GLuint shaderProgram) {

    glm::mat4 view = cam.getViewMat();
    glm::mat4 proj = glm::perspective(FOV, ASPECT_RATIO, NEAR_PLANE, FAR_PLANE);
    glm::mat4 proj_view = proj * view;

    for (Tile& t : m_tiles) {

        t.animateGrass();

        moveTileToGPU(t);

        t.renderGrass(
            cam,
            proj_view,
            sh,
            shaderProgram,
            GrassVAO
        );
    }
}

void World::moveTileToGPU(const Tile& t) {
    // Move all transforms into the VBO beforehand
    std::vector<glm::mat4> transforms;
    std::vector<glm::mat4> bladeDirs;
    std::vector<float> bladeScalings;
    for (const Grass& g : t.m_blades) {
        transforms.push_back(g.m_transform);
        bladeDirs.push_back(g.m_bladeDir);
        bladeScalings.push_back(g.m_bladeScaling);
    }

    glBindBuffer(GL_ARRAY_BUFFER, GrassTransformVBO);
    glBufferData(GL_ARRAY_BUFFER, transforms.size() * sizeof(glm::mat4), transforms.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, GrassBladeDirVBO);
    glBufferData(GL_ARRAY_BUFFER, bladeDirs.size() * sizeof(glm::mat4), bladeDirs.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, GrassBladeScalingVBO);
    glBufferData(GL_ARRAY_BUFFER, bladeScalings.size() * sizeof(glm::vec3), bladeScalings.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}