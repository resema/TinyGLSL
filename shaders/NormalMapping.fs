#version 330 core

// interpolated values from the vertex shaders
in vec2 UV;
in vec3 Position_worldspace;
in vec3 EyeDirection_cameraspace;
in vec3 LightDirection_cameraspace;

in vec3 LightDirection_tangentspace;
in vec3 EyeDirection_tangentspace;

// output data
out vec3 color;

// values that stay constant for the whole mesh
uniform sampler2D DiffuseTextureSampler;
uniform sampler2D NormalTextureSampler;
uniform sampler2D SpecularTextureSampler;
uniform mat4 MV;
uniform mat3 MV3x3;
uniform vec3 LightPosition_worldspace;

void main()
{
    // light emission properties
    vec3 LightColor = vec3(1,1,1);
    float LightPower = 40.f;

    // material properties
    vec3 MaterialDiffuseColor = texture(DiffuseTextureSampler, UV).rgb;
    vec3 MaterialAmbientColor = vec3(0.1,0.1,0.1) * MaterialDiffuseColor;
    vec3 MaterialSpecularColor = texture(SpecularTextureSampler, UV).rgb * 0.3;

    // local normal, in tangentspace.
    //  V tex coord is inverted because normal map is in TGA for better quality
    vec3 TextureNormal_tangentspace = normalize(
        texture(NormalTextureSampler, vec2(UV.x, -UV.y)).rgb * 2.0 - 1.0
        );

    // distance to the light
    float distance = length(LightPosition_worldspace - Position_worldspace);

    // normal of the computed fragment, in tangent space
    vec3 n = normalize(TextureNormal_tangentspace);

    // direction of the light (from the fragment to the light)
    vec3 l = normalize(LightDirection_tangentspace);

    // cosine of the angle between the normal and the light direction, clamped above 0
    //  - light is at the vertical of the triangle -> 1
    //  - light is perpendicalur to the triangle -> 0
    //  - light is behind the triangle -> 0
    float cosTheta = clamp(dot(n,l), 0, 1);     // clamp between 0 and 1

    // eye vector (towards the camera)
    vec3 E = normalize(EyeDirection_tangentspace);

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