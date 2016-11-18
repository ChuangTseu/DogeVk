#version 450 core

in layout(location = 0) vec3 pos;
in layout(location = 1) vec3 normal;

#include "global_descriptor_sets.h"

layout(push_constant) uniform BlockName {
    mat4 world;
} Push;

void main() 
{
	vec4 worldPos = g_world[gl_InstanceIndex] * vec4(pos - normal * 0.008, 1.0);

	gl_Position = g_lightViewProj[0] * worldPos;
}
