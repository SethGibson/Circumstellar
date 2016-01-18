#version 150
in vec3		i_AngleRadii;	//x=Angle, y=radius init z=radius actual
in vec2		i_Speed; //x = Linear, y = angular
in vec2		i_Distance;	//x=initial, y=actual

out vec3	o_AngleRadii;	//x=Angle, y=radius init z=radius actual
out vec2	o_Distance;	//x=initial, y=actual
out vec3	o_DrawPos;
out float	o_Live;


void main()
{
	if (i_Distance.y > 0.5f)
	{
		o_Distance = i_Distance;
		o_Distance.y -= Speed.x;

		o_AngleRadii = i_AngleRadii;
		o_AngleRadii.x -= i_Speed.y;
		o_AngleRadii.z = mix(0.0f, i_AngleRadii.y, o_Distance.y / o_Distance.x);

		float x = sin(o_AngleRadii.x)*o_AngleRadii.z;
		float y = cos(o_AngleRadii.x)*o_AngleRadii.z;
		DrawPos = vec3(x, y, o_Distance.y);
	}

	else
		o_Live = 0.0;
}