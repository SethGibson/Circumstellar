#version 150
in vec3 InPos;
in float InVel;

out vec3 OutPos;
out float OutVel;

void main()
{
	OutVel = InVel;
	OutPos = vec3(InPos.x,InPos.y-OutVel,InPos.z);
}