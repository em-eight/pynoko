#version 420 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec4 aColor;
out vec3 ambientColor;

out vec3 Normal;
out vec3 FragPos;

uniform mat4 model;

layout (binding = 0, std140) uniform Matrices
{
    mat4 projection;
    mat4 view;
};

void main()
{
    gl_Position = transpose(projection) * transpose(view) * model * vec4(aPos, 1.0);
    FragPos = vec3(model * vec4(aPos, 1.0));
    Normal = mat3(transpose(inverse(model))) * aNormal;
    ambientColor = aColor.rgb;
}
