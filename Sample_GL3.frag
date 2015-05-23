#version 330 core

// Interpolated values from the vertex shaders
in vec3 fragColor;

// output data
out vec3 color;
 
void main()
{
	// Output color = color specified in the vertex shader,
	// interpolated between all 3 surrounding vertices of the triangle
	//fragColor = vec4(0.0f, 1.0f, 1.0f, 1.0f);
	color = fragColor;
	//color = vec3(1.0f, 1.0f, 1.0f);
	//printf ("%d\n",fragcolor);
}
