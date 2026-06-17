#version 420 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec4 aColor;

layout (location = 0) out vec3 Normal;
layout (location = 1) out vec3 FragPos;
layout (location = 2) out vec3 ambientColor;

layout(push_constant) uniform PushConstants {
    mat4 model;
} pc;

layout (set = 0, binding = 0, std140) uniform Matrices
{
    mat4 projection;
    mat4 view;
} ubo;

void main()
{
    gl_Position = transpose(ubo.projection) * transpose(ubo.view) * pc.model * vec4(aPos, 1.0);
    FragPos = vec3(pc.model * vec4(aPos, 1.0));
    Normal = mat3(transpose(inverse(pc.model))) * aNormal;
    ambientColor = aColor.rgb;
}
