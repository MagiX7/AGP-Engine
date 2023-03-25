#pragma once

#include "Resources/Model.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

class ModelImporter
{
public:
	static std::shared_ptr<Model> ImportModel(std::string path);

private:
	static void ProcessNode(aiNode* node, const aiScene* scene, Model& model);
	static std::shared_ptr<Mesh> ProcessMesh(aiMesh* node, const aiScene* scene);
	static void ProcessMaterial(aiMaterial* material, Material& myMaterial, const std::string& directory);
	static void ComputeTangentsAndBiTangents(std::vector<MeshVertex>& vertices, unsigned int indicesCount);
};
