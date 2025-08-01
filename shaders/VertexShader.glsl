#version 330 core

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;

out vec3 Normal;

uniform mat4 Transform;

void main()
{
    gl_Position = Transform * vec4(aPos, 1.0);
    Normal = aNormal;
}