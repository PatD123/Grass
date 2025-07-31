#include <iostream>
#include <stdlib.h>
#include <cmath>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "common/ShaderHelper.h"
#include "camera/Camera.h"
#include "shapes/Cube.h"

// Timing
float PROGRAM_START_TIME = glfwGetTime();
float deltaTime = 0.0f;	// Time between current frame and last frame
float lastFrame = 0.0f; // Time of last frame

void framebufferSizeCallback(GLFWwindow* window, int width, int height);
void mouseCallback(GLFWwindow* window, double xpos, double ypos);

glm::vec3 bezier(glm::vec3 p0, glm::vec3 p1, glm::vec3 p2, float t);
glm::vec3 bezierDerivative(const glm::vec3& p0, const glm::vec3& p1, const glm::vec3& p2, float t);

void printVector(const glm::vec3& v);

void processInput(GLFWwindow* window);

// Making Camera
Camera cam(glm::vec3(0.0f, 0.0f, 0.0f));
const float radius = 5.0f;
bool firstMouse = true;
float lastX = 800.0f / 2.0;
float lastY = 600.0 / 2.0;

// CONSTS
const glm::vec3 YAXIS = glm::vec3(0.0, 1.0, 0.0);
const int NUM_BEZIER_VERTS = 4; // Vertices per side of bezier.

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
    glm::vec3 bladePos = glm::vec3(0.0f, 0.0f, 0.0f);
    glm::vec3 bladeDir = glm::vec3(1.0f, 0.0f, 0.0f);
    float bladeHeight = 0.5f;
    float bladeLean = 0.3;
    float bladeP0Width = 0.08;
    float bladeP1Width = 0.05;
    float bladeP2Width = 0.025;

    // A single blade of grass is a bezier curve, out of 3 points.
    glm::vec3 p0 = bladePos;
    glm::vec3 p1 = bladePos + glm::vec3(0.0f, bladeHeight, 0.0f);
    glm::vec3 p2 = p1 + bladeDir * bladeLean; // Think about bladeHeight here --> Taller blades, bend more

    // Calculate and apply width vectors
    glm::vec3 bladeSideDir = glm::normalize(glm::cross(YAXIS, bladeDir));
    glm::vec3 p0_neg = p0 - bladeSideDir * bladeP0Width;
    glm::vec3 p0_pos = p0 + bladeSideDir * bladeP0Width;
    glm::vec3 p1_neg = p1 - bladeSideDir * bladeP1Width;
    glm::vec3 p1_pos = p1 + bladeSideDir * bladeP1Width;
    glm::vec3 p2_neg = p2 - bladeSideDir * bladeP2Width;
    glm::vec3 p2_pos = p2 + bladeSideDir * bladeP1Width;

    // We want 4 vertices per bezier --> 8 in total
    // Left side = bezier(t) for t 0..4
    // Right side = bezier(t) for t 0..4
    // Make sure winding order is right ==> Should have 6 triangles.

    float tSeq[NUM_BEZIER_VERTS] = { 0.0f, 1.0f / 3.0, 2.0f / 3.0, 1.0f };
    glm::vec3 negBezier[NUM_BEZIER_VERTS];
    glm::vec3 posBezier[NUM_BEZIER_VERTS];

    for (int i = 0; i < NUM_BEZIER_VERTS; i++) {
        float t = tSeq[i];
        negBezier[i] = bezier(p0_neg, p1_neg, p2_neg, t);
        posBezier[i] = bezier(p0_pos, p1_pos, p2_pos, t);
    }

    /*for (int i = 0; i < NUM_BEZIER_VERTS; i++) {
        printVector(posBezier[i]);
    }*/

    std::vector<glm::vec3> vertices;

    // CCW order
    for (int i = 0; i < NUM_BEZIER_VERTS - 1; i++) {
        // Neg tri
        vertices.push_back(negBezier[i]);
        vertices.push_back(posBezier[i]);
        vertices.push_back(negBezier[i + 1]);

        // Pos tri
        vertices.push_back(posBezier[i]);
        vertices.push_back(negBezier[i + 1]);
        vertices.push_back(posBezier[i + 1]);

        std::cout << vertices.size() << std::endl;
    }

    GLuint VBO, VAO;
    glGenBuffers(1, &VBO);
    glGenVertexArrays(1, &VAO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), vertices.data(), GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GL_FLOAT), (void*)0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    //GLuint VBO, VAO;
    //glGenBuffers(1, &VBO);
    //glGenVertexArrays(1, &VAO);

    //glBindVertexArray(VAO);
    //glBindBuffer(GL_ARRAY_BUFFER, VBO);

    //glBufferData(GL_ARRAY_BUFFER, sizeof(Cube::vertices), Cube::vertices, GL_STATIC_DRAW);
    //glEnableVertexAttribArray(0);
    //glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GL_FLOAT), (void*)0);
    //glEnableVertexAttribArray(1);               // aNormal
    //glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GL_FLOAT), (void*)(3 * sizeof(GL_FLOAT)));

    //glBindBuffer(GL_ARRAY_BUFFER, 0);
    //glBindVertexArray(0);

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
        glDrawArrays(GL_TRIANGLES, 0, vertices.size());

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

glm::vec3 bezier(glm::vec3 p0, glm::vec3 p1, glm::vec3 p2, float t) {
    glm::vec3 a = glm::mix(p0, p1, t);
    glm::vec3 b = glm::mix(p1, p2, t);
    return glm::mix(a, b, t);
}

glm::vec3 bezierDerivative(const glm::vec3& p0, const glm::vec3& p1, const glm::vec3& p2, float t)
{
    return 2.0f * (1.0f - t) * (p1 - p0) + 2.0f * t * (p2 - p1);
}

void printVector(const glm::vec3& v) {
    std::cout << v[0] << " " << v[1] << " " << v[2] << std::endl;
}