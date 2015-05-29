#version 330

uniform float depthBias;
uniform mat4 uMV;
uniform mat4 uProjection;

in vec3 vertexPosition; 

void main()
{
	vec4 eyePos = uMV * vec4(vertexPosition, 1.0);
		eyePos.z += depthBias;
	gl_Position = uProjection * eyePos;
}