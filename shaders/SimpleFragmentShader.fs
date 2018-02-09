#version 330 core

// input data
in vec3 fragmentColor;

// output data
out vec3 color;

void main()
{
    color = fragmentColor;
}