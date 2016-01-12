#version 150
uniform sampler2D u_Sampler;
uniform float u_Max;

in float Z;
in vec2 UV;

out vec4 FragColor;

void main()
{
	FragColor = texture2D(u_Sampler, UV);
	FragColor.a *= Z/u_Max;
}