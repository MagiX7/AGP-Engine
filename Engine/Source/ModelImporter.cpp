#include "ModelImporter.h"

#include "engine.h"

#include "Resources/Model.h"
#include "Resources/Material.h"

#include <iostream>

void ModelImporter::ProcessAssimpMesh(const aiScene* scene, aiMesh* mesh, Mesh* myMesh, uint32_t baseMeshMaterialIndex, std::vector<uint32_t>& submeshMaterialIndices)
{
    std::vector<float> vertices;
    std::vector<uint32_t> indices;

    bool hasTexCoords = false;
    bool hasTangentSpace = false;

    // process vertices
    for (unsigned int i = 0; i < mesh->mNumVertices; i++)
    {
        vertices.push_back(mesh->mVertices[i].x);
        vertices.push_back(mesh->mVertices[i].y);
        vertices.push_back(mesh->mVertices[i].z);
        vertices.push_back(mesh->mNormals[i].x);
        vertices.push_back(mesh->mNormals[i].y);
        vertices.push_back(mesh->mNormals[i].z);

        if (mesh->mTextureCoords[0]) // does the mesh contain texture coordinates?
        {
            hasTexCoords = true;
            vertices.push_back(mesh->mTextureCoords[0][i].x);
            vertices.push_back(mesh->mTextureCoords[0][i].y);
        }

        if (mesh->mTangents != nullptr && mesh->mBitangents)
        {
            hasTangentSpace = true;
            vertices.push_back(mesh->mTangents[i].x);
            vertices.push_back(mesh->mTangents[i].y);
            vertices.push_back(mesh->mTangents[i].z);

            // For some reason ASSIMP gives me the bitangents flipped.
            // Maybe it's my fault, but when I generate my own geometry
            // in other files (see the generation of standard assets)
            // and all the bitangents have the orientation I expect,
            // everything works ok.
            // I think that (even if the documentation says the opposite)
            // it returns a left-handed tangent space matrix.
            // SOLUTION: I invert the components of the bitangent here.
            vertices.push_back(-mesh->mBitangents[i].x);
            vertices.push_back(-mesh->mBitangents[i].y);
            vertices.push_back(-mesh->mBitangents[i].z);
        }
    }

    // process indices
    for (unsigned int i = 0; i < mesh->mNumFaces; i++)
    {
        aiFace face = mesh->mFaces[i];
        for (unsigned int j = 0; j < face.mNumIndices; j++)
        {
            indices.push_back(face.mIndices[j]);
        }
    }

    // store the proper (previously proceessed) material for this mesh
    submeshMaterialIndices.push_back(baseMeshMaterialIndex + mesh->mMaterialIndex);

    // create the vertex format
    VertexBufferLayout vertexBufferLayout = {};
    vertexBufferLayout.attributes.push_back(VertexBufferAttribute{ 0, 3, 0 });
    vertexBufferLayout.attributes.push_back(VertexBufferAttribute{ 1, 3, 3 * sizeof(float) });
    vertexBufferLayout.stride = 6 * sizeof(float);
    if (hasTexCoords)
    {
        vertexBufferLayout.attributes.push_back(VertexBufferAttribute{ 2, 2, vertexBufferLayout.stride });
        vertexBufferLayout.stride += 2 * sizeof(float);
    }
    if (hasTangentSpace)
    {
        vertexBufferLayout.attributes.push_back(VertexBufferAttribute{ 3, 3, vertexBufferLayout.stride });
        vertexBufferLayout.stride += 3 * sizeof(float);

        vertexBufferLayout.attributes.push_back(VertexBufferAttribute{ 4, 3, vertexBufferLayout.stride });
        vertexBufferLayout.stride += 3 * sizeof(float);
    }

    // add the submesh into the mesh
    Submesh submesh = {};
    submesh.vertexBufferLayout = vertexBufferLayout;
    submesh.vertices.swap(vertices);
    submesh.indices.swap(indices);
    myMesh->submeshes.push_back(submesh);
}

void ModelImporter::ProcessAssimpMaterial(App* app, aiMaterial* material, Material& myMaterial, std::string directory)
{
    aiString name;
    aiColor3D diffuseColor;
    aiColor3D emissiveColor;
    aiColor3D specularColor;
    ai_real shininess;
    material->Get(AI_MATKEY_NAME, name);
    material->Get(AI_MATKEY_COLOR_DIFFUSE, diffuseColor);
    material->Get(AI_MATKEY_COLOR_EMISSIVE, emissiveColor);
    material->Get(AI_MATKEY_COLOR_SPECULAR, specularColor);
    material->Get(AI_MATKEY_SHININESS, shininess);

    myMaterial.name = name.C_Str();
    myMaterial.albedo =glm::vec3(diffuseColor.r, diffuseColor.g, diffuseColor.b);
    myMaterial.emissive =glm::vec3(emissiveColor.r, emissiveColor.g, emissiveColor.b);
    myMaterial.smoothness = shininess / 256.0f;

    //aiString aiFilename;
    //if (material->GetTextureCount(aiTextureType_DIFFUSE) > 0)
    //{
    //    material->GetTexture(aiTextureType_DIFFUSE, 0, &aiFilename);
    //    std::string filename = aiFilename.C_Str();
    //    std::string filepath = directory + "/" + filename;
    //    //String filename = MakeString(aiFilename.C_Str());
    //    //String filepath = MakePath(directory, filename);
    //    myMaterial.albedoTextureIndex = LoadTexture2D(app, filepath.str);
    //}
    //if (material->GetTextureCount(aiTextureType_EMISSIVE) > 0)
    //{
    //    material->GetTexture(aiTextureType_EMISSIVE, 0, &aiFilename);
    //    String filename = MakeString(aiFilename.C_Str());
    //    String filepath = MakePath(directory, filename);
    //    myMaterial.emissiveTextureIdx = LoadTexture2D(app, filepath.str);
    //}
    //if (material->GetTextureCount(aiTextureType_SPECULAR) > 0)
    //{
    //    material->GetTexture(aiTextureType_SPECULAR, 0, &aiFilename);
    //    String filename = MakeString(aiFilename.C_Str());
    //    String filepath = MakePath(directory, filename);
    //    myMaterial.specularTextureIdx = LoadTexture2D(app, filepath.str);
    //}
    //if (material->GetTextureCount(aiTextureType_NORMALS) > 0)
    //{
    //    material->GetTexture(aiTextureType_NORMALS, 0, &aiFilename);
    //    String filename = MakeString(aiFilename.C_Str());
    //    String filepath = MakePath(directory, filename);
    //    myMaterial.normalsTextureIdx = LoadTexture2D(app, filepath.str);
    //}
    //if (material->GetTextureCount(aiTextureType_HEIGHT) > 0)
    //{
    //    material->GetTexture(aiTextureType_HEIGHT, 0, &aiFilename);
    //    String filename = MakeString(aiFilename.C_Str());
    //    String filepath = MakePath(directory, filename);
    //    myMaterial.bumpTextureIdx = LoadTexture2D(app, filepath.str);
    //}

    //myMaterial.createNormalFromBump();
}

