#version 420 core
in vec3 Normal;
in vec3 FragPos;
in vec3 ambientColor;

out vec4 FragColor;

struct DirLight {
    vec3 direction;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

layout (std140, binding = 1) uniform ViewPos {
    vec3 viewPos;
};
layout(std140, binding = 2) uniform DirLightBlock {
    DirLight dirLight;
};

vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir)
{
    vec3 lightDir = normalize(-light.direction);
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // combine results
    vec3 ambient  = light.ambient  * ambientColor;
    vec3 diffuse  = light.diffuse  * diff * ambientColor;
    return (ambient + diffuse);
}

void main()
{
    // properties
    vec3 norm = normalize(Normal);
    vec3 viewDir = normalize(viewPos - FragPos);

    // phase 1: Directional lighting
    vec3 result = CalcDirLight(dirLight, norm, viewDir);
    // phase 2: Point lights
    //for(int i = 0; i < NR_POINT_LIGHTS; i++)
    //    result += CalcPointLight(pointLights[i], norm, FragPos, viewDir);
    // phase 3: Spot light
    //result += CalcSpotLight(spotLight, norm, FragPos, viewDir);

    FragColor = vec4(result, 1.0);
}
