#version 150

in float A;
out vec4 FragColor;

void main()
{
	float alph = mix(0.5f,1.0f, A);
	FragColor = vec4(1.0-A,0,A,alph);
}