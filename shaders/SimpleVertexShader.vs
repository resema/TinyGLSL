#version 330 core

// input vertex data, different for all executions of this shader
layout(location = 0) in vec3 vertexPosition_modelspace;

// values that stay constant for the whole mesh
uniform mat4 MVP;

void main() 
{
    gl_Position = MVP * vec4(vertexPosition_modelspace, 1);
}