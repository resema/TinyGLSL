#version 330 core

// interpolated values from the vertex shader
in vec2 UV;

// output data
out vec3 color;

// values that staz constant for the whole mesh
uniform sampler2D myTextureSampler;

void main() 
{
    // output color = color of the texures at the specified UV
    color = texture(myTextureSampler, UV).rgb;
}