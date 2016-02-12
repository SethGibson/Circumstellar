#version 150
uniform sampler2D u_SamplerRGB;

in vec2 UV;
in vec4 Tint;

out vec4 FragColor;

void main()
{
	vec4 samp = texture(u_SamplerRGB, UV);
	FragColor = vec4(samp.rgb*vec3(1,Tint.a,Tint.a)*Tint.rgb,samp.a*Tint.a);
}