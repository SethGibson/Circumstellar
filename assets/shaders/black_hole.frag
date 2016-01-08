#version 150
uniform sampler2D u_samplerRGB;
uniform sampler2D u_samplerMSK;
uniform float TexOffset;

in vec2 UV;

out vec4 FragColor;

void main()
{
	vec4 sampleA = texture2D(u_samplerRGB, vec2(UV.x,UV.y+TexOffset));
	vec4 sampleB = texture2D(u_samplerRGB, vec2(UV.x,UV.y+TexOffset*0.25f));
	vec4 sampleC = texture2D(u_samplerRGB, vec2(UV.x*2.0f,UV.y+TexOffset*1.25f));
	FragColor = sampleA*0.333f+sampleB*0.333f+sampleC*0.334f;
	FragColor.a = FragColor.r*texture2D(u_samplerMSK,UV).r;
}