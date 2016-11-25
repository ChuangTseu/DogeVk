#version 450 core

out layout(location = 0) vec4 fragColor;

in flat int instIdx;

in vec3 worldPos;
in vec3 worldNormal;
in vec2 worldTexcoord;
in vec3 worldTangent;


// TODO : Use specialization constants for things like MAX_LIGHT_COUNT, MAX_VIEWPOINT_COUNT, ...

#include "global_descriptor_sets.h"

float shininess = 10.f;

vec3 blinn_phong_calc_internal(vec3 lightDir, vec3 lightColor, vec3 normal) {
    float Id = max(dot(lightDir, normal), 0);

    vec3 viewDir = normalize(g_eyePosition - worldPos);
    vec3 halfV = normalize(lightDir + viewDir);

    float Is = 0;
    if (Id > 0) {
        Is = pow(clamp(dot(normal, halfV), 0, 1), shininess);
    }

    return Is * lightColor*Id;
}

vec3 blinn_phong_calc_dir_light(DirLight light, vec3 normal) {
    vec3 lightDir = normalize(-light.direction);

    return blinn_phong_calc_internal(lightDir, light.color, normal);
}

layout( set = 6, binding = 0 ) uniform sampler2DShadow shadowMapSamplers[MAX_SHADER_LIGHTS];

void main() 
{
	fragColor = vec4(g_globalColor, 1);

    vec3 worldNormal = normalize(worldNormal);

    vec3 finalColor = vec3(0);

    if (instIdx == 0) finalColor = vec3(1,0,0);
    else if (instIdx == 1) finalColor = vec3(0,1,0);
    else if (instIdx == 2) finalColor = vec3(0,0,1);
    else if (instIdx == 3) finalColor = vec3(1,1,0);
    else if (instIdx == 4) finalColor = vec3(0,1,1);
    else if (instIdx == 5) finalColor = vec3(1,0,1);
    else if (instIdx == 6) finalColor = vec3(0,0,0);
    else finalColor = vec3(1,1,1);

    for (int nDirLight = 0; nDirLight < g_numDirLights; ++nDirLight)
    {
        finalColor += blinn_phong_calc_dir_light(g_dirLights[nDirLight], worldNormal);
    }

    vec4 lightPos = g_lightViewProj[0] * vec4(worldPos, 1.0);
    vec3 lightPosNDC = lightPos.xyz / lightPos.w;
    lightPosNDC.xy = (lightPosNDC.xy * 0.5) + 0.5;
    //lightPosNDC.z -= 0.00002;

    float shadowFactor = texture(shadowMapSamplers[0], lightPosNDC);

    finalColor = finalColor * shadowFactor + vec3(0.05, 0.05, 0); // FAKE AMBIANT

    fragColor = vec4(finalColor, 1);

    //fragColor = vec4(customShadow, 0, 0, 1);
}
