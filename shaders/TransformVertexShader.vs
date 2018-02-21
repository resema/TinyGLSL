#version 330 core

// input vertex data, different for all executions of this hader
layout(location = 0) in vec3 vertexPosition_modelspace;
layout(location = 1) in vec2 vertexUV;

// output data ; will be interpolated for each fragment
out vec2 UV;

// values that stay constant for the whole mesh
uniform mat4 MVP;

void main() 
{
    // output position of the vertex, in clip space : MVP * position
    gl_Position = MVP * vec4(vertexPosition_modelspace, 1);

    // UV of the vertex. No special space for BMP
    // UV = vertexUV;

    // UV of the vertex. DirectX world : V texture coord is inversed compared to OPENL.
    vec2 invVertexUV = vertexUV;
    invVertexUV.y = (1.f - vertexUV.y);

    UV = invVertexUV;
}