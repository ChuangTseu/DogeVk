#include "material.h"

#include <assimp/Importer.hpp> // C++ importer interface
#include <assimp/scene.h> // Output data structure
#include <assimp/postprocess.h> // Post processing flags
#include <assimp/config.h>

#define TEXTEST(type) if (mat->GetTextureCount(type) > 0) std::cerr << "***  " << mat->GetTextureCount(type) << " "#type" \n";

constexpr glm::vec3 aiColor3D_to_glmVec3(const aiColor3D& color) {
	return glm::vec3{ color.r, color.g, color.b };
}

bool Material::loadFromAssimpMaterial(const aiMaterial *mat, std::string matBaseDir) {
    //        aiString name;
    //        mat->Get(AI_MATKEY_NAME,name);


//    std::cerr << color.r << ", " << color.g << ", " << color.b << '\n';

    aiColor3D color (0.f,0.f,0.f);
    float value = 0.f;

	glm::vec3 defaultColor{1.f, 1.f, 1.f};

    if ( AI_SUCCESS == mat->Get(AI_MATKEY_COLOR_AMBIENT, color) ) {
        m_ambientReflectance = aiColor3D_to_glmVec3(color);
    }
    else {
        m_ambientReflectance = defaultColor;
    }


//    std::cerr << color.r << ", " << color.g << ", " << color.b << '\n';

    if ( AI_SUCCESS == mat->Get(AI_MATKEY_COLOR_DIFFUSE, color) ) {
        m_diffuseReflectance = aiColor3D_to_glmVec3(color);
    }
    else {
        m_diffuseReflectance = defaultColor;
    }

    if ( AI_SUCCESS == mat->Get(AI_MATKEY_COLOR_SPECULAR, color) ) {
        m_specularReflectance = aiColor3D_to_glmVec3(color);
    }
    else {
        m_specularReflectance = defaultColor;
    }


    if ( AI_SUCCESS == mat->Get(AI_MATKEY_SHININESS, value) ) {
        m_specularExponent = value / 4.f;
    }
    else {
        m_specularExponent = 1.f;
    }


//    std::cerr << "Warning, dividing shininess by 4.f, Why Da Fuck ASSIMP ????? \n";

//    std::cerr << "***  " << mat->GetTextureCount(aiTextureType_DIFFUSE) << " aiTextureType_DIFFUSE \n";

    //if (mat->GetTextureCount(aiTextureType_DIFFUSE) > 0) {
    //    aiString texPath;

    //    if (mat->GetTexture(aiTextureType_DIFFUSE, 0, &texPath, NULL, NULL, NULL, NULL, NULL) == AI_SUCCESS) {
    //        m_diffuseTexture.loadFromFile(matBaseDir + '/' + texPath.data, GL_SRGB);
    //    }
    //}
    //else {
    //    m_diffuseTexture.loadFromMaterialColor(m_diffuseReflectance);
    //}

    //if (mat->GetTextureCount(aiTextureType_NORMALS) > 0) {
    //    aiString texPath;

    //    if (mat->GetTexture(aiTextureType_NORMALS, 0, &texPath, NULL, NULL, NULL, NULL, NULL) == AI_SUCCESS) {
    //        m_normalTexture.loadFromFile(matBaseDir + '/' + texPath.data);
    //    }
    //}
    //else {
    //    // WARNING, TEMPORARY HACK FOR OBJ FILES WITH BUMP MAPS. ASSIMP BUG :(
    //    if (mat->GetTextureCount(aiTextureType_HEIGHT) > 0) {
    //        aiString texPath;

    //        if (mat->GetTexture(aiTextureType_HEIGHT, 0, &texPath, NULL, NULL, NULL, NULL, NULL) == AI_SUCCESS) {
    //            m_normalTexture.loadFromFile(matBaseDir + '/' + texPath.data);
    //        }
    //    }
    //    else {
    //        m_normalTexture.loadFromMaterialColor(Color3f{0.5f, 0.5f, 1.f});
    //    }

    //}

//    TEXTEST(aiTextureType_NONE );
//    TEXTEST(aiTextureType_DIFFUSE);
//    TEXTEST(aiTextureType_SPECULAR );
//    TEXTEST(aiTextureType_AMBIENT );
//    TEXTEST(aiTextureType_EMISSIVE );
//    TEXTEST(aiTextureType_HEIGHT );
//    TEXTEST(aiTextureType_NORMALS );
//    TEXTEST(aiTextureType_SHININESS );
//    TEXTEST(aiTextureType_OPACITY );
//    TEXTEST(aiTextureType_DISPLACEMENT );
//    TEXTEST(aiTextureType_LIGHTMAP );
//    TEXTEST(aiTextureType_REFLECTION );
//    TEXTEST(aiTextureType_UNKNOWN );

//    std::cerr << '\n';

    return true;
}
