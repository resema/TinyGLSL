#version 330 core

// input vertex data, different for all executions of this shader
layout(location = 0) in vec3 vertexPosition_modelspace;
layout(location = 1) in vec2 vertexUV;
layout(location = 2) in vec3 vertexNormal_modelspace;
layout(location = 3) in vec3 vertexTangent_modelspace;
layout(location = 4) in vec3 vertexBitangent_modelspace;

// output data ; will be interpolated for each fragment
out vec2 UV;
out vec3 Position_worldspace;
out vec3 LightDirection_tangentspace;
out vec3 EyeDirection_tangentspace;

// values that stay constant for the whole mesh
uniform mat4 MVP;
uniform mat4 V;
uniform mat4 M;
uniform mat3 MV3x3;
uniform vec3 LightPosition_worldspace;

void main()
{
    // output position of the vertex, in clip space : MVP * position
    gl_Position = MVP * vec4(vertexPosition_modelspace, 1);

    // position of the vertex, in worldspace : M * position
    Position_worldspace = (M * vec4(vertexPosition_modelspace, 1)).xyz;

    // vector that goes from the vertex to the camera, in camera space
    //  in camera space, the camera is at the origin (0,0,0)
    vec3 vertexPosition_modelspace = (V * M * vec4(vertexPosition_modelspace, 1)).xyz;
    vec3 EyeDirection_cameraspace = vec3(0,0,0) - vertexPosition_modelspace;

    // vector that goes from the vertex to the light, in camera space.
    //  M is ommited because it's identity
    vec3 LightPosition_cameraspace = (V * vec4(LightPosition_worldspace, 1)).xyz;
    vec3 LightDirection_cameraspace = LightPosition_cameraspace + EyeDirection_cameraspace;

    // UV of the vertex. no special space for this one
    UV = vertexUV;

    // model to camera = ModelView
    vec3 vertexTangent_cameraspace = MV3x3 * vertexTangent_modelspace;
    vec3 vertexBitangent_cameraspace = MV3x3 * vertexBitangent_modelspace;
    vec3 vertexNormal_cameraspace = MV3x3 * vertexNormal_modelspace;

    // You can use dot products instead of building this matrix and transposing it. See References for details.
    mat3 TBN = transpose(mat3(
        vertexTangent_cameraspace,
        vertexBitangent_cameraspace,
        vertexNormal_cameraspace
    ));

    LightDirection_tangentspace = TBN * LightDirection_cameraspace;
    EyeDirection_tangentspace = TBN * EyeDirection_cameraspace;
}