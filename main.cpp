#include <iostream>
#include <stdlib.h>
#include <random>
#include <cmath>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "common/ShaderHelper.h"
#include "common/utils.hpp"
#include "camera/Camera.h"
#include "shapes/Cube.h"
#include "shapes/Grass.h"
#include "shapes/Frustum.h"
#include "tile/Tile.h"

// Timing
float PROGRAM_START_TIME = glfwGetTime();
float deltaTime = 0.0f;	// Time between current frame and last frame
float lastFrame = 0.0f; // Time of last frame

void framebufferSizeCallback(GLFWwindow* window, int width, int height);
void mouseCallback(GLFWwindow* window, double xpos, double ypos);
void processInput(GLFWwindow* window);

// Making Camera
const float FOV = glm::radians(45.0f);
const float ASPECT_RATIO = 800.0f / 600.0f;
const float NEAR_PLANE = 0.1f;
const float FAR_PLANE = 25.0f;
Camera cam(
    glm::vec3(0.0f, 0.0f, 0.0f),
    FOV,
    ASPECT_RATIO,
    NEAR_PLANE,
    FAR_PLANE
);
const float radius = 5.0f;
bool firstMouse = true;
float lastX = 800.0f / 2.0;
float lastY = 600.0 / 2.0;

int main()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(800, 600, "LearnOpenGL", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    glViewport(0, 0, 800, 600);

    glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);
    glfwSetCursorPosCallback(window, mouseCallback);

    // Compile and link shaders
    ShaderHelper sh;
    GLuint shaderProgram = sh.compileShaders("shaders/VertexShader.glsl", "shaders/FragmentShader.glsl");

    // Enable z-buffer depth testing
    glEnable(GL_DEPTH_TEST);

    // Capture cursor
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    // Let's start our grass.
    
    // Grass patch
    glm::vec3 grassPatchPos = glm::vec3(0.0f, 0.0f, 0.5f);
    glm::vec3 Normal = YAXIS;
    float grassPatchMinHeight = 0.5f;
    float grassPatchMaxHeight = 1.0f;
    float grassPatchMaxLean = 0.8f;
    float grassPatchRadius = 0.5f;

    const int NUM_TILES_ROWS = 20;
    const int NUM_TILES_COLS = 20;
    const int BLADES_PER_TILE = 50;

    std::vector<Tile> world;
    for (int i = 0; i < NUM_TILES_ROWS; i++) {
        for (int j = 0; j < NUM_TILES_COLS; j++) {
            glm::vec3 tilePos = glm::vec3(i, 0.0, j);
            Tile t(BLADES_PER_TILE, tilePos, YAXIS, grassPatchMinHeight, grassPatchMaxHeight, grassPatchMaxLean, grassPatchRadius);
            t.generateGrass();
            world.push_back(t);
        }
    }

    GLuint vertVBO, normVBO, VAO;
    glGenBuffers(1, &vertVBO);
    glGenBuffers(1, &normVBO);
    glGenVertexArrays(1, &VAO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, vertVBO);
    glEnableVertexAttribArray(0);       // aPos
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GL_FLOAT), (void*)0);
    glBindBuffer(GL_ARRAY_BUFFER, normVBO);
    glEnableVertexAttribArray(1);       // aNormal
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GL_FLOAT), (void*)0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    // FPS metrics
    double prevTime = 0.0;
    double currTime = 0.0;
    double timeDiff;
    unsigned int frameCounter = 0;

    // Enable V-sync for v-blanks
    glfwSwapInterval(1);

    std::cout << "Starting to render" << std::endl;

    while (!glfwWindowShouldClose(window))
    {
        // we are now around 60 fps, 16-17 ms per frame.
        currTime = glfwGetTime();
        timeDiff = currTime - prevTime;
        frameCounter++;
        if (timeDiff >= 1.0 / 30.0) {
            std::string fps = std::to_string((1.0 / timeDiff) * frameCounter);
            std::string ms = std::to_string((timeDiff / frameCounter) * 1000);
            std::string windowtitle = "fps: " + fps + " | ms: " + ms;
            glfwSetWindowTitle(window, windowtitle.c_str());
            prevTime = currTime;
            frameCounter = 0;
        }

        float currentframe = (float)glfwGetTime();
        deltaTime = currentframe - lastFrame;
        lastFrame = currentframe;

        cam.processMovement(window, deltaTime);
        processInput(window);

        // clear colors on screen so we start at fresh slate
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        float near_plane = 0.1f, far_plane = 100.0f;
        //glm::mat4 model = glm::mat4(1.0f); // Redudant technically
        glm::mat4 view = cam.getViewMat();
        glm::mat4 proj = glm::perspective(FOV, ASPECT_RATIO, NEAR_PLANE, FAR_PLANE);
        glm::mat4 proj_view = proj * view;

        for (Tile& t : world) {
            t.renderGrass(
                cam,
                proj_view,
                sh,
                shaderProgram,
                VAO,
                vertVBO, 
                normVBO
            );
        }
        

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    return 0;
}

void framebufferSizeCallback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

void mouseCallback(GLFWwindow* window, double dXPos, double dYPos)
{
    float xpos = static_cast<float>(dXPos);
    float ypos = static_cast<float>(dYPos);

    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos;

    cam.processMouseMovement(xoffset, yoffset);

    lastX = xpos;
    lastY = ypos;
}
