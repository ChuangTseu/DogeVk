#version 450 core

out layout(location = 0) vec4 color;

// in vec3 vertexColor;

layout (set=0, binding=0) uniform paramsUniformBuffer {
	vec4 triangleColor;
};

void main() 
{
	color = vec4(1.0, 0.0, 0.0, 1.0);
	//color = vec4(vertexColor, 1.0);
	color = triangleColor;
}
