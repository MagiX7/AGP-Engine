#include "ModelImporter.h"

#include <iostream>
#include <memory>

std::shared_ptr<Model> ModelImporter::ImportModel(std::string path)
{
    Assimp::Importer importer;
    std::string exts;
    importer.GetExtensionList(exts);
    std::vector<std::string> extensions;
    std::string currentExtension;
    
    for (int i = 0; i < exts.size(); ++i)
    {
        char currentChar = exts[i];
        if (currentChar == '*')
            continue;

        if (currentChar == ';' || i == exts.size() - 1)
        {
            extensions.push_back(currentExtension);
            currentExtension.clear();
        }
        else
        {
            currentExtension += exts[i];
        }
    }

    std::string fileExtension = path.substr(path.find_last_of("."));

    /*if (std::find(extensions.begin(), extensions.end(), fileExtension) == extensions.end())
    {
        std::cout << "Model Format " << fileExtension << " from " << path.c_str() << " not supported" << std::endl;
        return nullptr;
    }*/

    const aiScene* scene = importer.ReadFile(path, aiProcess_CalcTangentSpace | aiProcess_Triangulate | aiProcess_FlipUVs);

    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
    {
        std::cout << "Assimp error: " << importer.GetErrorString() << std::endl;
        return nullptr;
    }

    std::string name = path.substr(path.find_last_of("/") + 1);
    std::shared_ptr<Model> model = std::make_shared<Model>(name);


    std::string directory = path.substr(0, path.find_last_of("/") + 1);

    for (unsigned int i = 0; i < scene->mNumMaterials; ++i)
    {
        std::shared_ptr<Material> material = std::make_shared<Material>("");
        ProcessMaterial(scene->mMaterials[i], *material, directory);
        model->AddMaterial(material);
    }

    ProcessNode(scene->mRootNode, scene, *model);

    std::cout << "Model " << path.c_str() << " loaded" << std::endl;

    return model;
}

void ModelImporter::ProcessNode(aiNode* node, const aiScene* scene, Model& model)
{
    for (unsigned int i = 0; i < node->mNumMeshes; i++)
    {
        aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
        model.AddMesh(ProcessMesh(mesh, scene, model));
        //model.AddMesh(ProcessSubMesh(mesh, scene));
        //model.meshes.push_back(ProcessMesh(mesh, scene));
    }
    
    for (unsigned int i = 0; i < node->mNumChildren; i++)
    {
        ProcessNode(node->mChildren[i], scene, model);
    }
}

