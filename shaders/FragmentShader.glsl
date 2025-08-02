#version 330 core

in vec3 Normal;

out vec4 FragColor;

void main()
{
    FragColor = vec4(0.0f, min(1.0 - Normal.y, 0.5), 0.0f, 1.0f);
}