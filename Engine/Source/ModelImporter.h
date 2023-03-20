#pragma once

#include "Resources/Model.h"
#include <vector>
#include <memory>

#include <assimp/cimport.h>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

class App;
class Material;

class ModelImporter
{
public:
	// Returns the index of the model
	static std::shared_ptr<Model>  LoadModel(App* app, const char* filename);
private:
	static void ProcessAssimpMesh(const aiScene* scene, aiMesh* mesh, Mesh* myMesh, uint32_t baseMeshMaterialIndex, std::vector<uint32_t>& submeshMaterialIndices);
	static void ProcessAssimpMaterial(App* app, aiMaterial* material, Material& myMaterial, std::string directory);
	static void ProcessAssimpNode(const aiScene* scene, aiNode* node, Mesh* myMesh, uint32_t baseMeshMaterialIndex, std::vector<uint32_t>& submeshMaterialIndices);
};