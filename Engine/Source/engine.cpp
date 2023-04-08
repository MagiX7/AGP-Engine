//
// engine.cpp : Put all your graphics stuff in this file. This is kind of the graphics module.
// In here, you should type all your OpenGL commands, and you can also type code to handle
// input platform events (e.g to move the camera or react to certain shortcuts), writing some
// graphics related GUI options, and so on.
//

#include "engine.h"
#include "ModelImporter.h"
#include "Renderer/Shader.h"
#include "Renderer/Framebuffer.h"

#include <imgui.h>
#include <stb_image.h>
#include <stb_image_write.h>

#include <iostream>

//GLuint CreateProgramFromSource(String programSource, const char* shaderName)
//{
//    GLchar  infoLogBuffer[1024] = {};
//    GLsizei infoLogBufferSize = sizeof(infoLogBuffer);
//    GLsizei infoLogSize;
//    GLint   success;
//
//    char versionString[] = "#version 430\n";
//    char shaderNameDefine[128];
//    sprintf(shaderNameDefine, "#define %s\n", shaderName);
//    char vertexShaderDefine[] = "#define VERTEX\n";
//    char fragmentShaderDefine[] = "#define FRAGMENT\n";
//
//    const GLchar* vertexShaderSource[] = {
//        versionString,
//        shaderNameDefine,
//        vertexShaderDefine,
//        programSource.str
//    };
//    const GLint vertexShaderLengths[] = {
//        (GLint) strlen(versionString),
//        (GLint) strlen(shaderNameDefine),
//        (GLint) strlen(vertexShaderDefine),
//        (GLint) programSource.len
//    };
//    const GLchar* fragmentShaderSource[] = {
//        versionString,
//        shaderNameDefine,
//        fragmentShaderDefine,
//        programSource.str
//    };
//    const GLint fragmentShaderLengths[] = {
//        (GLint) strlen(versionString),
//        (GLint) strlen(shaderNameDefine),
//        (GLint) strlen(fragmentShaderDefine),
//        (GLint) programSource.len
//    };
//
//    GLuint vshader = glCreateShader(GL_VERTEX_SHADER);
//    int a = ARRAY_COUNT(vertexShaderSource);
//    glShaderSource(vshader, a, vertexShaderSource, vertexShaderLengths);
//    glCompileShader(vshader);
//    glGetShaderiv(vshader, GL_COMPILE_STATUS, &success);
//    if (!success)
//    {
//        glGetShaderInfoLog(vshader, infoLogBufferSize, &infoLogSize, infoLogBuffer);
//        ELOG("glCompileShader() failed with vertex shader %s\nReported message:\n%s\n", shaderName, infoLogBuffer);
//    }
//    
//    GLuint fshader = glCreateShader(GL_FRAGMENT_SHADER);
//    a = ARRAY_COUNT(fragmentShaderSource);
//    glShaderSource(fshader, a, fragmentShaderSource, fragmentShaderLengths);
//    glCompileShader(fshader);
//    glGetShaderiv(fshader, GL_COMPILE_STATUS, &success);
//    if (!success)
//    {
//        glGetShaderInfoLog(fshader, infoLogBufferSize, &infoLogSize, infoLogBuffer);
//        ELOG("glCompileShader() failed with fragment shader %s\nReported message:\n%s\n", shaderName, infoLogBuffer);
//    }
//
//    GLuint programHandle = glCreateProgram();
//    glAttachShader(programHandle, vshader);
//    glAttachShader(programHandle, fshader);
//    glLinkProgram(programHandle);
//    glGetProgramiv(programHandle, GL_LINK_STATUS, &success);
//    if (!success)
//    {
//        glGetProgramInfoLog(programHandle, infoLogBufferSize, &infoLogSize, infoLogBuffer);
//        ELOG("glLinkProgram() failed with program %s\nReported message:\n%s\n", shaderName, infoLogBuffer);
//    }
//
//    glUseProgram(0);
//
//    glDetachShader(programHandle, vshader);
//    glDetachShader(programHandle, fshader);
//    glDeleteShader(vshader);
//    glDeleteShader(fshader);
//
//    std::cout << shaderName << " Shader loaded successfully." << std::endl;
//
//    return programHandle;
//}

//u32 LoadProgram(Application* app, const char* filepath, const char* programName)
//{
//    String programSource = ReadTextFile(filepath);
//
//    Program program = {};
//    program.handle = CreateProgramFromSource(programSource, programName);
//    program.filepath = filepath;
//    program.programName = programName;
//    program.lastWriteTimestamp = GetFileLastWriteTimestamp(filepath);
//    app->programs.push_back(program);
//
//    return app->programs.size() - 1;
//}

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

