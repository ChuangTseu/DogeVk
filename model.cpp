#include "model.h"

#include <assert.h>

#include <assimp/Importer.hpp> // C++ importer interface
#include <assimp/scene.h> // Output data structure
#include <assimp/postprocess.h> // Post processing flags
#include <assimp/config.h>
#include <assimp/matrix4x4.h>

#define BUFFER_OFFSET(p) ((uint8_t*)0 + p)


Model::Model()
{
}

void aiMat4ToGlmMat4(const aiMatrix4x4& aiMat4, glm::mat4& glmMat4) {
    for (int i = 0 ; i < 4; ++i) {
        for (int j = 0 ; j < 4; ++j) {
            glmMat4[j][i] = aiMat4[i][j];
        }
    }
}

void Model::loadNodeGraph(aiNode* ainode, const aiMatrix4x4& accumulatedTransformation, int level) {
    aiMatrix4x4t<float> transformation =  accumulatedTransformation * ainode->mTransformation;

    if (ainode->mNumMeshes > 0) {
        ModelNode current;

        for (unsigned int i = 0; i < ainode->mNumMeshes; ++i) {
            current.m_meshIndices.push_back(ainode->mMeshes[i]);
        }

        aiMat4ToGlmMat4(transformation, current.m_transformation);

        m_nodes.push_back(current);
    }

    for (unsigned int i = 0; i < ainode->mNumChildren; ++i) {
        loadNodeGraph(ainode->mChildren[i], transformation, level + 1);
    }
}

bool Model::loadFromFile(const std::string& filename)
{
    Assimp::Importer importer;

    // Component to be removed when importing file
    importer.SetPropertyInteger(AI_CONFIG_PP_RVC_FLAGS,
                                aiComponent_COLORS
//                                | aiComponent_TANGENTS_AND_BITANGENTS
                                | aiComponent_BONEWEIGHTS
                                | aiComponent_ANIMATIONS
                                | aiComponent_LIGHTS
                                | aiComponent_CAMERAS);
    importer.SetPropertyInteger(AI_CONFIG_PP_PTV_NORMALIZE, 1);

    // And have it read the given file with some example postprocessing
    // Usually - if speed is not the most important aspect for you - you'll
    // propably to request more postprocessing than we do in this example.
	const aiScene* scene = importer.ReadFile(filename,
        aiProcess_Triangulate
        | aiProcess_JoinIdenticalVertices
        | aiProcess_RemoveComponent
        | aiProcess_GenNormals
        | aiProcess_CalcTangentSpace
        | aiProcess_PreTransformVertices
        //| aiProcess_GenUVCoords
        //| aiProcess_MakeLeftHanded
    );

//    const aiScene* scene = importer.ReadFile( filename,
//                                              aiProcessPreset_TargetRealtime_MaxQuality
//    );

    // If the import failed, report it
    if( !scene)
    {
        std::cerr << importer.GetErrorString() << '\n';
        return false;
    }    

    m_nodes.clear();

    aiNode* rootNode = scene->mRootNode;
    loadNodeGraph(rootNode, aiMatrix4x4());

    m_meshes.clear();
    m_meshes.resize(scene->mNumMeshes);

    m_materials.clear();
    //m_materials.resize(scene->mNumMaterials);

    for (unsigned int i = 0; i < scene->mNumMeshes; ++i) {
        m_meshes[i].loadFromAssimpMesh(scene->mMeshes[i]);
    }

    //for (unsigned int i = 0; i < scene->mNumMaterials; ++i) {
    //    m_materials[i].loadFromAssimpMaterial(scene->mMaterials[i], dir);
    //}

//    const aiMesh* mesh = scene->mMeshes[0];

//    std::cerr << "Mesh HasFaces ? " << mesh->HasFaces() << '\n';
//    std::cerr << "Mesh HasNormals ? " << mesh->HasNormals() << '\n';
//    std::cerr << "Mesh HasPositions ? " << mesh->HasPositions() << '\n';
//    std::cerr << "Mesh HasTangentsAndBitangents ? " << mesh->HasTangentsAndBitangents() << '\n';
//    std::cerr << "Mesh HasBones ? " << mesh->HasBones() << '\n';
//    std::cerr << "Mesh HasTextureCoords ? " << mesh->HasTextureCoords(0) << '\n';
//    std::cerr << "mesh->mTextureCoords[0] != NULL ? " << (mesh->mTextureCoords[0] != NULL) << '\n';
//    std::cerr << "Mesh GetNumUVChannels ? " << mesh->GetNumUVChannels() << '\n';
//    std::cerr << "Mesh GetNumColorChannels ? " << mesh->GetNumColorChannels() << '\n';
//    std::cerr << "Mesh mNumFaces ? " << mesh->mNumFaces << '\n';
//    std::cerr << "Mesh mNumVertices ? " << mesh->mNumVertices << '\n';
//    std::cerr << "Mesh mPrimitiveTypes ? " << mesh->mPrimitiveTypes << " and should be " << aiPrimitiveType_TRIANGLE << '\n';

    return true;
}

void Model::Draw() const
{
	for (const ModelNode& node : m_nodes) {
		for (unsigned int meshIndex : node.m_meshIndices) {
			const Mesh& mesh = m_meshes[meshIndex];

			//SystemCBuffersManager::GetMutablePerObjectCB().g_world.identity();

			//SystemCBuffersManager::CommitPerObjectCB();

			//mesh.drawAsTriangles();
		}
	}
}

void Model::drawAsPatch() {
    for (const ModelNode& node : m_nodes) {
        for (unsigned int meshIndex : node.m_meshIndices) {
            Mesh& mesh = m_meshes[meshIndex];

            //if (s) {
            //    s->sendMaterial(m_materials[mesh.m_materialIndex]);
            //}
            //mesh.drawAsPatch();
        }
    }
}

void Model::drawAsTriangles() {
    for (const ModelNode& node : m_nodes) {
        for (unsigned int meshIndex : node.m_meshIndices) {
            Mesh& mesh = m_meshes[meshIndex];

            //if (s) {
            //    s->sendMaterial(m_materials[mesh.m_materialIndex]);
            //}
            //mesh.drawAsTriangles();
        }
    }
}

void Model::drawAsPatch(const glm::mat4 &projection, const glm::mat4 &view, const glm::mat4 &model) {
    for (const ModelNode& node : m_nodes) {
        for (unsigned int meshIndex : node.m_meshIndices) {
            Mesh& mesh = m_meshes[meshIndex];

            //if (s) {
            //    s->sendMaterial(m_materials[mesh.m_materialIndex]);

            //    s->sendTransformations(projection, view, model * node.m_transformation);
            //}
            //mesh.drawAsPatch();
        }
    }
}

void Model::drawAsTriangles(const glm::mat4 &projection, const glm::mat4 &view, const glm::mat4 &model) {
    for (const ModelNode& node : m_nodes) {
        for (unsigned int meshIndex : node.m_meshIndices) {
            Mesh& mesh = m_meshes[meshIndex];

            //if (s) {
            //    s->sendMaterial(m_materials[mesh.m_materialIndex]);

            //    s->sendTransformations(projection, view, model * node.m_transformation);
            //}
            //mesh.drawAsTriangles();
        }
    }

}


