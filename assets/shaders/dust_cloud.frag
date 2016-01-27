#version 150
uniform sampler2D u_SamplerRGB;

in vec2 UV;
in float Alpha;

out vec4 FragColor;

void main()
{
	vec4 samp = texture(u_SamplerRGB, UV);
	FragColor = vec4(samp.rgb,samp.a*Alpha);
}