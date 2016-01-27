#version 150
uniform mat4 ciModelViewProjection;

in vec3 o_Pos;

void main()
{
	gl_Position = ciModelViewProjection*vec4(o_Pos,1.0);
}