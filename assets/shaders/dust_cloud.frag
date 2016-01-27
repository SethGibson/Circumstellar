#version 150
uniform sampler2D u_SamplerRGB;

in float Alpha;
in vec2 UV;
in vec3 Tint;

out vec4 FragColor;

void main()
{
	vec4 samp = texture(u_SamplerRGB, UV);
	FragColor = vec4(samp.rgb*vec3(1,Alpha,Alpha)*Tint,samp.a*Alpha);
}