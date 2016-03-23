#version 450 core

out layout(location = 0) vec4 color;

in vec3 vertexPos;

layout (set=1, binding=0) uniform paramsUniformBuffer {
	vec4 triangleColor;
};

void main() 
{
	color = triangleColor;

	//color = vec4(vertexPos, 1);
}
