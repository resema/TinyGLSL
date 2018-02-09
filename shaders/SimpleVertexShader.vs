#version 330 core

// input vertex data, different for all executions of this shader
layout(location = 0) in vec3 vertexPosition_modelspace;
layout(location = 1) in vec3 vertexColor;

// values that stay constant for the whole mesh
uniform mat4 MVP;

// output data
out vec3 fragmentColor;

void main() 
{
    gl_Position = MVP * vec4(vertexPosition_modelspace, 1);

    fragmentColor = vertexColor;
}