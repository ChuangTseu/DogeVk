#version 450 core

in layout(location = 0) vec3 pos;

// out vec3 vertexColor;

void main() 
{
	// vertexColor = abs(pos.xyz);
	// //vertexColor = vec3(0);

	// if (gl_VertexIndex == 0)
	// {
	// 	gl_Position = vec4(-0.5, -0.5, 0.0, 1.0);

	// 	// R : -0.5 / G : 0.0 / B : 0.0
	// }
	// else if (gl_VertexIndex == 1) 
	// {
	// 	gl_Position = vec4(0.5, -0.5, 0.0, 1.0);
	// 	//vertexColor = vec3(0.0, 0.5, 0.0);

	// 	// R : -0.5 / G : -0.5 / B : -0.5
	// }
	// else
	// {
	// 	gl_Position = vec4(0.0, 0.5, 0.0, 1.0);

	// 	// R : -0.5 / G : 0.5 / B : -0.5
	// }

	gl_Position = vec4(pos.xyz, 1.0);
}
