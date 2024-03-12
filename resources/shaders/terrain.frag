#version 330

in vec3 fragPosition;

out vec4 finalColor;

void main()
{
    finalColor = vec4( 0.0, fragPosition.y, 0.0, 1.0 );
}
