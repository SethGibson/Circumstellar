#version 150
uniform mat4 ciModelViewProjection;

in vec4 ciPosition;
in vec2 ciTexCoord0;

in vec3 i_Position;
in vec4 i_Tint;
in float i_Size;

out vec2 UV;
out vec4 Tint;

void main()
{
	vec3 vPos = (ciPosition.xyz*i_Size)+i_Position;
	UV = ciTexCoord0;
	Tint = i_Tint;
	gl_Position = ciModelViewProjection*vec4(vPos,1.0);
}