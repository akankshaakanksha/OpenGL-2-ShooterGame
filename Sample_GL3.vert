#version 330 core

// input data : sent from main program
layout (location = 0) in vec3 vertexPosition;
layout (location = 1) in vec3 vertexColor;

uniform mat4 MVP;

// output data : used by fragment shader
out vec3 fragColor;

void main ()
{
	vec4 v = vec4(vertexPosition, 1); // Transform an homogeneous 4D vector

	// The color of each vertex will be interpolated
	// to produce the color of each fragment
	fragColor = vertexColor;
	//fragColor = vertexColor * vec3(0.0f, 0.0f, 0.0f);
	//fragColor =  vec3(0.0f, 1.0f, 0.0f);
	//fragColor = vec3(0.0f, 1.0f, 1.0f);
	// Output position of the vertex, in clip space : MVP * position
	gl_Position = MVP * v;

}
