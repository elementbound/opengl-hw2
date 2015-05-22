#version 330

in	vec3 fragPosition;
in	vec3 fragNormal;
in  vec2 fragTexcoord;

out vec4 outColor;

uniform vec3 uLightPos;
uniform vec3 uLightColor;
uniform sampler2D uTexture;

void main()
{
	vec3 diffuseColor = texture(uTexture, fragTexcoord).rgb;

	//

	vec3 normal = normalize(fragNormal);
	vec3 delta = uLightPos - fragPosition;
	float dst = length(delta);
	delta /= dst;

	float lightF = max(dot(normal, delta), 0.0f) * (30.0f / pow(1.0f + dst, 2.0));

	vec3 diffuseContrib = uLightColor * lightF;

	//

	outColor = vec4(diffuseColor * diffuseContrib, 1.0);
}