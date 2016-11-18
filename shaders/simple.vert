#version 450 core

in layout(location = 0) vec3 pos;
in layout(location = 1) vec3 normal;

out vec3 worldPos;
out vec3 worldNormal;
out int instIdx;

#include "global_descriptor_sets.h"

layout(push_constant) uniform BlockName {
    mat4 world;
} Push;

void main() 
{
	worldPos = (g_world[gl_InstanceIndex] * vec4(pos, 1.0)).xyz;
	worldNormal = normal; // TODO Handle rotate

	instIdx = gl_InstanceIndex;

	gl_Position = g_viewProj[0] * vec4(worldPos, 1.0);
	//gl_Position = g_viewProj[0] * Push.world * vec4(pos.xyz, 1.0);
}
