#version 330 core

in vec3 fragPos;
in vec3 fragNormal;
in vec2 fragTexCoord;

uniform vec3  lightPos;
uniform vec3  lightColor;
uniform vec3  ambientColor;
uniform vec3  viewPos;

uniform vec3  diffuseColor;
uniform float shininess;

uniform bool      useTexture;
uniform sampler2D diffuseTexture;

out vec4 fragColor;

void main() {
    vec3 norm     = normalize(fragNormal);
    vec3 lightDir = normalize(lightPos - fragPos);
    vec3 viewDir  = normalize(viewPos - fragPos);
    vec3 halfDir  = normalize(lightDir + viewDir);

    // Ambient
    vec3 ambient = ambientColor * diffuseColor;

    // Diffuse
    float diff   = max(dot(norm, lightDir), 0.0);
    vec3  diffuse = diff * lightColor * diffuseColor;

    // Specular (Blinn-Phong)
    float spec     = pow(max(dot(norm, halfDir), 0.0), shininess);
    vec3  specular = spec * lightColor * 0.4;

    vec3 result = ambient + diffuse + specular;

    if (useTexture) {
        vec4 texColor = texture(diffuseTexture, fragTexCoord);
        vec3 ambTex = ambientColor * texColor.rgb;
        vec3 difTex = diff * lightColor * texColor.rgb;
        result = ambTex + difTex + specular;
    }

    fragColor = vec4(result, 1.0);
}
