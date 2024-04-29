#version 330

in vec3 fragPosition;
in vec2 fragTexCoord;
in vec3 fragNormal;

uniform sampler2D texture0;
uniform vec4 colDiffuse;

out vec4 finalColor;

void main() {
    vec3 lightDir = normalize(vec3(0.0, 0.0, 1.0) - fragPosition);
    float brightness = max(dot(fragNormal, lightDir), 0.0);
    finalColor = texture(texture0, fragTexCoord) * brightness;
    finalColor = vec4(finalColor.xyz,1.0);
}
