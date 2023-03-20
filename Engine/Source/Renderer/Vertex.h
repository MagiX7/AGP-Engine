#pragma once

#include <glm/glm.hpp>
#include <vector>

struct ScreenSpaceVertex
{
    glm::vec3 pos = glm::vec3(-1);
    glm::vec2 uv = glm::vec2(-1);
};

struct MeshVertex
{
    glm::vec3 pos = glm::vec3(-1);
    glm::vec2 uv = glm::vec2(-1);
    glm::vec3 normals = glm::vec3(-1);
    glm::vec3 tangents = glm::vec3(-1);
    glm::vec3 biTangents = glm::vec3(-1);
};




struct VertexShaderAttribute
{
    uint8_t location;
    uint8_t componentCount;
};

struct VertexShaderLayout
{
    std::vector< VertexShaderAttribute> attributes;
};



