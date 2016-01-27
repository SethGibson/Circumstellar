#version 150 core
uniform vec2 u_Distance; //x = distance initial/max y=cutoff distance

in vec3 i_Pos;
in vec2 i_Polars;
in vec2 i_Speeds;
in vec2 i_Age;

out vec3 o_Pos;
out vec2 o_Polars;
out vec2 o_Age;

void main()
{
	o_Age = i_Age;
	o_Polars = i_Polars;
	o_Pos = i_Pos;

	if(o_Age.x>0)
		o_Age.x-=1;
	else
	{
		o_Pos.z -= i_Speeds.x;
		if(o_Pos.z<u_Distance.y)
			o_Age.y = 0;
		else
		{
			float zRatio = o_Pos.z/u_Distance.x;
			float radius_0 = o_Polars.y*zRatio;
			
			o_Polars.x -= i_Speeds.y;
			o_Pos.x = cos(o_Polars.x)*radius_0;
			o_Pos.y = sin(o_Polars.x)*radius_0;
		}
	}
}