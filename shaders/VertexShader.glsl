#version 330 core

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;

out vec3 Normal;
out float RelativeHeight;

uniform mat4 Transform;
uniform mat4 WindDir;
uniform float BladeHeight;
uniform float Time;

void main()
{
    vec3 pos = aPos;
    RelativeHeight = aPos.y / BladeHeight;

    pos.x += 0.005 * sin(2.0 * Time + RelativeHeight * 3.12);

    gl_Position = Transform * WindDir * vec4(pos, 1.0);
    Normal = aNormal;
}