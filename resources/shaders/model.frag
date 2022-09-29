#version 450
layout (location = 0) out vec4 FragColor;

in VS_OUT {
    vec2 TexCoords;
    vec3 TangentLightPos;
    vec3 TangentViewPos;
    vec3 TangentFragPos;
} fs_in;

uniform sampler2D texture_diffuse1;
uniform sampler2D texture_specular1;
uniform sampler2D texture_normal1;

uniform vec3 u_lightColor = vec3(1, 1, 1);
uniform float u_lightPower = 50.0;

// LoadShaderFile uses this marker to insert some preprocessor defines to tailor this shader to a
// specific use-case.
//{USE_DEFINES}

void main()
{
#ifdef SKIP_NORMAL
    vec3 normal = vec3(0, 0, 1);
#else
    // obtain normal from normal map in range [0,1]
    vec3 normal = texture(texture_normal1, fs_in.TexCoords).rgb;
    // transform normal vector to range [-1,1]
    normal = normalize(normal * 2.0 - 1.0);  // this normal is in tangent space
#endif
    // get diffuse color
#ifdef SKIP_DIFFUSE
    vec3 color = vec3(1, 1, 1);
#else
    vec3 color = texture(texture_diffuse1, fs_in.TexCoords).rgb;
#endif
    // ambient
    vec3 ambient = 0.1 * color;
    // diffuse
    vec3 lightDir = normalize(fs_in.TangentLightPos - fs_in.TangentFragPos);
    float diff = max(dot(lightDir, normal), 0.0);
    vec3 diffuse = diff * color;
    // specular
    vec3 specularColor = texture(texture_specular1, fs_in.TexCoords).rgb;
    vec3 viewDir = normalize(fs_in.TangentViewPos - fs_in.TangentFragPos);
    // vec3 reflectDir = reflect(-lightDir, normal);
    vec3 halfwayDir = normalize(lightDir + viewDir);  
    float spec = pow(max(dot(normal, halfwayDir), 0.0), 32.0);

#ifdef SKIP_SPECULAR
    vec3 specular = vec3(0.2) * spec;
#else
    vec3 specular = specularColor * spec;
#endif
    FragColor = vec4(ambient + diffuse + specular, 1.0);
}
