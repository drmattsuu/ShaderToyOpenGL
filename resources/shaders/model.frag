#version 430
layout (location = 0) out vec4 FragColor;

in VS_OUT {
    vec3 FragPos;
    vec2 TexCoords;
    vec3 CamNormal;
    vec3 CamEyeDir;
    vec3 CamLightDir;
} vs_out;

uniform sampler2D texture_diffuse1;
uniform sampler2D texture_specular1;
uniform sampler2D texture_normal1;

uniform vec3 u_lightPos;
uniform vec3 u_lightColor = vec3(1, 1, 1);
uniform float u_lightPower = 50.0;

void main()
{
    // Material properties
    vec4 materialDiffuseColor = texture(texture_diffuse1, vs_out.TexCoords);
    vec4 materialAmbientColor = materialDiffuseColor * vec4(0.1, 0.1, 0.1, 1.0);
    vec4 materialSpecularColor = texture(texture_specular1, vs_out.TexCoords);

    // Distance to the light
    float distance = length(u_lightPos - vs_out.FragPos);

    // Normal of the computed fragment, in camera space
    vec3 n = normalize(vs_out.CamNormal);
    // Direction of the light (from the fragment to the light)
    vec3 l = normalize(vs_out.CamLightDir);
    // Cosine of the angle between the normal and the light direction, 
    // clamped above 0
    //  - light is at the vertical of the triangle -> 1
    //  - light is perpendicular to the triangle -> 0
    //  - light is behind the triangle -> 0
    float cosTheta = clamp(dot(n, l), 0, 1);
    
    // Eye vector (towards the camera)
    vec3 E = normalize(vs_out.CamEyeDir);
    // Direction in which the triangle reflects the light
    vec3 R = reflect(-l, n);
    // Cosine of the angle between the Eye vector and the Reflect vector,
    // clamped to 0
    //  - Looking into the reflection -> 1
    //  - Looking elsewhere -> < 1
    float cosAlpha = clamp(dot(E, R), 0, 1);

    vec4 diffuse = materialDiffuseColor * vec4(u_lightColor, 1) * u_lightPower * cosTheta / (distance * distance);
    vec4 specular = materialSpecularColor * vec4(u_lightColor, 1) * u_lightPower * pow(cosAlpha, 5) / (distance * distance);
    FragColor = materialAmbientColor + diffuse + specular;
}
