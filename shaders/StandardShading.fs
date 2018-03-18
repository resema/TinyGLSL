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

    // normal of the computed fragment, in camera space
    vec3 n = normalize(Normal_cameraspace);

    // direction of the light (from the fragment to the light)
    vec3 l = normalize(LightDirection_cameraspace);

    // cosine of the angle between the normal and the light direction, clamped above 0
    //  - light is at the vertical of the triangle -> 1
    //  - light is perpendicalur to the triangle -> 0
    //  - light is behind the triangle -> 0
    float cosTheta = clamp(dot(n,l), 0, 1);     // clamp between 0 and 1

    // eye vector (towards the camera)
    vec3 E = normalize(EyeDirection_cameraspace);

    //direction in which the triangle reflects the light
    vec3 R = reflect(-l, n);

    // cosing of the angle between the Eye and the Reflect vector
    //  - looking into the reflection -> 1
    //  - looking elsewhere -> (< 1)
    float cosAlpha = clamp(dot(E,R), 0, 1);

    // final color
    color =
            // ambient : simulates indirect lighting 
            MaterialAmbientColor +
            // diffuse : "color" of the object
            MaterialDiffuseColor * LightColor * LightPower * cosTheta / pow(distance,2) +
            // specular : reflective highlight, like a mirror
            MaterialSpecularColor * LightColor * LightPower * pow(cosAlpha, 5) / pow(distance,2);

}