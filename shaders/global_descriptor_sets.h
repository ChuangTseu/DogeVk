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

#define MAX_MODEL_WORLDS 4

layout( std140, set = 2, binding = 0 ) uniform PerObjectCB {
   	mat4 g_world[MAX_MODEL_WORLDS];
};

layout( std140, set = 3, binding = 0 ) uniform PerMaterialCB {
	vec3 g_ka;
	vec3 g_kd;
	vec3 g_ks;
	float g_shininess;
};

#define MAX_SHADER_LIGHTS 8

struct DirLight {
    vec3 direction;
    vec3 color;
};

layout( std140, set = 4, binding = 0 ) uniform LightsCB {
   	DirLight g_dirLights[MAX_SHADER_LIGHTS];
   	mat4 g_lightViewProj[MAX_SHADER_LIGHTS];
   	int g_numDirLights;
};

layout( std140, set = 5, binding = 0 ) uniform AppConfigCB {
   	vec3 g_globalColor;
};

