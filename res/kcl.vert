#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec4 aColor;

out vec3 Normal;
out vec3 FragPos;
out vec4 ambientColor;

uniform mat4 model;

layout (std140) uniform Matrices
{
    mat4 projection;
    mat4 view;
    vec4 rot; // quaternion
    vec3 pos;
};

mat4 matFromPosAndQuat(vec3 position, vec4 quaternion) {
    // Normalize the quaternion to ensure it's valid
    quaternion = normalize(quaternion);

    float x = quaternion.x;
    float y = quaternion.y;
    float z = quaternion.z;
    float w = quaternion.w;

    // quat to mat
    mat3 rotation = mat3(
        1.0 - 2.0 * y * y - 2.0 * z * z, 2.0 * x * y - 2.0 * z * w,       2.0 * x * z + 2.0 * y * w,
        2.0 * x * y + 2.0 * z * w,       1.0 - 2.0 * x * x - 2.0 * z * z, 2.0 * y * z - 2.0 * x * w,
        2.0 * x * z - 2.0 * y * w,       2.0 * y * z + 2.0 * x * w,       1.0 - 2.0 * x * x - 2.0 * y * y
    );

    mat3 tf180 = mat3(
        -1, 0, 0,
        0, 1, 0,
        0, 0, -1
    );
    rotation = tf180 * rotation;
    mat4 viewMatrix = mat4(rotation);

    // Apply translation (inverse of the camera position)
    viewMatrix[3] = vec4(-rotation * position, 1.0);

    return viewMatrix;
}

void main()
{
    //gl_Position = transpose(projection) * transpose(view) * model * vec4(aPos, 1.0);
    FragPos = vec3(model * vec4(aPos, 1.0));
    Normal = mat3(transpose(inverse(model))) * aNormal;
    ambientColor = aColor;

    mat4 view2 = matFromPosAndQuat(pos, rot);

    gl_Position = projection * view2 * model * vec4(aPos, 1.0);
}

