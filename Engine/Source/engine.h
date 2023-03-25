//
// engine.h: This file contains the types and functions relative to the engine.
//

#pragma once

#include "platform.h"
#include "Resources/Model.h"
#include "Resources/Material.h"
#include "Renderer/Camera.h"

#include <glad/glad.h>

typedef glm::vec2  vec2;
typedef glm::vec3  vec3;
typedef glm::vec4  vec4;
typedef glm::ivec2 ivec2;
typedef glm::ivec3 ivec3;
typedef glm::ivec4 ivec4;

struct Image
{
    void* pixels;
    ivec2 size;
    i32   nchannels;
    i32   stride;
};

struct Texture
{
    GLuint      handle;
    std::string filepath;
};

struct Program
{
    GLuint             handle;
    std::string        filepath;
    std::string        programName;
    u64                lastWriteTimestamp; // What is this for?

    VertexShaderLayout vertexInputLayout;
};

enum Mode
{
    Mode_TexturedQuad,
    Mode_Count,
    Mode_Model
};


const ScreenSpaceVertex screenSpaceVertices[] =
{
    { glm::vec3(-0.5, -0.5, 0.0), glm::vec2(0.0, 0.0) },
    { glm::vec3( 0.5, -0.5, 0.0), glm::vec2(1.0, 0.0) },
    { glm::vec3( 0.5,  0.5, 0.0), glm::vec2(1.0, 1.0) },
    { glm::vec3(-0.5,  0.5, 0.0), glm::vec2(0.0, 1.0) }
};

const uint16_t quadIndices[] =
{
    0, 1, 2, 0, 2, 3
};


struct App
{
    // Loop
    f32  deltaTime;
    bool isRunning;

    // Input
    Input input;

    // Graphics
    char gpuName[64];
    char openGlVersion[64];

    ivec2 displaySize;

    std::vector<Texture>  textures;
    std::vector<Material> materials;
    std::vector<Mesh>     meshes;
    std::vector<Model>    models;
    std::vector<Program>  programs;

    // program indices
    u32 texturedGeometryShaderIdx;
    u32 modelShaderIndex;

    // texture indices
    u32 diceTexIdx;
    u32 whiteTexIdx;
    u32 blackTexIdx;
    u32 normalTexIdx;
    u32 magentaTexIdx;


    // Models
    std::shared_ptr<Model> patrickModel;
    std::shared_ptr<Shader> patrickShader;
    std::shared_ptr<VertexArray> patrickVao;
    std::shared_ptr<VertexBuffer> patrickVbo;
    std::shared_ptr<IndexBuffer> patrickIbo;
    std::shared_ptr<Texture2D> patrickTexture;

    Camera camera;

    // Mode
    Mode mode;

    // Embedded geometry (in-editor simple meshes such as
    // a screen filling quad, a cube, a sphere...)
    GLuint screenSpaceVbo;
    GLuint screenSpaceIbo;

    // VAO object to link our screen filling quad with our textured quad shader
    GLuint screenSpaceVao;

    // Location of the texture uniform in the textured quad shader
    GLuint programUniformTexture;

};

void Init(App* app);

void Gui(App* app);

void Update(App* app);

void Render(App* app);

