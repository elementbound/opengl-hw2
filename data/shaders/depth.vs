#version 330

uniform mat4 uMVP;

in vec3 vertexPosition; 
in vec2 vertexTexcoord;

out float fragDepth;

void main()
{
	gl_Position = uMVP * vec4(vertexPosition, 1.0);
	fragDepth = gl_Position.z;
}