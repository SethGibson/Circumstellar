#version 150
uniform mat4 ciModelViewProjection;

in vec3 newPos;

void main()
{
	gl_Position = ciModelViewProjection*vec4(newPos,1.0);
}