#version 330 core

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTexCoord;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform mat3 normalMatrix;

out vec3 fragPos;
out vec3 fragNormal;
out vec2 fragTexCoord;

void main() {
    vec4 worldPos = model * vec4(aPos, 1.0);
    fragPos      = worldPos.xyz;
    fragNormal   = normalMatrix * aNormal;
    fragTexCoord = aTexCoord;
    gl_Position  = projection * view * worldPos;
}
