#version 450 core

in layout(location = 0) vec3 pos;
in layout(location = 1) vec3 normal;
in layout(location = 2) vec2 texcoord;
in layout(location = 3) vec3 tangent;

out int instIdx;

out vec3 worldPos;
out vec3 worldNormal;
out vec2 worldTexcoord;
out vec3 worldTangent;


#include "global_descriptor_sets.h"

layout(push_constant) uniform BlockName {
    mat4 world;
} Push;

void main() 
{
	worldPos = (g_world[gl_InstanceIndex] * vec4(pos, 1.0)).xyz;
	worldNormal = normal; // TODO Handle rotate
	//worldTexcoord = vec2(0);
	//worldTangent = vec3(0); // TODO Handle rotate

	instIdx = gl_InstanceIndex;

	gl_Position = g_viewProj[0] * vec4(worldPos, 1.0);
	//gl_Position = g_viewProj[0] * Push.world * vec4(pos.xyz, 1.0);
}
