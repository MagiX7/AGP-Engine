#include "Engine.h"
#include "ModelImporter.h"
#include "TexturesManager.h"
#include "Renderer/Shader.h"
#include "Renderer/Framebuffer.h"
#include "Renderer/Skybox.h"

#include <imgui.h>
#include <stb_image.h>
#include <stb_image_write.h>

#include <glm/gtx/euler_angles.hpp>

#include <iostream>
#include <random>


Image LoadImage(const char* filename)
{
    Image img = {};
    stbi_set_flip_vertically_on_load(true);
    img.pixels = stbi_load(filename, &img.size.x, &img.size.y, &img.nchannels, 0);
    if (img.pixels)
    {
        img.stride = img.size.x * img.nchannels;
    }
    else
    {
        ELOG("Could not open file %s", filename);
    }
    return img;
}

void FreeImage(Image image)
{
    stbi_image_free(image.pixels);
}

GLuint CreateTexture2DFromImage(Image image)
{
    GLenum internalFormat = GL_RGB8;
    GLenum dataFormat     = GL_RGB;
    GLenum dataType       = GL_UNSIGNED_BYTE;

    switch (image.nchannels)
    {
        case 3: dataFormat = GL_RGB; internalFormat = GL_RGB8; break;
        case 4: dataFormat = GL_RGBA; internalFormat = GL_RGBA8; break;
        default: ELOG("LoadTexture2D() - Unsupported number of channels");
    }

    GLuint texHandle;
    glGenTextures(1, &texHandle);
    glBindTexture(GL_TEXTURE_2D, texHandle);
    glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, image.size.x, image.size.y, 0, dataFormat, dataType, image.pixels);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glGenerateMipmap(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, 0);

    return texHandle;
}

Application* Application::instance = nullptr;

Application::Application()
{
    instance = this;
}

Application::~Application()
{
}

