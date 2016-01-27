#version 150
#define M_PI 3.1415926535897932384626433832795

uniform mat4 ciModelViewProjection;

in vec2 o_Pos;
out float A;
void main()
{
	gl_Position = ciModelViewProjection*vec4(o_Pos,0,1);
	//gl_PointSize = (atan(o_Pos.y, o_Pos.x)+1)*0.5+1;
	A = (atan(o_Pos.y, o_Pos.x)+1.0f)*0.5f;
	//gl_PointSize = 2.0;
}