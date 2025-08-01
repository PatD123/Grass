#version 330 core

layout(location = 0) in vec3 aPos;
layout(location = 1) in mat4 aTransform; // Takes 1, 2, 3, 4

uniform mat4 model;
uniform mat4 proj_view;

void main()
{
    gl_Position = proj_view * aTransform * model * vec4(aPos, 1.0);
}