void Application::Init()
{
    camera = Camera({ 0.0f,0,20.0f }, { 0,0,0 }, 45.0f, 1280 / 720);
    
    #pragma region Lights

    lights.emplace_back(Light(LightType::DIRECTIONAL, { 1, 1, 1 }, { 1 ,1, 1 }));
    lights.emplace_back(Light(LightType::DIRECTIONAL, { -1,-1,-1 }, { 0.5, 0.5, 0.5 }));
    lights.back().SetIntensity(5.f);

    lights.emplace_back(Light(LightType::POINT, { -6, 0, 4 }, { 1.00, 0.45, 1. }));
    lights.back().SetIntensity(2.5f);
    lights.emplace_back(Light(LightType::POINT, { -6, 1, 4 }, { 1.00, 0.05, 0.95 }));
    lights.back().SetIntensity(2.5f);
    lights.emplace_back(Light(LightType::POINT, { -6, 0, -4 }, { 0.00, 0.05, 1.0 }));
    lights.back().SetIntensity(2.5f);
    lights.emplace_back(Light(LightType::POINT, { -6, 1, -4 }, { 0.05, 1.00, 0.25 }));
    lights.back().SetIntensity(2.5f);

    lights.emplace_back(Light(LightType::POINT, { 6, 0, -4 }, { 1.00, 0.65, 0.30 }));
    lights.back().SetIntensity(2.5f);
    lights.emplace_back(Light(LightType::POINT, { 7.5, 0.2, -1 }, { 0.10, 0.10, 0.60 }));
    lights.back().SetIntensity(1.5f);
    lights.emplace_back(Light(LightType::POINT, { 6.7, 1.2, -.75 }, { 0.4, 0.4, 0.4 }));
    //lights.back().SetIntensity(1.5f);


    lights.emplace_back(Light(LightType::POINT, { -1, 1, 2 }, { 0.05, 0.35, 0.65 }));
    lights.back().SetIntensity(1.5f);
    lights.emplace_back(Light(LightType::POINT, { -1, -1, 2 }, { 0.5, 0.5, 0.95 }));
    lights.back().SetIntensity(1.5f);
    lights.emplace_back(Light(LightType::POINT, { 1, 1, -2 }, { 1.05, 0.35, 0.65 }));
    lights.emplace_back(Light(LightType::POINT, { 1, -1, -2 }, { 1.05, 0.35, 0.65 }));
    lights.back().SetIntensity(1.5f);


    lights.emplace_back(Light(LightType::POINT, { 2, 1, 6 }, { 0.5, 0.55, 0.65 }));
    lights.emplace_back(Light(LightType::POINT, { 6, -1, 2 }, { 0.95, 0.00, 0.25 }));
    lights.back().SetIntensity(1.5f);
    lights.emplace_back(Light(LightType::POINT, { 6, 0, 4 }, { 0.55, 0.80, 0.25 }));
    lights.back().SetIntensity(3.5f);
    lights.emplace_back(Light(LightType::POINT, { 6, 0, 2 }, { 1.0, 0.05, 0.9 }));
    lights.back().SetIntensity(2.0f);
    lights.emplace_back(Light(LightType::POINT, { 2, -1, 0 }, { 0.75, 0.15, 0.15 }));
    lights.emplace_back(Light(LightType::POINT, { -2, 0, 0 }, { 1.0, 0, 1 }));
    lights.back().SetIntensity(2.5f);
    lights.emplace_back(Light(LightType::POINT, { 2, 0.5, 0 }, { 0.0, 1, 1 }));
    lights.back().SetIntensity(2.5f);
    lights.emplace_back(Light(LightType::POINT, { 0, 3, 0 }, { 0.0, 0.9, 0 }));
    lights.back().SetIntensity(1.5f);
    
    #pragma endregion

    #pragma region Framebuffers

    FramebufferAttachments att{ true, 4, true, true, true, true, true };
    gBufferFbo = std::make_shared<Framebuffer>(att, viewportSize.x, viewportSize.y);

    FramebufferAttachments att2{ true, 4, false, false, true };
    deferredPassFbo = std::make_shared<Framebuffer>(att2, viewportSize.x, viewportSize.y);
    postProcessFbo = std::make_shared<Framebuffer>(att2, viewportSize.x, viewportSize.y);
    
    FramebufferAttachments att3{ true, 4, false, false, false};
    skyboxFbo = std::make_shared<Framebuffer>(att3, viewportSize.x, viewportSize.y);
    
    FramebufferAttachments att4{ true, 1, false, false, false};
    ssaoFbo = std::make_shared<Framebuffer>(att4, viewportSize.x, viewportSize.y);
    blurredSsaoFbo = std::make_shared<Framebuffer>(att4, viewportSize.x, viewportSize.y);

    currentRenderTargetId = 0;

    #pragma endregion

    #pragma region Shaders and Models Loading

    texturedGeometryShader = std::make_shared<Shader>("Assets/Shaders/shaders.glsl", "TEXTURED_GEOMETRY");
    deferredPassShader = std::make_shared<Shader>("Assets/Shaders/deferred_pass.glsl", "DEFERRED");
    postProcessShader = std::make_shared<Shader>("Assets/Shaders/post_process.glsl", "POST_PROCESS");
    debugLightShader = std::make_shared<Shader>("Assets/Shaders/sphere_light.glsl", "SPHERE_LIGHT");
    ssaoShader = std::make_shared<Shader>("Assets/Shaders/ssao.glsl", "SSAO");
    blurredSsaoShader = std::make_shared<Shader>("Assets/Shaders/ssao_blur.glsl", "SSAO_BLUR");

    patrickModel = ModelImporter::ImportModel("Assets/Models/Patrick/Patrick.obj");
    //patrickModel = ModelImporter::ImportModel("Assets/Models/Cerberus/Cerberus.fbx");
    sphereModel = ModelImporter::ImportModel("Assets/Models/Sphere.fbx");
    planeModel = ModelImporter::ImportModel("Assets/Models/Plane.fbx");
    #pragma endregion

    #pragma region Entities

    //Entity m1 = Entity("Right Patrick", patrickModel);
    //m1.SetPosition({ 6,0,0 });
    //entities.emplace_back(m1);

    Entity m2 = Entity("Center Patrick", patrickModel);
    m2.SetPosition({ 0,0,0 });
    entities.emplace_back(m2);

    //Entity m3 = Entity("Left Patrick", patrickModel);
    //m3.SetPosition({ -6,0,0 });
    //entities.emplace_back(m3);

    #pragma endregion    

    skybox = std::make_shared<Skybox>("Assets/Skybox/Desert.hdr");

    glGetIntegerv(GL_MAX_UNIFORM_BLOCK_SIZE, &maxUniformBufferSize);
    glGetIntegerv(GL_UNIFORM_BUFFER_OFFSET_ALIGNMENT, &uniformBlockAlignment);
    localParamsUbo = std::make_shared<UniformBuffer>(maxUniformBufferSize, uniformBlockAlignment);
    globalParamsUbo = std::make_shared<UniformBuffer>(maxUniformBufferSize, uniformBlockAlignment);
    ssaoParamsUbo = std::make_shared<UniformBuffer>(maxUniformBufferSize, uniformBlockAlignment);

    #pragma region Screen Space Buffers

    glGenBuffers(1, &screenSpaceVbo);
    glBindBuffer(GL_ARRAY_BUFFER, screenSpaceVbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(screenSpaceVertices), screenSpaceVertices, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glGenBuffers(1, &screenSpaceIbo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, screenSpaceIbo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(quadIndices), quadIndices, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    glGenVertexArrays(1, &screenSpaceVao);
    glBindVertexArray(screenSpaceVao);
    glBindBuffer(GL_ARRAY_BUFFER, screenSpaceVbo);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(ScreenSpaceVertex), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(ScreenSpaceVertex), (void*)(sizeof(float) * 3));
    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, screenSpaceIbo);
    glBindVertexArray(0);
#pragma endregion

    diceTex = std::make_shared<Texture2D>("Assets/Textures/dice.png");

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    glEnable(GL_CULL_FACE);

    mode = Mode_Model;
    renderPath = RenderPath::DEFERRED;

    TexturesManager::LoadTextures();

    GenerateSSAOKernel();
}

