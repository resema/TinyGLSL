#version 330 core

// interpolated values from the vertex shader
in vec2 UV;

// output data
out vec4 color;

// values that stay constant for the whole mesh
uniform sampler2D myTextureSampler;

void main()
{
    color = texture(myTextureSampler, UV);
}