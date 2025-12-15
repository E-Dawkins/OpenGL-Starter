#include "core/material.h"

#include "core/shader.h"
#include "core/texture.h"

Material::Material(std::shared_ptr<Shader> _shader, const TextureMap& _textures)
    : mShader(_shader), mTextures(_textures)
{
    mShader->Use(); // so we can set uniforms

    int unit = 0;
    for (const auto& [name, texture] : mTextures)
    {
        mShader->SetInt(name, unit++);
    }
}

void Material::Use(const glm::mat4& _modelMatrix, const glm::mat4& _viewProjMatrix)
{
    mShader->Use();
    mShader->SetMat4("model", _modelMatrix);
    mShader->SetMat4("viewProjection", _viewProjMatrix);

    int unit = 0;
    for (const auto& [name, texture] : mTextures)
    {
        texture->Use(unit++);
    }
}

std::shared_ptr<Shader> Material::GetShader() const
{
    return mShader;
}