void Application::Update()
{
    camera.Update(deltaTime);

    gBufferFbo->Resize(viewportSize.x, viewportSize.y);
    deferredPassFbo->Resize(viewportSize.x, viewportSize.y);
    postProcessFbo->Resize(viewportSize.x, viewportSize.y);

    //glPushDebugGroup(GL_DEBUG_SOURCE_APPLICATION, 20, -1, "SKYBOX CREATION");
    //skybox = std::make_shared<Skybox>("Assets/Skybox/Desert.hdr");
    //glPopDebugGroup();

    #pragma region Global Parameters Uniform Buffer

    globalParamsUbo->Map();
    {
        globalParamsUbo->AlignHead(uniformBlockAlignment);
        globalParamsOffset = globalParamsUbo->GetHead();

        globalParamsUbo->Push1i((int)renderPath);
        globalParamsUbo->Push1f(camera.GetNearClip());
        globalParamsUbo->Push1f(camera.GetFarClip());
        globalParamsUbo->PushVector3f(camera.GetPosition());
        globalParamsUbo->Push1i(lights.size());
        for (auto& light : lights)
        {
            globalParamsUbo->AlignHead(sizeof(glm::vec4));
            globalParamsUbo->Push1i((int)light.GetType());
            globalParamsUbo->PushVector3f(light.GetDiffuse());
            globalParamsUbo->PushVector3f(light.GetPosition());
            globalParamsUbo->Push1f(light.GetIntensity());
        }

        globalParamsSize = globalParamsUbo->GetHead() - globalParamsOffset;
    }
    globalParamsUbo->Unmap();

    #pragma endregion

    #pragma region Local Parameters Uniform Buffer

    localParamsUbo->Map();
    {
        for (auto& entity : entities)
        {
            localParamsUbo->AlignHead(uniformBlockAlignment);

            entity.localParamsOffset = localParamsUbo->GetHead();
            localParamsUbo->PushMatrix4f(entity.GetTransform());
            localParamsUbo->PushMatrix4f(camera.GetViewProj() * entity.GetTransform());

            entity.localParamsSize = localParamsUbo->GetHead() - entity.localParamsOffset;
        }
    }
    localParamsUbo->Unmap();

    #pragma endregion


    #pragma region SSAO Parameters Uniform Buffer

    ssaoParamsUbo->Map();
    {
        ssaoParamsUbo->AlignHead(uniformBlockAlignment);
        ssaoParamsOffset = ssaoParamsUbo->GetHead();

        ssaoParamsUbo->PushVector2f(viewportSize);
        ssaoParamsUbo->PushMatrix4f(camera.GetProjection());
        ssaoParamsUbo->Push1i(ssaoProps.noiseSize); // Noise Size
        ssaoParamsUbo->Push1i(ssaoKernel.size()); // Kernel Size
        ssaoParamsUbo->Push1f(ssaoProps.radius); // Hemi-Sphere Radius
        ssaoParamsUbo->Push1f(ssaoProps.bias); // Bias
        ssaoParamsUbo->Push1f(ssaoProps.strength); // Bias
        ssaoParamsUbo->Push1f(camera.GetNearClip()); // Bias
        ssaoParamsUbo->Push1f(camera.GetFarClip()); // Bias

        for (auto value : ssaoKernel)
        {
            ssaoParamsUbo->AlignHead(sizeof(glm::vec4));
            ssaoParamsUbo->PushVector3f(value);
        }

        ssaoParamsSize = ssaoParamsUbo->GetHead() - ssaoParamsOffset;
    }
    ssaoParamsUbo->Unmap();

    #pragma endregion
}

