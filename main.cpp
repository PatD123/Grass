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

// Timing
float PROGRAM_START_TIME = glfwGetTime();
float deltaTime = 0.0f;	// Time between current frame and last frame
float lastFrame = 0.0f; // Time of last frame

void framebufferSizeCallback(GLFWwindow* window, int width, int height);
void mouseCallback(GLFWwindow* window, double xpos, double ypos);
void processInput(GLFWwindow* window);

// Making Camera
Camera cam(glm::vec3(0.0f, 0.0f, 0.0f));
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
    glm::vec3 grassPatchPos = glm::vec3(0.0f, 0.0f, 1.0f);
    glm::vec3 Normal = YAXIS;
    float grassPatchMaxHeight = 0.5f;
    float grassPatchRadius = 0.2f;

    /*Grass g;
    g.generateBlade(grassPatchPos, YAXIS, grassPatchMaxHeight, grassPatchRadius);*/

    std::vector<Grass> world;
    for (int i = 0; i < 5; i++) {
        Grass g;
        g.generateBlade(grassPatchPos, YAXIS, grassPatchMaxHeight, grassPatchRadius);
        world.push_back(g);
    }

    std::vector<glm::vec3> vertices; // Reserve space so no reallocations?
    std::vector<glm::mat4> trnsfms;
    for (Grass g : world) {
        for (glm::vec3 v : g.m_vertices)
            vertices.push_back(v);
        trnsfms.push_back(g.m_transform);
    }

    GLuint vertVBO, trnsfmVBO, VAO;
    glGenBuffers(1, &vertVBO);
    glGenBuffers(1, &trnsfmVBO);
    glGenVertexArrays(1, &VAO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, vertVBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), vertices.data(), GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);       // aPos
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GL_FLOAT), (void*)0);

    glBindBuffer(GL_ARRAY_BUFFER, trnsfmVBO);
    glBufferData(GL_ARRAY_BUFFER, trnsfms.size() * sizeof(glm::mat4), trnsfms.data(), GL_STATIC_DRAW);
    for (int i = 0; i < 4; i++) {
        glEnableVertexAttribArray(1 + i);       // aTransform
        glVertexAttribPointer(1 + i, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(i * sizeof(glm::vec4)));
        glVertexAttribDivisor(1 + i, 1);
    }

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

    // Culling back faces
    //glEnable(GL_CULL_FACE);

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

        float near_plane = 1.0f, far_plane = 600.0f;
        glm::mat4 model = glm::mat4(1.0f);
        glm::mat4 view = cam.getViewMat();
        glm::mat4 proj = glm::perspective(glm::radians(45.0f), 800.0f / 600.0f, near_plane, far_plane);
        glm::mat4 proj_view = proj * view;

        sh.setUniformMat4fv(shaderProgram, "model", glm::value_ptr(model));
        sh.setUniformMat4fv(shaderProgram, "proj_view", glm::value_ptr(proj_view));

        // Draw
        sh.useShaderProgram(shaderProgram);
        glBindVertexArray(VAO);
        glDrawArraysInstanced(GL_TRIANGLES, 0, 18, world.size());

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