//u32 LoadTexture2D(Application* app, const char* filepath)
//{
//    for (u32 texIdx = 0; texIdx < app->textures.size(); ++texIdx)
//        if (app->textures[texIdx].filepath == filepath)
//            return texIdx;
//
//    Image image = LoadImage(filepath);
//
//    if (image.pixels)
//    {
//        Texture tex = {};
//        tex.handle = CreateTexture2DFromImage(image);
//        tex.filepath = filepath;
//
//        u32 texIdx = app->textures.size();
//        app->textures.push_back(tex);
//
//        FreeImage(image);
//        return texIdx;
//    }
//    else
//    {
//        return UINT32_MAX;
//    }
//}

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


//void Init(Application* app)
//{
//    app->camera = Camera({ 0,2,2 }, { 0,0,0 }, 45.0f, 1280 / 720);
//
//    app->texturedGeometryShaderIdx = LoadProgram(app, "Assets/Shaders/shaders.glsl", "TEXTURED_GEOMETRY");
//    app->modelShaderIndex = LoadProgram(app, "Assets/Shaders/model.glsl", "MODEL");
//    
//    SetShaderUniforms(app, app->texturedGeometryShaderIdx);
//
//    app->patrickModel = ModelImporter::ImportModel("Assets/Models/Cerberus/Cerberus.fbx");
//
//
//    glGetIntegerv(GL_MAX_UNIFORM_BLOCK_SIZE, &app->maxUniformBufferSize);
//    glGetIntegerv(GL_UNIFORM_BUFFER_OFFSET_ALIGNMENT, &app->uniformBlockAlignment);
//
//
//    glGenBuffers(1, &app->screenSpaceVbo);
//    glBindBuffer(GL_ARRAY_BUFFER, app->screenSpaceVbo);
//    glBufferData(GL_ARRAY_BUFFER, sizeof(screenSpaceVertices), screenSpaceVertices, GL_STATIC_DRAW);
//    glBindBuffer(GL_ARRAY_BUFFER, 0);
//    
//    glGenBuffers(1, &app->screenSpaceIbo);
//    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, app->screenSpaceIbo);
//    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(quadIndices), quadIndices, GL_STATIC_DRAW);
//    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
//
//    glGenVertexArrays(1, &app->screenSpaceVao);
//    glBindVertexArray(app->screenSpaceVao);
//    glBindBuffer(GL_ARRAY_BUFFER, app->screenSpaceVbo);
//    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(ScreenSpaceVertex), (void*)0);
//    glEnableVertexAttribArray(0);
//    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(ScreenSpaceVertex), (void*)(sizeof(float) * 3));
//    glEnableVertexAttribArray(1);
//    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, app->screenSpaceIbo);
//    glBindVertexArray(0);
//
//    app->diceTexIdx = LoadTexture2D(app, "Assets/Textures/dice.png");
//    app->whiteTexIdx = LoadTexture2D(app, "Assets/textures/color_white.png");
//    app->blackTexIdx = LoadTexture2D(app, "Assets/textures/color_black.png");
//    app->normalTexIdx = LoadTexture2D(app, "Assets/textures/color_normal.png");
//    app->magentaTexIdx = LoadTexture2D(app, "Assets/textures/color_magenta.png");
//
//    glEnable(GL_BLEND);
//    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
//
//    glEnable(GL_CULL_FACE);
//    glEnable(GL_DEPTH_TEST);
//
//    //app->mode = Mode_TexturedQuad;
//    app->mode = Mode_Model;
//}

