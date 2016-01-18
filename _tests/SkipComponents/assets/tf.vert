#version 150
in vec2 i_Polars;
in float i_Speed;

out vec2 o_Polars;
out vec2 o_Pos;

void main()
{
	o_Polars = i_Polars;
	o_Polars.x-= i_Speed;

	o_Pos.x = cos(o_Polars.x)*o_Polars.y;
	o_Pos.y = sin(o_Polars.x)*o_Polars.y;
}