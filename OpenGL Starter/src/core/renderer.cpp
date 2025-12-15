#include "pch.h"

#include "core/renderer.h"

#include "core/mesh.h"
#include "core/material.h"

#include "core/shader.h"
#include "core/asset_loader.h"

std::shared_ptr<Shader> peelShader, screenQuadShader;

Renderer::Renderer()
{
    glfwInit();

    window = std::make_unique<Window>(1200, 900, "OpenGL Starter");
    window->SetCursorInputMode(false, true);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD\n";
        return;
    }

    camera = std::make_unique<Camera>(glm::vec3(0), window->GetWidth() / (float)window->GetHeight());

    // setup OpenGL
    glEnable(GL_CULL_FACE);

    // setup frame buffer objects
    mOpaqueFbo = GenerateFbo();

    for (int i = 0; i < mDepthPeelFbos.size(); i++)
    {
        mDepthPeelFbos[i] = GenerateFbo();
    }
}

Renderer::~Renderer()
{
    glfwTerminate();
}

void Renderer::RenderLoop()
{
    // unfortunately, these have to be loaded after the AssetLoader class has initialized
    // which in-turn has to be initialized after the Renderer gets constructed :(
    peelShader = AssetLoader::GetAsset<Shader>("peel");
    screenQuadShader = AssetLoader::GetAsset<Shader>("screen_quad");

    float lastFrame = 0.f;

    while (!window->ShouldClose())
    {
        float currentFrame = static_cast<float>(glfwGetTime());
        float deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        mUpdateCallback(this, deltaTime);

        // Render >>
        {
            glm::mat4 viewProjection = camera->GetViewProjectionMatrix();

            Pass_Opaque(viewProjection);
            Pass_DepthPeelLayers(viewProjection);
            Pass_Composite();
        }
        // << Render

        window->SwapBuffers();
        glfwPollEvents();
    }
}

void Renderer::BindToUpdateCallback(UpdateCallback _callback)
{
    mUpdateCallback = _callback;
}

void Renderer::RegisterRenderObject(const RenderObject& _object)
{
    if (_object.material->surfaceType == SurfaceType::TRANSLUCENT)
    {
        mTranslucentObjects.emplace_back(_object);
    }
    else
    {
        mOpaqueObjects.emplace_back(_object);
    }
}

FrameBufferObject Renderer::GenerateFbo() const
{
    FrameBufferObject newFbo = {};

    glGenFramebuffers(1, &newFbo.frameBuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, newFbo.frameBuffer);

    glGenTextures(1, &newFbo.colorTexture);
    glBindTexture(GL_TEXTURE_2D, newFbo.colorTexture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, window->GetWidth(), window->GetHeight(), 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, newFbo.colorTexture, 0);

    glGenTextures(1, &newFbo.depthTexture);
    glBindTexture(GL_TEXTURE_2D, newFbo.depthTexture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24, window->GetWidth(), window->GetHeight(), 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, newFbo.depthTexture, 0);

    GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if (status != GL_FRAMEBUFFER_COMPLETE) {
        std::cerr << "Framebuffer incomplete: " << status << std::endl;
    }

    return newFbo;
}

void Renderer::Pass_Opaque(const glm::mat4& _viewProjection)
{
    glBindFramebuffer(GL_FRAMEBUFFER, mOpaqueFbo.frameBuffer);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClearDepth(1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glDisable(GL_BLEND);

    for (const auto& obj : mOpaqueObjects)
    {
        if (!obj.material || !obj.mesh) 
        {
            continue;
        }

        obj.material->Use(obj.transform, _viewProjection);
        obj.mesh->Render();
    }
}

void Renderer::Pass_DepthPeelLayers(const glm::mat4& _viewProjection)
{
    peelShader->Use();
    peelShader->SetMat4("viewProjection", _viewProjection);
    peelShader->SetVec2("viewportSize", glm::vec2(window->GetWidth(), window->GetHeight()));
    
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, mOpaqueFbo.depthTexture);
    peelShader->SetInt("opaqueDepthTex", 0);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, 0); // clear tex unit 1
    peelShader->SetInt("previousDepthTex", 1);

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glDisable(GL_BLEND);

    for (int i = 0; i < mDepthPeelFbos.size(); i++)
    {
        glBindFramebuffer(GL_FRAMEBUFFER, mDepthPeelFbos[i].frameBuffer);
        glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
        glClearDepth(1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        if (i > 0) // depth texture only exists after the first layer has been rendered
        {
            glBindTexture(GL_TEXTURE_2D, mDepthPeelFbos[i - 1].depthTexture);
        }

        for (const auto& obj : mTranslucentObjects)
        {
            if (!obj.material || !obj.mesh)
            {
                continue;
            }

            peelShader->SetMat4("model", obj.transform);
            obj.mesh->Render();
        }
    }
}

void Renderer::Pass_Composite()
{
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glDisable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    screenQuadShader->Use();

    // setup screen texture binding
    glActiveTexture(GL_TEXTURE0);
    screenQuadShader->SetInt("screenTex", 0);

    // composite opaque pass
    glBindTexture(GL_TEXTURE_2D, mOpaqueFbo.colorTexture);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

    // composite depth peel layers, back-to-front
    for (int i = peelFirstRender; i >= peelLastRender; --i)
    {
        glBindTexture(GL_TEXTURE_2D, mDepthPeelFbos[i].colorTexture);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    }
}
