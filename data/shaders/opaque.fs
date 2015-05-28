#version 330

in	vec3 fragPosition;
in	vec3 fragNormal;
in  vec2 fragTexcoord;

out vec4 outColor;

uniform vec3 uLightPos;
uniform vec3 uLightColor;
uniform float uLightRange;

uniform vec3 uViewPos;

uniform sampler2D uDiffuseTexture;
uniform sampler2D uSpecularTexture;
uniform sampler2D uAOTexture;

void main()
{
	vec3 diffuseColor = texture(uDiffuseTexture, fragTexcoord).rgb;
	vec3 specularColor = texture(uSpecularTexture, fragTexcoord).rgb;
	vec3 aoColor = texture(uAOTexture, fragTexcoord).rgb;

	//

	vec3 normal = normalize(fragNormal);
	vec3 delta = uLightPos - fragPosition;

	//

	float dst = length(delta);
	vec3 delta_dir = delta/dst;

	float lightF = pow(uLightRange, 2.0) / (pow(uLightRange, 2.0) + pow(dst, 2.0));
	lightF *= max(dot(normal, delta_dir), 0.0f);

	vec3 diffuseContrib = uLightColor * lightF;

	//

	vec3 halfVec = normalize((uViewPos - fragPosition) + delta_dir);
	lightF = max(dot(halfVec, normal), 0.0);
	lightF = pow(lightF, 50.0f);
	vec3 specularContrib = uLightColor * lightF;

	//

	outColor = vec4(diffuseColor*diffuseContrib*aoColor + specularColor*specularContrib, 1.0);
}