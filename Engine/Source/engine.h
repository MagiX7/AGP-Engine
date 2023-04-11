#pragma once

#include "platform.h"
#include "Resources/Material.h"
#include "Renderer/Camera.h"
#include "Entity.h"
#include "Light.h"

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

class Framebuffer;

class Application
{
public:
    Application();
    virtual ~Application();

    inline static Application& GetInstance() { return *instance; }

    void Init();
    void Update();
    void Render();
    void OnImGuiRender();

    inline float GetDeltaTime() const { return deltaTime; }
    inline void SetDeltaTime(float dt) { deltaTime = dt; }
    inline const Input& GetInput() const { return input; }
    inline Input& GetInput() { return input; }
    inline const glm::ivec2& GetDisplaySize() const { return displaySize; }
    inline void SetDisplaySize(const glm::ivec2& size) { displaySize = size; }

private:
    Application(const Application&);
    Application& operator=(Application&) {}


public:
    bool isRunning;

private:
    static Application* instance;

    bool showRenderOptionsPanel = true;
    unsigned int currentRenderTargetId;

    // Loop
    f32  deltaTime;

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
    std::shared_ptr<Shader> texturedGeometryShader;
    u32 modelShaderIndex;

    // texture indices
    u32 diceTexIdx;
    std::shared_ptr<Texture2D> diceTex;
    u32 whiteTexIdx;
    u32 blackTexIdx;
    u32 normalTexIdx;
    u32 magentaTexIdx;


    // Buffers
    int maxUniformBufferSize;
    int uniformBlockAlignment;

    // Models
    std::shared_ptr<Framebuffer> fbo;
    glm::vec2 viewportSize = glm::vec2(0);

    std::shared_ptr<Model> patrickModel;
    std::shared_ptr<Shader> patrickShader;
    std::shared_ptr<Texture2D> patrickTexture;

    std::shared_ptr<UniformBuffer> localParamsUbo;
    std::shared_ptr<UniformBuffer> globalParamsUbo;
    int globalParamsOffset;
    int globalParamsSize;

    std::vector<Entity> entities;
    Entity* currentEntity = nullptr;

    Camera camera;
    Light dirLight;

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