void ModelImporter::ProcessAssimpNode(const aiScene* scene, aiNode* node, Mesh* myMesh, uint32_t baseMeshMaterialIndex, std::vector<uint32_t>& submeshMaterialIndices)
{
    // process all the node's meshes (if any)
    for (unsigned int i = 0; i < node->mNumMeshes; i++)
    {
        aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
        ModelImporter::ProcessAssimpMesh(scene, mesh, myMesh, baseMeshMaterialIndex, submeshMaterialIndices);
    }

    // then do the same for each of its children
    for (unsigned int i = 0; i < node->mNumChildren; i++)
    {
        ProcessAssimpNode(scene, node->mChildren[i], myMesh, baseMeshMaterialIndex, submeshMaterialIndices);
    }
}

std::shared_ptr<Model> ModelImporter::LoadModel(App* app, const char* filename)
{
    const aiScene* scene = aiImportFile(filename,
        aiProcess_Triangulate |
        aiProcess_GenSmoothNormals |
        aiProcess_CalcTangentSpace |
        aiProcess_JoinIdenticalVertices |
        aiProcess_PreTransformVertices |
        aiProcess_ImproveCacheLocality |
        aiProcess_OptimizeMeshes |
        aiProcess_SortByPType);

    if (!scene)
    {
        std::cout << "Error loading mesh " << filename << ": " << aiGetErrorString();
        return nullptr;
    }

    app->meshes.push_back(Mesh{});
    Mesh& mesh = app->meshes.back();
    uint32_t meshIdx = (uint32_t)app->meshes.size() - 1u;

    app->models.push_back(Model{});
    Model& model = app->models.back();
    model.meshIndex = meshIdx;
    uint32_t modelIdx = (uint32_t)app->models.size() - 1u;

    String directory = GetDirectoryPart(MakeString(filename));

    // Create a list of materials
    uint32_t baseMeshMaterialIndex = (uint32_t)app->materials.size();
    for (unsigned int i = 0; i < scene->mNumMaterials; ++i)
    {
        app->materials.push_back(Material{});
        Material& material = app->materials.back();
        ProcessAssimpMaterial(app, scene->mMaterials[i], material, directory.str);
    }

    ProcessAssimpNode(scene, scene->mRootNode, &mesh, baseMeshMaterialIndex, model.materialsIndex);

    aiReleaseImport(scene);

    uint32_t vertexBufferSize = 0;
    uint32_t indexBufferSize = 0;

    for (uint32_t i = 0; i < mesh.submeshes.size(); ++i)
    {
        vertexBufferSize += mesh.submeshes[i].vertices.size() * sizeof(float);
        indexBufferSize += mesh.submeshes[i].indices.size() * sizeof(uint32_t);
    }


    auto ret = std::make_shared<Model>();



    /*glGenBuffers(1, &mesh.vboHandle);
    glBindBuffer(GL_ARRAY_BUFFER, mesh.vboHandle);
    glBufferData(GL_ARRAY_BUFFER, vertexBufferSize, NULL, GL_STATIC_DRAW);

    glGenBuffers(1, &mesh.iboHandle);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.iboHandle);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexBufferSize, NULL, GL_STATIC_DRAW);*/

    uint32_t indicesOffset = 0;
    uint32_t verticesOffset = 0;

    for (uint32_t i = 0; i < mesh.submeshes.size(); ++i)
    {
        const void* verticesData = mesh.submeshes[i].vertices.data();
        const uint32_t   verticesSize = mesh.submeshes[i].vertices.size() * sizeof(float);
        glBufferSubData(GL_ARRAY_BUFFER, verticesOffset, verticesSize, verticesData);
        mesh.submeshes[i].vertexOffset = verticesOffset;
        verticesOffset += verticesSize;

        const void* indicesData = mesh.submeshes[i].indices.data();
        const uint32_t   indicesSize = mesh.submeshes[i].indices.size() * sizeof(uint32_t);
        glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, indicesOffset, indicesSize, indicesData);
        mesh.submeshes[i].indexOffset = indicesOffset;
        indicesOffset += indicesSize;
    }

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    return modelIdx;
}