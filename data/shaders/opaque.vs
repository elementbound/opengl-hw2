#version 330

uniform mat4 uMVP;
uniform mat4 uWorldMatrix;
uniform mat4 uNormalMatrix;

in vec3 vertexPosition; 
in vec3 vertexNormal;
in vec2 vertexTexcoord;

out	vec3 fragPosition;
out vec3 fragNormal;
out vec2 fragTexcoord;

void main()
{
	gl_Position = uMVP * vec4(vertexPosition, 1.0);

	fragPosition = (uWorldMatrix * vec4(vertexPosition, 1.0)).xyz;
	fragNormal = (uNormalMatrix * vec4(vertexNormal, 1.0)).xyz;
	fragTexcoord = vertexTexcoord;
}