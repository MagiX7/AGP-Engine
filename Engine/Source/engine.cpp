#include "Engine.h"
#include "ModelImporter.h"
#include "Renderer/Shader.h"
#include "Renderer/Framebuffer.h"

#include <imgui.h>
#include <stb_image.h>
#include <stb_image_write.h>

#include <glm/gtx/euler_angles.hpp>

#include <iostream>


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

//void SetShaderUniforms(Application* app, int shaderIndex)
//{
//    Program& shader = app->programs[shaderIndex];
//    int attribCount = -1;
//    glGetProgramiv(shader.handle, GL_ACTIVE_ATTRIBUTES, &attribCount);
//    for (int i = 0; i < attribCount; ++i)
//    {
//        GLchar attribName[64] = {};
//        int attribNameLength;
//        GLint attribSize;
//        GLenum type = -1;
//        glGetActiveAttrib(shader.handle, i, ARRAY_COUNT(attribName), &attribNameLength, &attribSize, &type, attribName);
//
//        u8 attribLoc = glGetAttribLocation(shader.handle, attribName);
//        
//        // How to get component count??????
//        shader.vertexInputLayout.attributes.push_back({ attribLoc, (u8)attribSize });
//
//        int a = 0;
//        a += 9;
//    }
//}


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

    FramebufferAttachments att{ true, true, true, true };
    gBufferFbo = std::make_shared<Framebuffer>(att, viewportSize.x, viewportSize.y);

    FramebufferAttachments att2{ true, false, false, false };
    deferredPassFbo = std::make_shared<Framebuffer>(att, viewportSize.x, viewportSize.y);
    
    currentRenderTargetId = 0;

    texturedGeometryShader = std::make_shared<Shader>("Assets/Shaders/shaders.glsl", "TEXTURED_GEOMETRY");
    deferredPassShader = std::make_shared<Shader>("Assets/Shaders/deferred_pass.glsl", "DEFERRED");
    postProcessShader = std::make_shared<Shader>("Assets/Shaders/post_process.glsl", "POST_PROCESS");
    debugLightShader = std::make_shared<Shader>("Assets/Shaders/sphere_light.glsl", "SPHERE_LIGHT");

    //SetShaderUniforms(this, texturedGeometryShaderIdx);
    
    //patrickModel = ModelImporter::ImportModel("Assets/Models/Cerberus/Cerberus.fbx");
    //patrickModel->GetFirstMaterial()->SetNormalMap(std::make_shared<Texture2D>("Assets/Models/Cerberus/Textures/N.tga.png"));
    patrickModel = ModelImporter::ImportModel("Assets/Models/Patrick/Patrick.obj");
    sphereModel = ModelImporter::ImportModel("Assets/Models/Sphere.fbx");
    planeModel = ModelImporter::ImportModel("Assets/Models/Plane.fbx");
    

    Entity m1 = Entity("Right Patrick", patrickModel);
    m1.SetPosition({ 6,0,0 });
    entities.emplace_back(m1);

    Entity m2 = Entity("Center Patrick", patrickModel);
    m2.SetPosition({ 0,0,0 });
    entities.emplace_back(m2);

    Entity m3 = Entity("Left Patrick", patrickModel);
    m3.SetPosition({ -6,0,0 });
    entities.emplace_back(m3);
    

    glGetIntegerv(GL_MAX_UNIFORM_BLOCK_SIZE, &maxUniformBufferSize);
    glGetIntegerv(GL_UNIFORM_BUFFER_OFFSET_ALIGNMENT, &uniformBlockAlignment);
    localParamsUbo = std::make_shared<UniformBuffer>(maxUniformBufferSize, uniformBlockAlignment);
    globalParamsUbo = std::make_shared<UniformBuffer>(maxUniformBufferSize, uniformBlockAlignment);

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
    glEnable(GL_CULL_FACE);

    mode = Mode_Model;
    renderPath = RenderPath::DEFERRED;
}

void Application::Update()
{
    camera.Update(deltaTime);

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

}

