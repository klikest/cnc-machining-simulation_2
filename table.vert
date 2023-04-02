#version 330 core

// Positions/Coordinates
layout (location = 0) in vec3 aPos;

// Normals (not necessarily normalized)
layout (location = 1) in vec3 aNormal;




// Outputs the normal for the Fragment Shader
out vec3 Normal;
// Outputs the current position for the Fragment Shader
out vec3 crntPos;

// Imports the camera matrix from the main function
uniform mat4 camMatrix;
// Imports the model matrix from the main function
uniform mat4 model_b;


void main()
{
	// calculates current position
	crntPos = vec3(model_b * vec4(aPos, 1.0f));
	// Outputs the positions/coordinates of all vertices
	gl_Position = camMatrix * vec4(crntPos, 1.0);

	// Assigns the normal from the Vertex Data to "Normal"
	Normal = aNormal;
}