#version 330

in vec3 fragTexcoord;

out vec4 outColor;

void main() 
{
	if(fragTexcoord.z > 0.0 &&
		clamp(fragTexcoord.x, -1.0, 1.0) == fragTexcoord.x && 
		clamp(fragTexcoord.y, -1.0, 1.0) == fragTexcoord.y)
		outColor = vec4((1.0 + fragTexcoord.xy) / 2.0, 0.0, 1.0);
	else
		outColor = vec4((1.0 + fragTexcoord.xy) / 4.0, 0.0, 1.0);
}