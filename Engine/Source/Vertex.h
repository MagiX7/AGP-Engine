#pragma once

#include <glm/glm.hpp>

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

struct VertexBufferAttribute
{
    uint8_t location;
    uint8_t componentCount;
    uint8_t offset;
};

struct VertexBufferLayout
{
    std::vector<VertexBufferAttribute> attributes;
    uint8_t stride;
};