void Application::Render()
{
    gBufferFbo->Bind();
    glEnable(GL_DEPTH_TEST);

    glClearColor(0.0, 0.0, 0.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
    
    switch (mode)
    {
        case Mode_TexturedQuad:
        {
            texturedGeometryShader->Bind();
            glBindVertexArray(screenSpaceVao);

            texturedGeometryShader->SetUniform1i("uTexture", 0);
            diceTex->Bind(0);
            glDrawElements(GL_TRIANGLES, sizeof(quadIndices) / sizeof(u16), GL_UNSIGNED_SHORT, 0);

            glBindVertexArray(0);
            glUseProgram(0);

            break;
        }
        case Mode_Model:
        {
            glPushDebugGroup(GL_DEBUG_SOURCE_APPLICATION, 2, -1, "Geometry pass");

            GLuint buffs[] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3, GL_COLOR_ATTACHMENT4 };
            glDrawBuffers(5, buffs);
            globalParamsUbo->BindRange(0, globalParamsOffset, globalParamsSize);
            for (auto& entity : entities)
            {
                localParamsUbo->BindRange(1, entity.localParamsOffset, entity.localParamsSize);
                entity.GetModel()->Draw(true);
            }

            glPopDebugGroup();

            if (renderPath == RenderPath::FORWARD)
            {
                glPushDebugGroup(GL_DEBUG_SOURCE_APPLICATION, 1, -1, "Skybox pass");
                skybox->Draw(camera.GetView(), camera.GetProjection());
                glPopDebugGroup();
            }

            break;
        }

    default:
        break;
    }

    gBufferFbo->Unbind();


    #pragma region SSAO Pass

    glPushDebugGroup(GL_DEBUG_SOURCE_APPLICATION, 5, -1, "SSAO Pass");
    ssaoFbo->Bind();
    glClear(GL_COLOR_BUFFER_BIT);
    
    glDrawBuffer(GL_COLOR_ATTACHMENT0);

    ssaoShader->Bind();

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, gBufferFbo->GetNormalsAttachment());
    ssaoShader->SetUniform1i("uNormalsTexture", 0);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, gBufferFbo->GetPositionAttachment());
    ssaoShader->SetUniform1i("uPositionTexture", 1);

    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, ssaoNoiseTexture);
    ssaoShader->SetUniform1i("uNoiseTexture", 2);

    glActiveTexture(GL_TEXTURE3);
    glBindTexture(GL_TEXTURE_2D, gBufferFbo->GetDepthAttachment());
    ssaoShader->SetUniform1i("uDepthTexture", 3);

    ssaoParamsUbo->BindRange(2, ssaoParamsOffset, ssaoParamsSize);

    glDisable(GL_DEPTH_TEST);

    glDrawBuffer(GL_COLOR_ATTACHMENT0);
    glBindVertexArray(screenSpaceVao);
    glDrawElements(GL_TRIANGLES, sizeof(quadIndices) / sizeof(u16), GL_UNSIGNED_SHORT, 0);

    glEnable(GL_DEPTH_TEST);

    ssaoFbo->Unbind();
    glPopDebugGroup();



    blurredSsaoFbo->Bind();

    blurredSsaoShader->Bind();
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, ssaoFbo->GetColorAttachment());
    blurredSsaoShader->SetUniform1i("uNoiseTexture", 0);

    glDisable(GL_DEPTH_TEST);

    glDrawBuffer(GL_COLOR_ATTACHMENT0);
    glBindVertexArray(screenSpaceVao);
    glDrawElements(GL_TRIANGLES, sizeof(quadIndices) / sizeof(u16), GL_UNSIGNED_SHORT, 0);

    glEnable(GL_DEPTH_TEST);

    blurredSsaoFbo->Unbind();


    #pragma endregion




    #pragma region Deferred Pass

    if (renderPath == RenderPath::DEFERRED)
    {
        skyboxFbo->Bind();
        glPushDebugGroup(GL_DEBUG_SOURCE_APPLICATION, 1, -1, "Skybox pass");
        skybox->Draw(camera.GetView(), camera.GetProjection());
        glPopDebugGroup();
        skyboxFbo->Unbind();


        glPushDebugGroup(GL_DEBUG_SOURCE_APPLICATION, 3, -1, "Deferred pass");
        
        deferredPassFbo->Bind();
        glClearColor(0.08, 0.08, 0.08, 1);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        deferredPassShader->Bind();
        deferredPassShader->SetUniform1i("renderTarget", currentRenderTargetId);
        deferredPassShader->SetUniform1i("renderMode", currentRenderTargetId);
        glBindVertexArray(screenSpaceVao);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, gBufferFbo->GetColorAttachment());
        deferredPassShader->SetUniform1i("uColorTexture", 0);

        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, gBufferFbo->GetNormalsAttachment());
        deferredPassShader->SetUniform1i("uNormalsTexture", 1);

        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, gBufferFbo->GetPositionAttachment());
        deferredPassShader->SetUniform1i("uPositionTexture", 2);

        

        glActiveTexture(GL_TEXTURE3);
        glBindTexture(GL_TEXTURE_2D, gBufferFbo->GetMetallicAttachment());
        deferredPassShader->SetUniform1i("uMetallicMap", 3);

        glActiveTexture(GL_TEXTURE4);
        glBindTexture(GL_TEXTURE_2D, gBufferFbo->GetRoughnessAttachment());
        deferredPassShader->SetUniform1i("uRoughnessMap", 4);

        // Skybox
        skybox->BindIrradianceMap(5);
        deferredPassShader->SetUniform1i("uIrradianceMap", 5);

        skybox->BindPrefilterMap(6);
        deferredPassShader->SetUniform1i("uSkyboxPrefilterMap", 6);

        skybox->BindBRDF(7);
        deferredPassShader->SetUniform1i("uSkyboxBrdf", 7);

        glActiveTexture(GL_TEXTURE8);
        glBindTexture(GL_TEXTURE_2D, skyboxFbo->GetColorAttachment());
        deferredPassShader->SetUniform1i("uSkybox", 8);

        deferredPassShader->SetUniform1i("uSsaoEnabled", ssaoProps.enabled);
        glActiveTexture(GL_TEXTURE9);
        glBindTexture(GL_TEXTURE_2D, blurredSsaoFbo->GetColorAttachment());
        deferredPassShader->SetUniform1i("uSSAOTexture", 9);

        glActiveTexture(GL_TEXTURE10);
        glBindTexture(GL_TEXTURE_2D, gBufferFbo->GetDepthAttachment());
        deferredPassShader->SetUniform1i("uDepthTexture", 10);


        globalParamsUbo->BindRange(0, globalParamsOffset, globalParamsSize);

        glDisable(GL_DEPTH_TEST);

        glDrawBuffer(GL_COLOR_ATTACHMENT0);
        glBindVertexArray(screenSpaceVao);
        glDrawElements(GL_TRIANGLES, sizeof(quadIndices) / sizeof(u16), GL_UNSIGNED_SHORT, 0);
        
        glEnable(GL_DEPTH_TEST);

        deferredPassFbo->Unbind();
        glPopDebugGroup();
    }

    #pragma endregion

    // The pass that collects all the textures and checks for the current render target to output it
    // Eventually, post processing computations will only be done if the current render target is the albedo attachment
    #pragma region Post Processing Pass

    postProcessFbo->Bind();
    glPushDebugGroup(GL_DEBUG_SOURCE_APPLICATION, 4, -1, "Post-Processing Pass");

    //glClearColor(0.08, 0.08, 0.08, 1);
    //glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    //glDrawBuffer(GL_COLOR_ATTACHMENT0);

    postProcessShader->Bind();
    postProcessShader->SetUniform1i("renderTarget", currentRenderTargetId);
    postProcessShader->SetUniform1i("uSsaoEnabled", ssaoProps.enabled);
    postProcessShader->SetUniform1f("uNear", camera.GetNearClip());
    postProcessShader->SetUniform1f("uFar", camera.GetFarClip());

    glBindVertexArray(screenSpaceVao);
    bool isDeferred = renderPath == RenderPath::DEFERRED;

    // Since the deferred pass only outputs a color texture, we only need to check wether to use one or another.
    // For the rest it's safe to get them from the G-buffer

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, isDeferred ? deferredPassFbo->GetColorAttachment() : gBufferFbo->GetColorAttachment());
    postProcessShader->SetUniform1i("uColorTexture", 0);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, gBufferFbo->GetNormalsAttachment());
    postProcessShader->SetUniform1i("uNormalsTexture", 1);

    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, gBufferFbo->GetPositionAttachment());
    postProcessShader->SetUniform1i("uPositionTexture", 2);

    glActiveTexture(GL_TEXTURE3);
    glBindTexture(GL_TEXTURE_2D, gBufferFbo->GetDepthAttachment());
    postProcessShader->SetUniform1i("uDepthTexture", 3);

    glActiveTexture(GL_TEXTURE4);
    glBindTexture(GL_TEXTURE_2D, ssaoNoiseTexture);
    postProcessShader->SetUniform1i("uNoiseTexture", 4);
    

    ssaoParamsUbo->BindRange(2, ssaoParamsOffset, ssaoParamsSize);

    glDisable(GL_DEPTH_TEST);
    glDrawElements(GL_TRIANGLES, sizeof(quadIndices) / sizeof(u16), GL_UNSIGNED_SHORT, 0);
    glEnable(GL_DEPTH_TEST);

    glBindVertexArray(0);
    glUseProgram(0);
    postProcessShader->Unbind();
    
    if (debugDrawLights)
    {
        DebugDrawLights();
    }

    glPopDebugGroup();
    postProcessFbo->Unbind();

    #pragma endregion

}

