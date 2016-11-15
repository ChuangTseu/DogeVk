#version 450 core

in layout(location = 0) vec3 pos;
in layout(location = 1) vec3 normal;

out vec3 vertexPos;
out vec3 vertexNormal;
out int instIdx;

#include "global_descriptor_sets.h"

layout(push_constant) uniform BlockName {
    mat4 world;
} Push;

void main() 
{
	vertexPos = pos;
	vertexNormal = normal;

	instIdx = gl_InstanceIndex;

	gl_Position = g_viewProj[0] * g_world[gl_InstanceIndex] * vec4(pos.xyz, 1.0);
	//gl_Position = g_viewProj[0] * Push.world * vec4(pos.xyz, 1.0);
}
