#version 150
uniform mat4 ciModelViewProjection;

in vec4 ciPosition;
in vec2 ciTexCoord0;

out float Z;
out vec2 UV;

void main()
{
	Z = ciPosition.z;
	UV = ciTexCoord0;
	gl_Position = ciModelViewProjection*ciPosition;
}