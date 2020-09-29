#version 330 core
layout (location = 0) in vec2 triangleVertices;		
layout (location = 1) in vec3 triangleColor;	

layout (location = 2) in vec2 squareVertices;		
layout (location = 3) in vec3 squareColor;	

out vec3 vColor;
uniform int object;
uniform mat4 transform;		//the transformation matrix

void main()
{
	//for object type 1, apply transformation (rotation)
	if (object == 1) {
		gl_Position = transform * vec4 (triangleVertices, 0.0, 1.0);
		vColor = triangleColor;	
	}

	//object type 2 no need to apply transformation
	else { // square don't rotate
		gl_Position = vec4 (squareVertices, 0.0, 1.0);
		vColor = squareColor;
	}
}