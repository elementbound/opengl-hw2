#version 330

in vec3 fragTexcoord;
out vec4 outColor;

uniform sampler2D uProjectedTexture;
uniform sampler2D uDepthTexture;

void main() 
{
	vec3 projected = fragTexcoord.xyz;
	if(projected.z < 0.0 ||
		projected.x < -1.0 || projected.x > 1.0 || 
		projected.y < -1.0 || projected.y > 1.0)
		discard; 

	projected.xy = (1.0 + projected.xy)/2.0; 
	float depth = texture2D(uDepthTexture, projected.xy).x;
	float factor = 1.0f;
	if(depth - 0.0625 < projected.z)
		factor = 0.125f;

	outColor = texture2D(uProjectedTexture, projected.xy)*factor;
}