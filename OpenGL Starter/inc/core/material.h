#pragma once

#include <unordered_map>

class Shader;
class Texture;

typedef std::unordered_map<std::string, std::shared_ptr<Texture>> TextureMap;

enum class SurfaceType
{
	OPAQUE,
	MASKED,
	TRANSLUCENT,

	COUNT
};

class Material
{
public:
	SurfaceType surfaceType = SurfaceType::OPAQUE;

private:
	std::shared_ptr<Shader> mShader;
	TextureMap mTextures;

public:
	Material(std::shared_ptr<Shader> _shader, const TextureMap& _textures = {});

	void Use(const glm::mat4& _modelMatrix, const glm::mat4& _viewProjMatrix);

	std::shared_ptr<Shader> GetShader() const;
};