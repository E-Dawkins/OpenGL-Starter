#include "pch.h"

#include "core/renderer.h"
#include "core/asset_loader.h"
#include "core/material.h"

#include "utils/math_helpers.h"

#include "core/shader.h"

static void Update(Renderer* _r, float _dt)
{
    if (_r->window->GetKeyPressed(GLFW_KEY_ESCAPE))
        _r->window->SetShouldClose();

    // camera controls
    constexpr float sensitivity = 100.f;
    constexpr float moveMultiplier = 2.5f;

    const glm::vec2 mouseDelta = _r->window->GetMouseDelta();
    _r->camera->AddYaw(mouseDelta.x * _dt * sensitivity);
    _r->camera->AddPitch(-mouseDelta.y * _dt * sensitivity);

    if (int forwardInput = _r->window->GetKeyPressed(GLFW_KEY_W) - _r->window->GetKeyPressed(GLFW_KEY_S))
    {
        glm::vec3 flatForward = MathHelpers::FlattenVector(_r->camera->GetCameraForward(), MathHelpers::up);
        _r->camera->position += glm::normalize(flatForward) * (float)forwardInput * _dt * moveMultiplier;
    }

    if (int rightInput = _r->window->GetKeyPressed(GLFW_KEY_D) - _r->window->GetKeyPressed(GLFW_KEY_A))
    {
        glm::vec3 flatRight = MathHelpers::FlattenVector(_r->camera->GetCameraRight(), MathHelpers::up);
        _r->camera->position += glm::normalize(flatRight) * (float)rightInput * _dt * moveMultiplier;
    }

    int upInput = _r->window->GetKeyPressed(GLFW_KEY_SPACE) - _r->window->GetKeyPressed(GLFW_KEY_LEFT_CONTROL);
    _r->camera->position += glm::vec3(0, upInput, 0) * _dt * moveMultiplier;

    if (_r->window->GetKeyPressed(GLFW_KEY_KP_0)) _r->peelLastRender = 0;
    if (_r->window->GetKeyPressed(GLFW_KEY_KP_1)) _r->peelLastRender = 1;
    if (_r->window->GetKeyPressed(GLFW_KEY_KP_2)) _r->peelLastRender = 2;
    if (_r->window->GetKeyPressed(GLFW_KEY_KP_3)) _r->peelLastRender = 3;

    if (_r->window->GetKeyPressed(GLFW_KEY_0))
    {
        _r->peelFirstRender = 0;
        _r->peelLastRender = 0;
    }
    if (_r->window->GetKeyPressed(GLFW_KEY_1))
    {
        _r->peelFirstRender = 1;
        _r->peelLastRender = 1;
    }
    if (_r->window->GetKeyPressed(GLFW_KEY_2))
    {
        _r->peelFirstRender = 2;
        _r->peelLastRender = 2;
    }
    if (_r->window->GetKeyPressed(GLFW_KEY_3))
    {
        _r->peelFirstRender = 3;
        _r->peelLastRender = 3;
    }
}

#include "core/mesh.h"
int main()
{
    std::unique_ptr<Renderer> renderer = std::make_unique<Renderer>();
    renderer->BindToUpdateCallback(&Update);

    renderer->camera->position = glm::vec3(6, 0, 0);
    renderer->camera->FaceLocation(glm::vec3(0));

    {
        AssetLoader::Init("assets");

        std::shared_ptr<Material> matBaseColor = std::make_shared<Material>(AssetLoader::GetAsset<Shader>("debug-base-color"));
        matBaseColor->surfaceType = SurfaceType::TRANSLUCENT;
        matBaseColor->GetShader()->SetVec4("color", glm::vec4(1, 0, 0, 0.5f));

        std::shared_ptr<Material> matUv = std::make_shared<Material>(AssetLoader::GetAsset<Shader>("debug-uv"));
        std::shared_ptr<Material> matWorldNormals = std::make_shared<Material>(AssetLoader::GetAsset<Shader>("debug-normals-world"));

        std::shared_ptr<Material> matDefault = std::make_shared<Material>(
            AssetLoader::GetAsset<Shader>("test"),
            TextureMap {
                { "texture0", AssetLoader::GetAsset<Texture>("wall") },
                { "texture1", AssetLoader::GetAsset<Texture>("face") },
            }
        );

        renderer->RegisterRenderObject({
            .mesh = AssetLoader::GetAsset<Mesh>("primitive-plane"),
            .material = matDefault
        });

        renderer->RegisterRenderObject({
            .mesh = AssetLoader::GetAsset<Mesh>("primitive-multi-plane"),
            .material = matBaseColor,
            .transform = glm::translate(glm::mat4(1), glm::vec3(2, 0, 0))
        });

        renderer->RegisterRenderObject({
            .mesh = AssetLoader::GetAsset<Mesh>("primitive-cube"),
            .material = matBaseColor,
            .transform = glm::translate(glm::mat4(1), glm::vec3(4, 0, 0))
        });

        renderer->RegisterRenderObject({
            .mesh = AssetLoader::GetAsset<Mesh>("primitive-sphere"),
            .material = matDefault,
            .transform = glm::translate(glm::mat4(1), glm::vec3(-2, 0, 0))
        });

        renderer->RegisterRenderObject({
            .mesh = AssetLoader::GetAsset<Mesh>("primitive-capsule"),
            .material = matBaseColor,
            .transform = glm::translate(glm::mat4(1), glm::vec3(-4, 0, 0))
        });

        renderer->RegisterRenderObject({
            .mesh = AssetLoader::GetAsset<Mesh>("indent_plane"),
            .material = matDefault,
            .transform = glm::translate(glm::mat4(1), glm::vec3(-1, 2, 0))
        });

        renderer->RegisterRenderObject({
            .mesh = AssetLoader::GetAsset<Mesh>("suzanne"),
            .material = matWorldNormals,
            .transform = glm::translate(glm::mat4(1), glm::vec3(1, 2, 0))
        });
    }

    renderer->RenderLoop();
}
