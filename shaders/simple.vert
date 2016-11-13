#version 450 core

in layout(location = 0) vec3 pos;
in layout(location = 1) vec3 normal;

out vec3 vertexPos;
out vec3 vertexNormal;

#include "global_descriptor_sets.h"

/*
layout (set=0, binding=0) uniform Transform {
	mat4 perspective;
};
*/

void main() 
{
	vertexPos = pos;
	vertexNormal = normal;

	gl_Position = g_viewProj[0] * vec4(pos.xyz, 1.0);
	//gl_Position = vec4(pos.xyz, 1.0);
}
