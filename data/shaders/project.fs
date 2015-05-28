#version 330

in vec3 fragTexcoord;

out vec4 outColor;

void main() 
{
	vec3 projected = fragTexcoord.xyz;
	if(projected.z > 0.0 &&
		projected.x > -1.0 && projected.x < 1.0 && 
		projected.y > -1.0 && projected.y < 1.0)
		outColor = vec4((1.0 + projected.xy)/2.0, 0.0, 1.0);//vec4(0.125, 0.875, 0.0, 1.0); //vec((1.0 + projected.xy)/2.0, 0.0, 1.0);
	else
		discard; //outColor = vec4(0.0);//vec4(0.875, 0.125, 0.0, 1.0); //vec4(projected.xy, 0.0, 1.0);
}