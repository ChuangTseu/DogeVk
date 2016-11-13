#version 450 core

out layout(location = 0) vec4 color;

in vec3 vertexPos;
in vec3 vertexNormal;

// TODO : Use specialization constants for things like MAX_LIGHT_COUNT, MAX_VIEWPOINT_COUNT, ...

#include "global_descriptor_sets.h"

/*
layout (std140, set=1, binding=0) uniform paramsUniformBuffer {
	vec4 triangleColor;
};
*/

void main() 
{
	color = vec4(g_randRgbColor, 1);
	//color = vec4(normalize(vertexNormal), 1);

	//color = vec4(vertexPos, 1);
}
