#version 150
uniform sampler2D u_samplerRGBA;
uniform float TexOffset;
uniform vec4 u_Intens;
uniform vec3 u_Scales;
uniform vec3 u_Speeds;

uniform vec3 u_Color1;
uniform vec3 u_Color2;
uniform vec3 u_Color3;

in vec2 UV;

out vec4 FragColor;

void main()
{
	vec4 sampleA = texture2D(u_samplerRGBA, vec2(UV.x*u_Scales.x,UV.y+TexOffset*u_Speeds.x))*vec4(u_Color1,1.0);
	vec4 sampleB = texture2D(u_samplerRGBA, vec2(UV.x*u_Scales.y,UV.y+TexOffset*u_Speeds.y))*vec4(u_Color2,1.0);
	vec4 sampleC = texture2D(u_samplerRGBA, vec2(UV.x*u_Scales.z,UV.y+TexOffset*u_Speeds.z))*vec4(u_Color3,1.0);
	//FragColor = sampleA*u_Intens.x+sampleB*u_Intens.y+sampleC*u_Intens.z;
	//FragColor.a = FragColor.r*texture2D(u_samplerRGBA,UV).a*u_Intens.a;

	FragColor = sampleA*u_Intens.x+sampleB*u_Intens.y+sampleC*u_Intens.z;
	FragColor.a = FragColor.r*texture2D(u_samplerRGBA,UV).a*u_Intens.a;
}