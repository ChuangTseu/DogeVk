#version 450 core

out layout(location = 0) vec4 fragColor;

in vec3 vertexPos;
in vec3 vertexNormal;
in flat int instIdx;

// TODO : Use specialization constants for things like MAX_LIGHT_COUNT, MAX_VIEWPOINT_COUNT, ...

#include "global_descriptor_sets.h"

float shininess = 10.f;

vec3 blinn_phong_calc_internal(vec3 lightDir, vec3 lightColor, vec3 normal) {
    float Id = clamp(dot(normal, lightDir), 0, 1);

    vec3 viewDir = normalize(g_eyePosition - vertexPos);
    vec3 halfV = normalize(lightDir + viewDir);

    float Is = 0;
    if (Id > 0) {
        Is = pow(clamp(dot(normal, halfV), 0, 1), shininess);
    }

    return Is * lightColor*Id;
}

vec3 blinn_phong_calc(DirLight light, vec3 normal) {
    vec3 lightDir = normalize(-light.direction);

    return blinn_phong_calc_internal(lightDir, light.color, normal);
}

void main() 
{
	fragColor = vec4(g_globalColor, 1);

    vec3 worldNormal = normalize(vertexNormal);

    vec3 finalColor = vec3(0);

    if (instIdx == 0) finalColor = vec3(1,0,0);
    if (instIdx == 1) finalColor = vec3(0,1,0);
    if (instIdx == 2) finalColor = vec3(0,0,1);
    if (instIdx == 3) finalColor = vec3(1,1,0);
    if (instIdx == 4) finalColor = vec3(0,1,1);
    if (instIdx == 5) finalColor = vec3(1,0,1);
    if (instIdx == 6) finalColor = vec3(1,1,1);
    if (instIdx == 7) finalColor = vec3(0,0,0);

    for (int nDirLight = 0; nDirLight < g_numDirLights; ++nDirLight)
    {
        finalColor += blinn_phong_calc(g_dirLights[nDirLight], worldNormal);
    }

    finalColor += vec3(0.05, 0.05, 0); // FAKE AMBIANT

    fragColor = vec4(finalColor, 1);
}