void Application::OnImGuiRender()
{
    #pragma region Main Menu Bar

    ImGui::BeginMainMenuBar();
    {
        if (ImGui::BeginMenu("View"))
        {
            if (ImGui::MenuItem("OpenGL Info"))
            {
                showExtensionsPanel = true;
            }
            if (ImGui::BeginMenu("Lights"))
            {
                ImGui::Checkbox("Debug Lights", &debugDrawLights);
                ImGui::DragFloat("Sphere Size", &sphereLightsSize, 0.01f, 0.0f, 1.0f);

                ImGui::EndMenu();
            }            
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Renderer"))
        {
            if (ImGui::MenuItem("Show Render Panel"))
                showRenderOptionsPanel = true;

            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Create"))
        {
            if (ImGui::BeginMenu("Light"))
            {
                if (ImGui::MenuItem("Directional"))
                {
                    lights.emplace_back(LightType::DIRECTIONAL, glm::normalize(glm::vec3(1)), glm::vec3(1));
                }
                if (ImGui::MenuItem("Point"))
                {
                    lights.emplace_back(LightType::POINT, glm::vec3(0), glm::vec3(1));
                }
                if (ImGui::MenuItem("10 Point"))
                {
                    std::srand(std::time(nullptr));
                    for (int i = 0; i < 10; ++i)
                    {
                        int r = std::rand();
                        int g = std::rand();
                        int b = std::rand();
                        lights.emplace_back(LightType::POINT, glm::vec3(i * 2.0f - 10.0f, 0, 2.0f), glm::normalize(glm::vec3(r, g, b)));
                    }
                }

                ImGui::EndMenu();
            }

            ImGui::EndMenu();
        }
    }
    ImGui::EndMainMenuBar();


    if (showRenderOptionsPanel)
    {
        ImGui::Begin("Renderer", &showRenderOptionsPanel);
        {
            {
                const char* items[] = { "Forward", "Deferred" };
                static int currentIndex = 1;
                const char* combLabel = items[currentIndex];
                if (ImGui::BeginCombo("Rendering Path", combLabel))
                {
                    for (int i = 0; i < IM_ARRAYSIZE(items); i++)
                    {
                        const bool isSelected = (currentIndex == i);
                        if (ImGui::Selectable(items[i], isSelected))
                        {
                            currentIndex = i;
                            renderPath = (RenderPath)i;
                        }

                        if (isSelected) ImGui::SetItemDefaultFocus();
                    }
                    ImGui::EndCombo();
                }
            }

            {
                const char* items[] = { "Albedo", "Normals", "Position", "Depth" };
                static int currentItemIndex = 0;
                const char* combLabel = items[currentItemIndex];
                if (ImGui::BeginCombo("Render Target", combLabel))
                {
                    for (int i = 0; i < IM_ARRAYSIZE(items); i++)
                    {
                        const bool isSelected = (currentItemIndex == i);
                        if (ImGui::Selectable(items[i], isSelected))
                            currentRenderTargetId = currentItemIndex = i;

                        if (isSelected) ImGui::SetItemDefaultFocus();
                    }
                    ImGui::EndCombo();
                }
            }
        }
        ImGui::End();
    }

    if (showExtensionsPanel)
    {
        ImGui::Begin("OpenGL Information", &showExtensionsPanel);
        
        const char* version = reinterpret_cast<const char*>(glGetString(GL_VERSION));
        ImGui::Text(std::string("Version: " + std::string(version)).c_str());

        const char* vendor = reinterpret_cast<const char*>(glGetString(GL_VENDOR));
        ImGui::Text(std::string("Vendor: " + std::string(vendor)).c_str());

        const char* renderer = reinterpret_cast<const char*>(glGetString(GL_RENDERER));
        ImGui::Text(std::string("Renderer: " + std::string(renderer)).c_str());

        const char* glslVersion = reinterpret_cast<const char*>(glGetString(GL_SHADING_LANGUAGE_VERSION));
        ImGui::Text(std::string("GLSL Version: " + std::string(glslVersion)).c_str());

        if (ImGui::TreeNodeEx("Extensions:"))
        {
            GLint numExt = -1;
            glGetIntegerv(GL_NUM_EXTENSIONS, &numExt);
            for (int i = 0; i < numExt; ++i)
            {
                const unsigned char* ext = glGetStringi(GL_EXTENSIONS, (GLuint)i);
                ImGui::Indent();
                ImGui::BulletText((const char*)ext);
                ImGui::Unindent();
            }
            ImGui::TreePop();
        }

        ImGui::End();
    }

    #pragma endregion

    ImGui::Begin("Viewport");
    {
        ImVec2 dimensions = ImGui::GetContentRegionAvail();
        if (viewportSize.x != dimensions.x || viewportSize.y != dimensions.y)
        {
            gBufferFbo->Resize(dimensions.x, dimensions.y);
            glViewport(0, 0, dimensions.x, dimensions.y);
            camera.SetViewportSize((uint32_t)dimensions.x, (uint32_t)dimensions.y);
            viewportSize = { dimensions.x, dimensions.y };
        }
        
        // Only one texture is outputted. The checking of the render target is done inside the shader
        int id = postProcessFbo->GetColorAttachment();
        ImGui::Image((ImTextureID*)id, { viewportSize.x, viewportSize.y }, { 0,1 }, { 1,0 });
    }
    ImGui::End();

    ImGui::Begin("Info");
    ImGui::Text("FPS: %f", 1.0f / deltaTime);
    ImGui::Text("Lights Count: %i", lights.size());
    ImGui::End();

    ImGui::Begin("Camera");
    {
        auto& pos = camera.GetPosition();
        if (ImGui::DragFloat3("Position", glm::value_ptr(pos), 0.01f))
            camera.SetPosition(pos);

        auto& rot = camera.GetRotation();
        if (ImGui::DragFloat3("Rotation", glm::value_ptr(rot), 0.01f))
            camera.SetRotation(rot);

        auto near = camera.GetNearClip();
        if (ImGui::DragFloat("Near Clip", &near))
            camera.SetNearClip(near);

        auto far = camera.GetFarClip();
        if (ImGui::DragFloat("Far Clip", &far))
            camera.SetFarClip(far);

        auto verticalFov = camera.GetVerticalFov();
        if (ImGui::DragFloat("Vertical FOV", &verticalFov))
            camera.SetVerticalFov(verticalFov);

        ImGui::Text("Aspect Ratio: %f", camera.GetAspectRatio());
    }
    ImGui::End();

    ImGui::Begin("Hierarchy");
    {
        bool selected = false;
        for (auto& entity : entities)
        {
            if (ImGui::Selectable(entity.GetName().c_str(), &selected))
            {
                currentEntity = &entity;
            }
        }
    }
    ImGui::End();

    ImGui::Begin("Lighting");
    {
        int idToDelete = -1;
        for (int i = 0; i < lights.size(); ++i)
        {
            ImGui::PushID(i);

            auto& light = lights[i];
            if (ImGui::CollapsingHeader(light.GetName().c_str()))
            {
                light.OnImGuiRender();
                ImGui::Separator();
                if (ImGui::Button("Delete"))
                    idToDelete = i;
            }

            ImGui::PopID();
        }

        if (idToDelete >= 0)
            lights.erase(lights.begin() + idToDelete);
    }
    ImGui::End();

    ImGui::Begin("Model");
    {
        if (currentEntity)
        {
            static std::shared_ptr<Material> material = nullptr;
            static int texIndex = -1;

            auto& pos = currentEntity->GetPosition();
            if (ImGui::DragFloat3("Position", glm::value_ptr(pos), 0.01f))
                currentEntity->SetPosition(pos);

            auto rot = glm::degrees(currentEntity->GetRotation());
            if (ImGui::DragFloat3("Rotation", glm::value_ptr(rot), 0.1f))
                currentEntity->SetRotation(glm::radians(rot));

            auto& scale = currentEntity->GetScale();
            if (ImGui::DragFloat3("Scale", glm::value_ptr(scale), 0.01f))
                currentEntity->SetScale(scale);

            ImGui::Separator();
            ImGui::Separator();

            if (ImGui::CollapsingHeader("Material"))
            {
                int i = 0;

                for (auto mesh : currentEntity->GetModel()->GetMeshes())
                {
                    auto mat = mesh->GetMaterial();
                    ImGui::Indent();
                    if (ImGui::CollapsingHeader(mesh->GetName().c_str()))
                    {
                        ImGui::PushID(i);
                        if (ImGui::TreeNodeEx("Material"))
                        {
                            ImGui::Text("Albedo Map:");
                            if (auto albedoMap = mat->GetAlbedoMap())
                                ImGui::Image((ImTextureID)albedoMap->GetId(), { 128,128 });
                            if (ImGui::Button("Change"))
                            {
                                showTexturesPanel = true;
                                material = mat;
                                texIndex = 0;
                            }
                            ImGui::SameLine();
                            if (ImGui::Button("Remove"))
                            {
                                mat->SetAlbedoMap(nullptr);
                            }

                            ImGui::Dummy({ 0,3.5 });

                            ImGui::Text("Normal Map:");
                            if (auto normalMap = mat->GetNormalMap())
                                ImGui::Image((ImTextureID)normalMap->GetId(), { 128,128 });
                            if (ImGui::Button("Change##1"))
                            {
                                showTexturesPanel = true;
                                material = mat;
                                texIndex = 1;
                            }
                            ImGui::SameLine();
                            if (ImGui::Button("Remove##1"))
                            {
                                mat->SetNormalMap(nullptr);
                            }
                            
                            ImGui::Dummy({ 0,3.5 });

                            ImGui::Text("Metallic Map:");
                            if (auto metallicMap = mat->GetMetallicMap())
                            {
                                ImGui::Image((ImTextureID)metallicMap->GetId(), { 128,128 });
                            }
                            if (ImGui::Button("Change##2"))
                            {
                                showTexturesPanel = true;
                                material = mat;
                                texIndex = 2;
                            }
                            ImGui::SameLine();
                            if (ImGui::Button("Remove##2"))
                            {
                                mat->SetMetallicMap(nullptr);
                            }

                            ImGui::Dummy({ 0,3.5 });

                            ImGui::Text("Roughness Map:");
                            if (auto specularMap = mat->GetRoughnessMap())
                            {
                                ImGui::Image((ImTextureID)specularMap->GetId(), { 128,128 });
                            }
                            if (ImGui::Button("Change##3"))
                            {
                                showTexturesPanel = true;
                                material = mat;
                                texIndex = 3;
                            }
                            ImGui::SameLine();
                            if (ImGui::Button("Remove##3"))
                            {
                                mat->SetRoughnessMap(nullptr);
                            }

                            ImGui::Dummy({ 0,3.5 });
                            ImGui::Separator();
                            ImGui::Dummy({ 0,1.75 });

                            ImGui::TreePop();
                        }
                        ImGui::PopID();

                    }
                    ImGui::Unindent();
                    ++i;
                }
            }

            if (texIndex >= 0)
            {
                if (ShowTexturesPanel(material, texIndex))
                {
                    material = nullptr;
                    texIndex = -1;
                }

            }

            //if (texIndex >= 0)
            //{
            //    //ImGui::Begin("Textures", &showTexturesPanel);

            //}
        }
    }
    ImGui::End();



    ImGui::Begin("SSAO");
    {
        ImGui::Checkbox("Enabled", &ssaoProps.enabled);
        ImGui::DragFloat("Radius", &ssaoProps.radius, 0.01f, 0.001f);
        ImGui::DragFloat("Strength", &ssaoProps.strength, 0.01f, 0.001f);
        ImGui::DragFloat("Bias", &ssaoProps.bias, 0.001f, 0.01f);
        ImGui::DragInt("Noise Size", &ssaoProps.noiseSize, 0.1f, 0);
    }
    ImGui::End();

}

void Application::OnWindowResized(const glm::vec2& dimensions)
{
    gBufferFbo->Resize(dimensions.x, dimensions.y);
    glViewport(0, 0, dimensions.x, dimensions.y);
    camera.SetViewportSize((uint32_t)dimensions.x, (uint32_t)dimensions.y);
    viewportSize = { dimensions.x, dimensions.y };
}

void Application::DebugDrawLights()
{
    glBindFramebuffer(GL_READ_FRAMEBUFFER, gBufferFbo->GetId());
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, postProcessFbo->GetId());
    glBlitFramebuffer(
        0, 0, gBufferFbo->GetSize().x, gBufferFbo->GetSize().y, 0, 0, postProcessFbo->GetSize().x, postProcessFbo->GetSize().y, GL_DEPTH_BUFFER_BIT, GL_NEAREST
    );

    // Disable Cull face because at some orientations dir light's quad are culled and can't be seen
    glDisable(GL_CULL_FACE);
    glDepthMask(GL_FALSE);

    glPushDebugGroup(GL_DEBUG_SOURCE_APPLICATION, 2, -1, "Debug Light Spheres");

    glm::mat4 sphereScale = glm::scale(glm::mat4(1.0), glm::vec3(sphereLightsSize));

    // Initialized after declaration because otherwise it keeps adding to the position.
    // This way it resets every frame.
    static float offset;
    offset = 0;

    for (auto& light : lights)
    {
        debugLightShader->Bind();
        debugLightShader->SetUniformMatrix4f("viewProj", camera.GetViewProj());
        debugLightShader->SetUniformVec3f("lightColor", light.GetDiffuse());
        if (light.GetType() == LightType::DIRECTIONAL)
        {
            // Draw Quad
            glm::vec3 dirLightPos = glm::vec3(-1 + offset, 4, 0);
            glm::vec3 rot = light.GetPosition();
            glm::mat4 transform = glm::translate(glm::mat4(1.0), dirLightPos)
                * glm::eulerAngleXYZ(rot.x, rot.y, rot.z);

            debugLightShader->SetUniformMatrix4f("model", transform);

            planeModel->Draw(false);
            offset += 2.0f;
        }
        else if (light.GetType() == LightType::POINT)
        {
            // Draw Sphere
            glm::mat4 transform = glm::translate(glm::mat4(1.0), light.GetPosition()) * sphereScale;
            debugLightShader->SetUniformMatrix4f("model", transform);

            sphereModel->Draw(false);
        }
        debugLightShader->Unbind();
    }

    //glEnable(GL_DEPTH_TEST);
    //glDepthMask(GL_TRUE);
    glEnable(GL_CULL_FACE);
    glDepthMask(GL_TRUE);

    glPopDebugGroup();
}

bool Application::ShowTexturesPanel(std::shared_ptr<Material> material, int texIndex)
{
    bool ret = false;

    if (ImGui::Begin("Textures", &showTexturesPanel))
    {
        if (ImGui::BeginTable("", 2))
        {
            for (const auto texture : TexturesManager::GetTextures())
            {
                ImGui::TableNextColumn();
                if (ImGui::ImageButton((ImTextureID)texture->GetId(), { 128,128 }))
                {
                    switch (texIndex)
                    {
                        case 0: material->SetAlbedoMap(texture); ret = true; break;
                        case 1: material->SetNormalMap(texture); ret = true; break;
                        case 2: material->SetMetallicMap(texture); ret = true; break;
                        case 3: material->SetRoughnessMap(texture); ret = true; break;
                    }
                }
                ImGui::Text(texture->GetName().c_str());
            }
            ImGui::EndTable();
        }

        ImGui::End();
    }

    return ret;
}

void Application::GenerateSSAOKernel()
{
    std::uniform_real_distribution<float> randomFloats(0.0, 1.0); // random floats between [0.0, 1.0]
    std::default_random_engine generator;
    for (unsigned int i = 0; i < 64; ++i)
    {
        glm::vec3 sample(
            randomFloats(generator) * 2.0 - 1.0,
            randomFloats(generator) * 2.0 - 1.0,
            randomFloats(generator)
        );
        float scale = (float)i / 64.0;
        scale = LerpFloat(0.1f, 1.0f, scale * scale);
        sample *= scale;
        ssaoKernel.push_back(sample);
    }

    for (unsigned int i = 0; i < 16; i++)
    {
        glm::vec3 noise(
            randomFloats(generator) * 2.0 - 1.0,
            randomFloats(generator) * 2.0 - 1.0,
            0.0f);
        ssaoNoise.push_back(noise);
    }

    glGenTextures(1, &ssaoNoiseTexture);
    glBindTexture(GL_TEXTURE_2D, ssaoNoiseTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, 4, 4, 0, GL_RGB, GL_FLOAT, &ssaoNoise[0]);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

}

float Application::LerpFloat(float a, float b, float f)
{
    return a + f * (b - a);
}
