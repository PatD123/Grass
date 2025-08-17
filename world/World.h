
#pragma once

#include <iostream>
#include <vector>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include "../tile/Tile.h"
#include "../shapes/Grass.h"
#include "../camera/Camera.h"
#include "../common/ShaderHelper.h"

const float TILE_MIN_HEIGHT = 0.5f;
const float TILE_MAX_HEIGHT = 1.0f;
const float GRASS_MAX_LEAN = 0.8f;

const int NUM_TILES_ROWS = 1;
const int NUM_TILES_COLS = 2;
const int BLADES_PER_TILE = 200;

class World {
public:
	World();

	void generateWorld();

	void animateGrass() {};

	void renderGrass(Camera& cam, ShaderHelper& sh, GLuint shaderProgram);

private:

	// Updates GPU buffers
	void updateGrassBladeDirGPU() {};

	// World attributes


	// Keeps track of all tiles in our world
	std::vector<Tile> m_tiles;

	// Wind heightmap

	// VBOs and VAOs
	GLuint grassVAO;
	GLuint grassVBO;
	GLuint grassNormVBO;
};