std::shared_ptr<Mesh> ModelImporter::ProcessMesh(aiMesh* mesh, const aiScene* scene, Model& model)
{
    std::vector<float> vertices;
    std::vector<uint32_t> indices;

    //vertices.resize(mesh->mNumVertices);
    //indices.resize(mesh->mNumFaces);
    
    bool hasNormals = false;
    bool hasTexCoords = false;
    bool hasTangentSpace = false;

    // process vertices
    for (unsigned int i = 0; i < mesh->mNumVertices; i++)
    {
        vertices.push_back(mesh->mVertices[i].x);
        vertices.push_back(mesh->mVertices[i].y);
        vertices.push_back(mesh->mVertices[i].z);
        if (mesh->mNormals)
        {
            vertices.push_back(mesh->mNormals[i].x);
            vertices.push_back(mesh->mNormals[i].y);
            vertices.push_back(mesh->mNormals[i].z);
            hasNormals = true;
        }

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

    //if (!mesh->HasTangentsAndBitangents())
    //{
    //    ComputeTangentsAndBiTangents(vertices, mesh->mNumFaces);
    //}

    VertexBufferLayout vertexBufferLayout = {};
    vertexBufferLayout.attributes.push_back(VertexBufferAttribute{ 0, 3, 0 });
    vertexBufferLayout.stride += 3 * sizeof(float);
    if (hasNormals)
    {
        vertexBufferLayout.attributes.push_back(VertexBufferAttribute{ 1, 3, 3 * sizeof(float) });
        vertexBufferLayout.stride += 3 * sizeof(float);
    }
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

    std::shared_ptr<Mesh> myMesh = std::make_shared<Mesh>(mesh->mName.C_Str(), vertices, indices, vertexBufferLayout);
    myMesh->SetMaterial(model.GetMaterials()[mesh->mMaterialIndex]);
    return myMesh;

    //Submesh submesh = {};
    //submesh.vertexBufferLayout = vertexBufferLayout;
    //submesh.vertices.swap(vertices);
    //submesh.indices.swap(indices);
    //myMesh.submeshes.push_back(submesh);

    //return new SubMesh(mesh->mName.C_Str(), vertices, indices);
}

void ModelImporter::ProcessMaterial(aiMaterial* material, Material& myMaterial, const std::string& directory)
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

    myMaterial.SetName(name.C_Str());
    myMaterial.SetAlbedoColor({ diffuseColor.r, diffuseColor.g, diffuseColor.b });
    myMaterial.SetEmissiveColor({ emissiveColor.r, emissiveColor.g, emissiveColor.b });

    myMaterial.SetSmoothness(shininess / 256.0f);

    aiString aiFilename;
    if (material->GetTextureCount(aiTextureType_DIFFUSE) > 0)
    {
        material->GetTexture(aiTextureType_DIFFUSE, 0, &aiFilename);

        std::string fileName = aiFilename.C_Str();

        if (size_t loc = fileName.find("\\"))
            fileName.replace(loc, loc + 2, "/");


        std::string filepath = directory + fileName;
        myMaterial.SetAlbedoMap(std::make_shared<Texture2D>(filepath));
    }
    if (material->GetTextureCount(aiTextureType_EMISSIVE) > 0)
    {
        material->GetTexture(aiTextureType_EMISSIVE, 0, &aiFilename);
        
        std::string fileName = aiFilename.C_Str();

        if (size_t loc = fileName.find("\\"))
            fileName.replace(loc, loc + 2, "/");

        std::string filepath = directory + fileName;
        myMaterial.SetEmissiveMap(std::make_shared<Texture2D>(filepath));
    }
    if (material->GetTextureCount(aiTextureType_SPECULAR) > 0)
    {
        material->GetTexture(aiTextureType_SPECULAR, 0, &aiFilename);

        std::string fileName = aiFilename.C_Str();

        if (size_t loc = fileName.find("\\"))
            fileName.replace(loc, loc + 2, "/");

        std::string filepath = directory + fileName;
        myMaterial.SetSpecularMap(std::make_shared<Texture2D>(filepath));
    }
    if (material->GetTextureCount(aiTextureType_NORMALS) > 0)
    {
        material->GetTexture(aiTextureType_NORMALS, 0, &aiFilename);

        std::string fileName = aiFilename.C_Str();

        if (size_t loc = fileName.find("\\"))
            fileName.replace(loc, loc + 2, "/");

        std::string filepath = directory + fileName;
        myMaterial.SetNormalMap(std::make_shared<Texture2D>(filepath));
    }
    if (material->GetTextureCount(aiTextureType_HEIGHT) > 0)
    {
        material->GetTexture(aiTextureType_HEIGHT, 0, &aiFilename);

        std::string fileName = aiFilename.C_Str();

        if (size_t loc = fileName.find("\\"))
            fileName.replace(loc, loc + 2, "/");

        std::string filepath = directory + fileName;
        myMaterial.SetBumpMap(std::make_shared<Texture2D>(filepath));
    }

    //myMaterial.createNormalFromBump();
}

void ModelImporter::ComputeTangentsAndBiTangents(std::vector<MeshVertex>& vertices, unsigned int indicesCount)
{
    for (int i = 0; i < indicesCount; i += 3)
    {
        glm::vec2 uv1 = { vertices[i].texCoords };
        glm::vec2 uv2 = { vertices[i + 1].texCoords };
        glm::vec2 uv3 = { vertices[i + 2].texCoords };

        glm::vec2 deltaUv1 = uv2 - uv1;
        glm::vec2 deltaUv2 = uv3 - uv1;

        glm::vec3 edge1 = vertices[i + 1].position - vertices[i].position;
        glm::vec3 edge2 = vertices[i + 2].position - vertices[i].position;

        float f = 1.0f / (deltaUv1.x * deltaUv2.y - deltaUv2.x * deltaUv1.y);

        vertices[i].tangents.x = f * (deltaUv2.y * edge1.x - deltaUv1.y * edge2.x);
        vertices[i].tangents.y = f * (deltaUv2.y * edge1.y - deltaUv1.y * edge2.y);
        vertices[i].tangents.z = f * (deltaUv2.y * edge1.z - deltaUv1.y * edge2.z);
        vertices[i].tangents = glm::normalize(vertices[i].tangents);

        if (i + 3 > indicesCount)
            break;
    }
}