//void Gui(Application* app)
//{
//    ImGui::BeginMainMenuBar();
//    {
//        //ImGui::BeginMenu("View");
//        /*if (ImGui::MenuItem("Extensions"))
//        {
//
//        }*/
//        //ImGui::EndMenu();
//    }
//    ImGui::EndMainMenuBar();
//
//    ImGui::Begin("Info");
//    ImGui::Text("FPS: %f", 1.0f/app->deltaTime);
//    ImGui::End();
//
//    ImGui::Begin("Camera");
//    {
//        auto& pos = app->camera.GetPosition();
//        if (ImGui::DragFloat3("Position", glm::value_ptr(pos), 0.01f))
//            app->camera.SetPosition(pos);
//
//        auto& rot = app->camera.GetRotation();
//        if (ImGui::DragFloat3("Rotation", glm::value_ptr(rot), 0.01f))
//            app->camera.SetRotation(rot);
//    }
//    ImGui::End();
//
//    ImGui::Begin("Model");
//    {
//        auto& pos = app->patrickModel->GetPosition();
//        if (ImGui::DragFloat3("Position", glm::value_ptr(app->patrickModel->GetPosition()), 0.01f))
//            app->patrickModel->SetPosition(pos);
//
//        auto& rot = app->patrickModel->GetRotation();
//        if (ImGui::DragFloat3("Rotation", glm::value_ptr(rot), 0.01f))
//            app->patrickModel->SetRotation(rot);
//
//        auto& scale = app->patrickModel->GetScale();
//        if (ImGui::DragFloat3("Scale", glm::value_ptr(scale), 0.01f))
//            app->patrickModel->SetScale(scale);
//
//        ImGui::Separator();
//        ImGui::Separator();
//
//        if (ImGui::TreeNodeEx("Materials"))
//        {
//            for (auto mat : app->patrickModel->GetMaterials())
//            {
//                if (ImGui::Button(mat->GetName().c_str()))
//                {
//
//                }
//            }
//            ImGui::TreePop();
//        }
//
//    }
//    ImGui::End();
//
//}

//void Update(Application* app)
//{
//    // You can handle app->input keyboard/mouse here
//    //if (app->input.keys[Key::K_W] == BUTTON_PRESSED)
//    //{
//    //    
//    //}
//}

//void Render(Application* app)
//{
//    glClearColor(0.08, 0.08, 0.08, 1);
//    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//    glViewport(0, 0, app->displaySize.x, app->displaySize.y);
//    app->camera.SetViewportSize(app->displaySize.x, app->displaySize.y);
//
//    switch (app->mode)
//    {
//        case Mode_TexturedQuad:
//        {            
//            Program& shader = app->programs[app->texturedGeometryShaderIdx];
//
//            glUseProgram(shader.handle);
//            glBindVertexArray(app->screenSpaceVao);
//
//            glUniform1i(glGetUniformLocation(shader.handle, "uTexture"), 0);
//            glActiveTexture(GL_TEXTURE0);
//            glBindTexture(GL_TEXTURE_2D, app->textures[app->diceTexIdx].handle);
//
//            glDrawElements(GL_TRIANGLES, sizeof(quadIndices) / sizeof(u16), GL_UNSIGNED_SHORT, 0);
//
//            glBindVertexArray(0);
//            glUseProgram(0);
//
//            break;
//        }
//        case Mode_Model:
//        {
//            //app->patrickShader->Bind();
//            //app->patrickShader->SetUniformMatrix4f("view", app->camera.GetView());
//            //app->patrickShader->SetUniformMatrix4f("projection", app->camera.GetProjection());
//            //app->patrickShader->SetUniformMatrix4f("model", app->patrickModel->GetTransform());
//
//            //app->patrickTexture->Bind(0);
//            //app->patrickShader->SetUniform1i("uTexture", 0);
//            app->patrickModel->Draw(app->camera.GetProjection() * app->camera.GetView());
//            break;
//        }
//
//        default:
//            break;
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
    camera = Camera({ 0.0f,0,200.0f }, { 0,0,0 }, 45.0f, 1280 / 720);
    dirLight = Light(LightType::DIRECTIONAL, { 1,1,1 });

    fbo = std::make_shared<Framebuffer>(displaySize.x, displaySize.y);
    currentRenderTargetId = fbo->GetColorAttachment();

    texturedGeometryShader = std::make_shared<Shader>("Assets/Shaders/shaders.glsl", "TEXTURED_GEOMETRY");

    //SetShaderUniforms(this, texturedGeometryShaderIdx);

    //patrickModel = ModelImporter::ImportModel("Assets/Models/Cerberus 2/Cerberus_LP.FBX");
    patrickModel = ModelImporter::ImportModel("Assets/Models/Cerberus/Cerberus.fbx");
    //patrickModel = ModelImporter::ImportModel("Assets/Models/Patrick/Patrick.obj");

    Entity m1 = Entity("Right Cerberus", patrickModel);
    m1.SetPosition({ 40,0,0 });
    entities.emplace_back(m1);

    Entity m2 = Entity("Center Cerberus", patrickModel);
    m2.SetPosition({ 0,0,0 });
    entities.emplace_back(m2);

    Entity m3 = Entity("Left Cerberus", patrickModel);
    m3.SetPosition({ -40,0,0 });
    entities.emplace_back(m3);
    

    glGetIntegerv(GL_MAX_UNIFORM_BLOCK_SIZE, &maxUniformBufferSize);
    glGetIntegerv(GL_UNIFORM_BUFFER_OFFSET_ALIGNMENT, &uniformBlockAlignment);
    localParamsUbo = std::make_shared<UniformBuffer>(maxUniformBufferSize, uniformBlockAlignment);
    globalParamsUbo = std::make_shared<UniformBuffer>(maxUniformBufferSize, uniformBlockAlignment);


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

    diceTex = std::make_shared<Texture2D>("Assets/Textures/dice.png");
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);

    //app->mode = Mode_TexturedQuad;
    mode = Mode_Model;
}

