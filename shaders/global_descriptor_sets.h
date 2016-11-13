#pragma once

#define MAX_NUM_VIEWPOINTPLANES 8

layout( std140, set = 0, binding = 0 ) uniform PerFrameCB {
	vec3 g_randRgbColor;
   	float g_time;
};

layout( std140, set = 1, binding = 0 ) uniform PerViewPointCB {
   	mat4 g_view[MAX_NUM_VIEWPOINTPLANES];
	mat4 g_proj[MAX_NUM_VIEWPOINTPLANES];
	mat4 g_viewProj[MAX_NUM_VIEWPOINTPLANES];

	vec3 g_eyePosition; // TODO : EXTRACT FROM VIEW
};

layout( std140, set = 2, binding = 0 ) uniform PerObjectCB {
   	mat4 g_world;
};

layout( std140, set = 5, binding = 0 ) uniform PerMaterialCB {
	vec3 g_ka;
	vec3 g_kd;
	vec3 g_ks;
	float g_shininess;
};

layout( std140, set = 6, binding = 0 ) uniform PerFboCB {
   	vec4 g_FboDimensions[16]; // x : Width, y : Height, z : 1 / Width, w : 1 / Height
};

#define MAX_SHADER_LIGHTS 16

struct DirLight {
    vec3 direction;
    vec3 color;
};

layout( std140, set = 7, binding = 0 ) uniform LightsCB {
   	DirLight g_dirLights[MAX_SHADER_LIGHTS];
   	int g_numDirLights;
};
