#include "ModelImporter.h"

#include <iostream>

Model* ModelImporter::ImportModel(std::string path)
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

    Model* model = new Model();
    
    ProcessNode(scene->mRootNode, scene, *model);

    std::cout << "Model " << path.c_str() << " loaded" << std::endl;

    return model;
}

void ModelImporter::ProcessNode(aiNode* node, const aiScene* scene, Model& model)
{
    for (unsigned int i = 0; i < node->mNumMeshes; i++)
    {
        aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
        model.AddMesh(ProcessMesh(mesh, scene));
        //model.AddMesh(ProcessSubMesh(mesh, scene));
        //model.meshes.push_back(ProcessMesh(mesh, scene));
    }
    
    for (unsigned int i = 0; i < node->mNumChildren; i++)
    {
        ProcessNode(node->mChildren[i], scene, model);
    }
}

std::shared_ptr<Mesh> ModelImporter::ProcessMesh(aiMesh* mesh, const aiScene* scene)
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
    if (hasNormals)
    {
        vertexBufferLayout.attributes.push_back(VertexBufferAttribute{ 1, 3, 3 * sizeof(float) });
        vertexBufferLayout.stride = 6 * sizeof(float);
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

    std::shared_ptr<Mesh> myMesh = std::make_shared<Mesh>(vertices, indices, vertexBufferLayout);
    return myMesh;

    //Submesh submesh = {};
    //submesh.vertexBufferLayout = vertexBufferLayout;
    //submesh.vertices.swap(vertices);
    //submesh.indices.swap(indices);
    //myMesh.submeshes.push_back(submesh);

    //return new SubMesh(mesh->mName.C_Str(), vertices, indices);
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

