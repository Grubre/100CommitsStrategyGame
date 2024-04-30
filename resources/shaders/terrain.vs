#version 330

// Input vertex attributes
in vec3 vertexPosition;
in vec2 vertexTexCoord;
in vec3 vertexNormal;
in vec4 vertexColor;

// Input uniform values
uniform mat4 mvp;
uniform mat4 matModel;
uniform mat4 matNormal;

out float fragHeight;

// NOTE: Add here your custom variables

void main()
{
    fragHeight = vertexPosition.y;
    // Calculate final vertex position
    gl_Position = mvp*vec4(vertexPosition, 1.0);
}
