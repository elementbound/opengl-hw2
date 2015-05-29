#version 330

in  float fragDepth;
out float outColor;

void main()
{
	outColor = fragDepth;
}