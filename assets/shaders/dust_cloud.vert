#version 150
uniform mat4 ciModelViewProjection;

in vec4 ciPosition;
in vec2 ciTexCoord0;

in vec3 i_Position;
in float i_Size;
in float i_Alpha;

out vec2 UV;
out float Alpha;

void main()
{
	vec3 vPos = (ciPosition.xyz*i_Size)+i_Position;
	UV = ciTexCoord0;
	Alpha = i_Alpha;
	gl_Position = ciModelViewProjection*vec4(vPos,1.0);
}