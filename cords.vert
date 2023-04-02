#version 330 core

// Positions/Coordinates
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor;

// Outputs the current position for the Fragment Shader
out vec3 crntPos;

out vec3 color;

// Imports the camera matrix from the main function
uniform mat4 camMatrix;
// Imports the model matrix from the main function
uniform mat4 model_cords;


void main()
{
	// calculates current position
	crntPos = vec3(model_cords * vec4(aPos, 1.0f));
	// Outputs the positions/coordinates of all vertices
	gl_Position = camMatrix * vec4(crntPos, 1.0);

	color = aColor;
}