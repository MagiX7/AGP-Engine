//
// engine.cpp : Put all your graphics stuff in this file. This is kind of the graphics module.
// In here, you should type all your OpenGL commands, and you can also type code to handle
// input platform events (e.g to move the camera or react to certain shortcuts), writing some
// graphics related GUI options, and so on.
//

#include "engine.h"
#include "ModelImporter.h"
#include "Renderer/Shader.h"

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
    camera = Camera({ 0,2,2 }, { 0,0,0 }, 45.0f, 1280 / 720);


    texturedGeometryShader = std::make_shared<Shader>("Assets/Shaders/shaders.glsl", "TEXTURED_GEOMETRY");
    //modelShaderIndex = LoadProgram(this, "Assets/Shaders/model.glsl", "MODEL");

    //SetShaderUniforms(this, texturedGeometryShaderIdx);

    //patrickModel = ModelImporter::ImportModel("Assets/Models/Cerberus 2/Cerberus_LP.FBX");
    patrickModel = ModelImporter::ImportModel("Assets/Models/Cerberus/Cerberus.fbx");


    glGetIntegerv(GL_MAX_UNIFORM_BLOCK_SIZE, &maxUniformBufferSize);
    glGetIntegerv(GL_UNIFORM_BUFFER_OFFSET_ALIGNMENT, &uniformBlockAlignment);
    sceneUbo = std::make_shared<UniformBuffer>(maxUniformBufferSize, uniformBlockAlignment);

    //sceneUbo->Map<glm::mat4>(glm::value_ptr(camera.GetViewProj()));

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
    //diceTexIdx = LoadTexture2D(this, "Assets/Textures/dice.png");
    //whiteTexIdx = LoadTexture2D(this, "Assets/textures/color_white.png");
    //blackTexIdx = LoadTexture2D(this, "Assets/textures/color_black.png");
    //normalTexIdx = LoadTexture2D(this, "Assets/textures/color_normal.png");
    //magentaTexIdx = LoadTexture2D(this, "Assets/textures/color_magenta.png");

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);

    //app->mode = Mode_TexturedQuad;
    mode = Mode_Model;
}

void Application::Update()
{
    //if (app->input.keys[Key::K_W] == BUTTON_PRESSED)
    //{
    //    
    //}
    sceneUbo->Map();
    sceneUbo->PushMatrix4f(patrickModel->GetTransform());
    sceneUbo->PushMatrix4f(camera.GetViewProj());
    sceneUbo->Unmap();


}

void Application::Render()
{
    glClearColor(0.08, 0.08, 0.08, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glViewport(0, 0, displaySize.x, displaySize.y);
    camera.SetViewportSize(displaySize.x, displaySize.y);

    switch (mode)
    {
        case Mode_TexturedQuad:
        {
            //Program& shader = programs[texturedGeometryShaderIdx];

            texturedGeometryShader->Bind();

            //glUseProgram(shader.handle);
            glBindVertexArray(screenSpaceVao);

            texturedGeometryShader->SetUniform1i("uTexture", 0);
            diceTex->Bind(0);
            //glUniform1i(glGetUniformLocation(shader.handle, "uTexture"), 0);
            //glActiveTexture(GL_TEXTURE0);
            //glBindTexture(GL_TEXTURE_2D, textures[diceTexIdx].handle);

            glDrawElements(GL_TRIANGLES, sizeof(quadIndices) / sizeof(u16), GL_UNSIGNED_SHORT, 0);

            glBindVertexArray(0);
            glUseProgram(0);

            break;
        }
        case Mode_Model:
        {
            //app->patrickShader->Bind();
            //app->patrickShader->SetUniformMatrix4f("view", app->camera.GetView());
            //app->patrickShader->SetUniformMatrix4f("projection", app->camera.GetProjection());
            //app->patrickShader->SetUniformMatrix4f("model", app->patrickModel->GetTransform());

            //app->patrickTexture->Bind(0);
            //app->patrickShader->SetUniform1i("uTexture", 0);

            //sceneUbo->Map();
            //sceneUbo->PushMatrix4f(patrickModel->GetTransform());
            //sceneUbo->PushMatrix4f(camera.GetViewProj());
            //sceneUbo->Unmap();

            sceneUbo->BindRange(1, 0, 2 * sizeof(glm::mat4));
            patrickModel->Draw(camera.GetProjection() * camera.GetView());
            break;
        }

    default:
        break;
    }

}

void Application::OnImGuiRender()
{
    ImGui::BeginMainMenuBar();
    {
        //ImGui::BeginMenu("View");
        /*if (ImGui::MenuItem("Extensions"))
        {

        }*/
        //ImGui::EndMenu();
    }
    ImGui::EndMainMenuBar();

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
    }
    ImGui::End();

    ImGui::Begin("Model");
    {
        auto& pos = patrickModel->GetPosition();
        if (ImGui::DragFloat3("Position", glm::value_ptr(patrickModel->GetPosition()), 0.01f))
            patrickModel->SetPosition(pos);

        auto& rot = patrickModel->GetRotation();
        if (ImGui::DragFloat3("Rotation", glm::value_ptr(rot), 0.01f))
            patrickModel->SetRotation(rot);

        auto& scale = patrickModel->GetScale();
        if (ImGui::DragFloat3("Scale", glm::value_ptr(scale), 0.01f))
            patrickModel->SetScale(scale);

        ImGui::Separator();
        ImGui::Separator();

        if (ImGui::TreeNodeEx("Materials"))
        {
            for (auto mat : patrickModel->GetMaterials())
            {
                if (ImGui::Button(mat->GetName().c_str()))
                {

                }
            }
            ImGui::TreePop();
        }

    }
    ImGui::End();
}
