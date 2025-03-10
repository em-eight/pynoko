#version 330 core
in vec3 Normal;
in vec3 FragPos;
in vec4 ambientColor;

out vec4 FragColor;

void main()
{
    vec3 objectColor = ambientColor.rgb;

    vec3 norm = normalize(Normal);
    //vec3 lightDir = normalize(lightPos - FragPos);
    // yes, hardcoded light params. Fight me
    vec3 lightDir = normalize(vec3(0.2, 0.8, 0.2));
    vec3 lightColor = vec3(1.0, 1.0, 1.0);

    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * lightColor;

    vec3 result = (0.2 * ambientColor.rgb + 0.8 * diffuse) * objectColor;
    FragColor = vec4(result, 1.0);
}