void Application::Update()
{
    //if (app->input.keys[Key::K_W] == BUTTON_PRESSED)
    //{
    //    
    //}

    globalParamsUbo->Map();

    globalParamsUbo->AlignHead(uniformBlockAlignment);
    globalParamsOffset = globalParamsUbo->GetHead();
    
    globalParamsUbo->Push1f(camera.GetNearClip());
    globalParamsUbo->Push1f(camera.GetFarClip());
    globalParamsUbo->PushVector3f(camera.GetPosition());
    globalParamsUbo->Push1i(1); // Light count
    
    globalParamsUbo->Push1i((int)dirLight.GetType());
    globalParamsUbo->PushVector3f(dirLight.GetDiffuse());
    globalParamsUbo->Push1f(dirLight.GetIntensity());
    globalParamsUbo->PushVector3f(dirLight.GetPosition());

    globalParamsSize = globalParamsUbo->GetHead() - globalParamsOffset;

    globalParamsUbo->Unmap();



    localParamsUbo->Map();

    for (auto& entity : entities)
    {
        localParamsUbo->AlignHead(uniformBlockAlignment);

        entity.localParamsOffset = localParamsUbo->GetHead();
        localParamsUbo->PushMatrix4f(entity.GetTransform());
        localParamsUbo->PushMatrix4f(camera.GetViewProj() * entity.GetTransform());

        entity.localParamsSize = localParamsUbo->GetHead() - entity.localParamsOffset;        

    }

    localParamsUbo->Unmap();

}

void Application::Render()
{
    fbo->Bind();

    glClearColor(0.08, 0.08, 0.08, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT  | GL_STENCIL_BUFFER_BIT);

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
            globalParamsUbo->BindRange(0, globalParamsOffset, globalParamsSize);

            for (auto& entity : entities)
            {
                localParamsUbo->BindRange(1, entity.localParamsOffset, entity.localParamsSize);
                entity.GetModel()->Draw();
            }

            break;
        }

    default:
        break;
    }

    fbo->Unbind();

}

void Application::OnImGuiRender()
{
    ImGui::BeginMainMenuBar();
    {
        if(ImGui::BeginMenu("View"))
        {
            if (ImGui::MenuItem("Extensions"))
            {

            }
            if (ImGui::MenuItem("Render Options"))
            {
                showRenderOptionsPanel = true;
            }
            ImGui::EndMenu();
        }
    }
    ImGui::EndMainMenuBar();


    if (showRenderOptionsPanel)
    {
        ImGui::Begin("Render Options", &showRenderOptionsPanel);
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
                    {
                        switch (i)
                        {
                            case 0: currentRenderTargetId = fbo->GetColorAttachment(); break;
                            case 1: currentRenderTargetId = fbo->GetNormalsAttachment(); break;
                            case 2: currentRenderTargetId = fbo->GetPositionAttachment(); break;
                            case 3: currentRenderTargetId = fbo->GetDepthAttachment(); break;
                        }
                        currentItemIndex = i;
                    }

                    if (isSelected) ImGui::SetItemDefaultFocus();
                }
                ImGui::EndCombo();
            }
        }
        ImGui::End();
    }


    ImGui::Begin("Viewport");
    {
        ImVec2 dimensions = ImGui::GetContentRegionAvail();
        if (viewportSize.x != dimensions.x || viewportSize.y != dimensions.y)
        {
            currentRenderTargetId = fbo->Resize(dimensions.x, dimensions.y);
            glViewport(0, 0, dimensions.x, dimensions.y);
            camera.SetViewportSize((uint32_t)dimensions.x, (uint32_t)dimensions.y);
            viewportSize = { dimensions.x, dimensions.y };
        }
        ImGui::Image((void*)currentRenderTargetId, { viewportSize.x, viewportSize.y }, { 0,1 }, { 1,0 });
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

    dirLight.OnImGuiRender();

}
