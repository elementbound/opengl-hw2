#version 330

in vec4 ProjTexCoord;

uniform sampler2D ProjectorTex;
uniform sampler2D ShadowMap;

out vec4 FragColor;

void main() {
    vec4 projTexColor = vec4(0.0);
    if( ProjTexCoord.z > 0.0)
        projTexColor = textureProj( ProjectorTex, ProjTexCoord ) * vec4(textureProj( ShadowMap, ProjTexCoord ).xxx, 1.0);

    FragColor = projTexColor;
}