void Application::Render()
{
    gBufferFbo->Bind();
    glEnable(GL_DEPTH_TEST);

    glClearColor(0.0, 0.0, 0.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    GLuint buffs[] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3 };
    glDrawBuffers(4, buffs);
    
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
            glPushDebugGroup(GL_DEBUG_SOURCE_APPLICATION, 1, -1, "Geometry pass");
            globalParamsUbo->BindRange(0, globalParamsOffset, globalParamsSize);

            for (auto& entity : entities)
            {
                localParamsUbo->BindRange(1, entity.localParamsOffset, entity.localParamsSize);
                entity.GetModel()->Draw(true);
            }

            if (debugDrawLights)
            {
                // Disable Cull face because at some orientations dir light's quad are culled and can't be seen
                // And depth testing because we don't want it to be in the depth buffer
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

                glDepthMask(GL_TRUE);
                glEnable(GL_CULL_FACE);

                glPopDebugGroup();
            }

            glPopDebugGroup();

            break;
        }

    default:
        break;
    }

    gBufferFbo->Unbind();

    if (renderPath == RenderPath::DEFERRED)
    {
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
        glBindTexture(GL_TEXTURE_2D, gBufferFbo->GetDepthAttachment());
        deferredPassShader->SetUniform1i("uDepthTexture", 3);

        globalParamsUbo->BindRange(0, globalParamsOffset, globalParamsSize);

        glDrawBuffer(GL_COLOR_ATTACHMENT0);
        glBindVertexArray(screenSpaceVao);
        glDrawElements(GL_TRIANGLES, sizeof(quadIndices) / sizeof(u16), GL_UNSIGNED_SHORT, 0);

        deferredPassFbo->Unbind();
        glPopDebugGroup();

    }

    glPushDebugGroup(GL_DEBUG_SOURCE_APPLICATION, 4, -1, "Post-Processing Pass");

    postProcessShader->Bind();
    glDisable(GL_DEPTH_TEST);

    glClearColor(0.08, 0.08, 0.08, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glDrawBuffer(GL_COLOR_ATTACHMENT0);

    postProcessShader->Bind();
    postProcessShader->SetUniform1i("renderTarget", currentRenderTargetId);
    glBindVertexArray(screenSpaceVao);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, deferredPassFbo->GetColorAttachment());
    postProcessShader->SetUniform1i("uColorTexture", 0);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, deferredPassFbo->GetNormalsAttachment());
    postProcessShader->SetUniform1i("uNormalsTexture", 1);

    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, deferredPassFbo->GetPositionAttachment());
    postProcessShader->SetUniform1i("uPositionTexture", 2);

    glActiveTexture(GL_TEXTURE3);
    glBindTexture(GL_TEXTURE_2D, deferredPassFbo->GetDepthAttachment());
    postProcessShader->SetUniform1i("uDepthTexture", 3);

    glDrawElements(GL_TRIANGLES, sizeof(quadIndices) / sizeof(u16), GL_UNSIGNED_SHORT, 0);

    glBindVertexArray(0);
    glUseProgram(0);
    postProcessShader->Unbind();
    
    glPopDebugGroup();

}

void Application::OnImGuiRender()
{
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
        
        int id = -1;
        if (renderPath == RenderPath::FORWARD)
        {
            switch(currentRenderTargetId)
            {
                case 0: id = gBufferFbo->GetColorAttachment(); break;
                case 1: id = gBufferFbo->GetNormalsAttachment();  break;
                case 2: id = gBufferFbo->GetPositionAttachment(); break;
                case 3: id = gBufferFbo->GetDepthAttachment();  break;
            }
        }
        else
        {
            // Only one texture is outputted. The checking of the render target is done inside the shader
            id = deferredPassFbo->GetColorAttachment();
        }
        
        ImGui::Image((ImTextureID*)id, { viewportSize.x, viewportSize.y }, { 0,1 }, { 1,0 });
    }
    ImGui::End();

    ImGui::Begin("Info");
    ImGui::Text("FPS: %f", 1.0f / deltaTime);
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

    ImGui::Begin("Model");
    {
        if (currentEntity)
        {
            auto& pos = currentEntity->GetPosition();
            if (ImGui::DragFloat3("Position", glm::value_ptr(pos), 0.01f))
                currentEntity->SetPosition(pos);

            auto& rot = currentEntity->GetRotation();
            if (ImGui::DragFloat3("Rotation", glm::value_ptr(rot), 0.01f))
                currentEntity->SetRotation(rot);

            auto& scale = currentEntity->GetScale();
            if (ImGui::DragFloat3("Scale", glm::value_ptr(scale), 0.01f))
                currentEntity->SetScale(scale);

            ImGui::Separator();
            ImGui::Separator();

            if (ImGui::TreeNodeEx("Materials"))
            {
                for (auto mat : currentEntity->GetModel()->GetMaterials())
                {
                    if (ImGui::Button(mat->GetName().c_str()))
                    {

                    }
                }
                ImGui::TreePop();
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

    //dirLight.OnImGuiRender();

}

void Application::OnWindowResized(const glm::vec2& dimensions)
{
    gBufferFbo->Resize(dimensions.x, dimensions.y);
    glViewport(0, 0, dimensions.x, dimensions.y);
    camera.SetViewportSize((uint32_t)dimensions.x, (uint32_t)dimensions.y);
    viewportSize = { dimensions.x, dimensions.y };
}
