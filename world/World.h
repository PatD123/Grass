
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
const float TILE_MAX_HEIGHT = 1.5f;
const float GRASS_MAX_LEAN = 0.8f;

const int NUM_TILES_ROWS = 20;
const int NUM_TILES_COLS = 20;
const int BLADES_PER_TILE = 200;

class World {
public:
	World();

	void generateWorld(ShaderHelper& sh, GLuint shaderProgram);

	void renderGrass(Camera& cam, ShaderHelper& sh, GLuint shaderProgram);

private:

	// Updates GPU buffers
	void moveTileToGPU(const Tile& t);

	// World attributes


	// Keeps track of all tiles in our world
	std::vector<Tile> m_tiles;

	// Wind heightmap

	// VBOs and VAOs
	GLuint GrassVAO;				// VAO for everything
	GLuint GrassVBO;				// VBO that holds base instance pos for blades
	GLuint GrassNormVBO;			// VBO that holds base instance norm for blades
	GLuint GrassTransformVBO;		// VBO that holds per-instance transforms (NOT BLADE DIR) for blades
	GLuint GrassBladeDirVBO;		// VBO that holds per-instance blade directions
	GLuint GrassBladeScalingVBO;		// VBO that holds per-instance scale transfomrs for blades
};
