#version 330

in vec3 vertexPosition;

out vec4 ProjTexCoord;

uniform mat4 ProjectorMatrix;
uniform mat4 ModelMatrix;
uniform mat4 MVP;

void main()
{
    vec4 pos4 = vec4(vertexPosition,1.0);

    ProjTexCoord = ProjectorMatrix * (ModelMatrix * pos4);
    gl_Position = MVP * pos4;
}
