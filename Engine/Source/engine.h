#pragma once

#include "platform.h"
#include "Resources/Material.h"
#include "Renderer/Camera.h"
#include "Entity.h"
#include "Light.h"

#include <GLFW/glfw3.h>

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
    { glm::vec3(-1.0, -1.0, 0.0), glm::vec2(0.0, 0.0) },
    { glm::vec3( 1.0, -1.0, 0.0), glm::vec2(1.0, 0.0) },
    { glm::vec3( 1.0,  1.0, 0.0), glm::vec2(1.0, 1.0) },
    { glm::vec3(-1.0,  1.0, 0.0), glm::vec2(0.0, 1.0) }
};

const uint16_t quadIndices[] =
{
    0, 1, 2, 0, 2, 3
};

class Framebuffer;

enum class RenderPath
{
    FORWARD = 0, DEFERRED
};

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

    void OnWindowResized(const glm::vec2& dimensions);

    inline float GetDeltaTime() const { return deltaTime; }
    inline void SetDeltaTime(float dt) { deltaTime = dt; }
    
    inline const glm::vec2& GetViewportSize() const { return viewportSize; }
    inline void SetViewportSize(const glm::vec2& size) { viewportSize = size; }

    inline RenderPath GetRenderPath() { return renderPath; }

    inline void SetWindow(GLFWwindow* win) { window = win; }
    inline GLFWwindow* GetWindow() { return window; }

private:
    Application(const Application&);
    Application& operator=(Application&) {}


public:
    bool isRunning;
    Input input;

private:
    static Application* instance;
    RenderPath renderPath;

    bool showRenderOptionsPanel = true;
    unsigned int currentRenderTargetId;

    // Loop
    f32  deltaTime;

    // Input
    GLFWwindow* window;

    // Graphics
    char gpuName[64];
    char openGlVersion[64];

    //ivec2 displaySize;

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

    // Shaders
    std::shared_ptr<Shader> deferredPassShader;
    std::shared_ptr<Shader> postProcessShader;

    // Lights
    std::vector<Light> lights;
    //Light dirLight;
    //Light dirLight2;

    // Buffers
    int maxUniformBufferSize;
    int uniformBlockAlignment;

    // Models
    std::shared_ptr<Framebuffer> gBufferFbo;
    std::shared_ptr<Framebuffer> deferredPassFbo;
    glm::vec2 viewportSize = glm::vec2(0);

    std::shared_ptr<Model> sphereModel;
    std::shared_ptr<Model> planeModel;
    std::shared_ptr<Shader> debugLightShader;
    bool debugDrawLights = true;

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
