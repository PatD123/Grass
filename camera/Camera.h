#pragma once

#include <iostream>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "../shapes/Frustum.h"

const static float FOV = glm::radians(45.0f);
const static float ASPECT_RATIO = 800.0f / 600.0f;
const static float NEAR_PLANE = 0.01f;
const static float FAR_PLANE = 20.0f;

enum Camera_Movement {
	FORWARD,
	BACKWARD,
	LEFT,
	RIGHT
};

// Default camera values
const float YAW = -90.0f;
const float PITCH = 0.0f;
const float SPEED = 1.5f;
const float SENSITIVITY = 0.1f;
const float ZOOM = 45.0f;

class Camera {
public:

	glm::vec3 m_pos;
	glm::vec3 m_front;
	glm::vec3 m_right;
	glm::vec3 m_up;
	glm::vec3 m_worldUp;

	// Each camera has its own frustum
	std::unique_ptr<Frustum> m_frustum;
	
	float m_yaw = YAW;
	float m_pitch = PITCH;
	float m_camSpeed = SPEED;
	float m_mouseSensitivity = SENSITIVITY;

	Camera(glm::vec3 pos, float aspectRatio, float fov, float zNear, float zFar);

	void setPosition(glm::vec3 pos);

	void processMovement(GLFWwindow* window, float deltaTime);

	void updateCameraVectors();

	glm::mat4 getViewMat();

	void processMouseMovement(float xoffset, float yoffset, GLboolean constrainPitch = true);
};