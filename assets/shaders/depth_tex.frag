#version 150
uniform sampler2D u_SamplerDepth;

in vec2 UV;

out vec4 FragColor;

void main()
{
	FragColor = texture2D(u_SamplerDepth, UV);
}