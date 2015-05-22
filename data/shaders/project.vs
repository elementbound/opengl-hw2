#version 330

uniform mat4 uMVP;
uniform mat4 uWorldMatrix;
uniform mat4 uMatImage;

in vec3 vertexPosition;

out vec3 fragTexcoord;

void main() 
{
	gl_Position = uMVP * vec4(vertexPosition, 1.0);

	fragTexcoord = (uMatImage * (uWorldMatrix * vec4(vertexPosition, 1.0))).xyz; 
}