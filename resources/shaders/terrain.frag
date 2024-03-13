#version 330

in float fragHeight;

out vec4 fragColor;

uniform float yellow_threshold;
uniform float white_threshold;

void main()
{
    // You can define your own color gradient based on the height value
    vec3 color = vec3(0.0, 0.0, 1.0); // Default color

    // Example: Set color based on height
    if (fragHeight < yellow_threshold) {
        color = vec3(0.0, 0.5, 0.0); // Green for lower heights
    } else if (fragHeight < white_threshold) {
        color = vec3(0.5, 0.5, 0.0); // Yellow for medium heights
    } else {
        color = vec3(1.0, 1.0, 1.0); // White for higher heights
    }

    fragColor = vec4(color, 1.0);
}

