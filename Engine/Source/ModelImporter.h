#pragma once

#include "Resources/Model.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

class ModelImporter
{
public:
	static Model* ImportModel(std::string path);

private:
	static void ProcessNode(aiNode* node, const aiScene* scene, Model& model);
	static std::shared_ptr<Mesh> ProcessMesh(aiMesh* node, const aiScene* scene);
	static void ComputeTangentsAndBiTangents(std::vector<MeshVertex>& vertices, unsigned int indicesCount);
};
