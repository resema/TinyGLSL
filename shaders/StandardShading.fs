#version 330 core

// interpolated values from the vertex shaders
in vec2 UV;
in vec3 Position_worldspace;
in vec3 Normal_cameraspace;
in vec3 EyeDirection_cameraspace;
in vec3 LightDirection_cameraspace;

// output data
out vec3 color;

// values that stay constant for the whole mesh
uniform sampler2D myTextureSampler;
uniform mat4 MV;
uniform vec3 LightPosition_worldspace;

void main()
{
    // light emission properties
    vec3 LightColor = vec3(1,1,1);
    float LightPower = 50.f;

    // material properties
    vec3 MaterialDiffuseColor = texture(myTextureSampler, UV).rgb;
    vec3 MaterialAmbientColor = vec3(0.1,0.1,0.1) * MaterialDiffuseColor;
    vec3 MaterialSpecularColor = vec3(0.3,0.3,0.3);

    // distance to the light
    float distance = length(LightPosition_worldspace - Position_worldspace);
}