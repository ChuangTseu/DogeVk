#pragma once

#include <string>
#include <vector>

#include <assimp/matrix4x4.h>

#include "mesh.h"
#include "material.h"

struct aiNode;

struct ModelNode {
    unsigned int m_numMeshes;
    unsigned int* m_meshIndex;

    std::vector<unsigned int> m_meshIndices;

//    unsigned int m_materialIndex;

    glm::mat4 m_transformation;

    unsigned int m_numNodes;
    ModelNode* m_childNodes;
};

struct Model
{
    std::vector<Mesh> m_meshes;
    std::vector<Material> m_materials;

    std::vector<ModelNode> m_nodes;

    ModelNode* m_rootNode;

public:
    enum class BasicType {
        CUBE, PYRAMID, TRIANGLE, PLAN
    };

public:
    Model();

    bool loadFromFile(const std::string& filename);

	void Draw() const;

    void drawAsTriangles();
    void drawAsPatch();

    void drawAsTriangles(const glm::mat4& projection, const glm::mat4& view, const glm::mat4& model);
    void drawAsPatch(const glm::mat4& projection, const glm::mat4& view, const glm::mat4& model);

    void loadNodeGraph(aiNode* ainode, const aiMatrix4x4& accumulatedTransformation, int level = 0);
};
