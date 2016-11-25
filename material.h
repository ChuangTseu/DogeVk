#pragma once

#include <assert.h>

#include <string>
#include <iostream>

//#include "MathsTools/vec3.h"
//
//#include "texture.h"

#include <glm/vec3.hpp>


struct aiMaterial;

struct Material
{
    glm::vec3 m_ambientReflectance;
    glm::vec3 m_diffuseReflectance;
    glm::vec3 m_specularReflectance;

    float m_specularExponent; // Shininess

    //Texture m_diffuseTexture;
    //Texture m_normalTexture;

public:
    Material() {}

    bool loadFromAssimpMaterial(const aiMaterial* mat, std::string matBaseDir);
};
