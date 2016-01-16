#version 150

in vec4 oldPos;

out vec4 newPos;

void main()
{
	newPos = oldPos;
	newPos.y+=1;
}