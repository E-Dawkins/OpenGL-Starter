#include "pch.h"

#include "core/asset_loader.h"

#include "core/mesh.h"
#include "core/shader.h"
#include "core/texture.h"

#include "predef/primitives.h"
#include "predef/shader_presets.h"

template<typename T, typename... Ts>
constexpr bool EqualsAny(const T& _value, const Ts&... _values)
{
	return ((_value == _values) || ...);
}

void AssetLoader::Init(const std::filesystem::path& _assetsDirectory)
{
	auto instance = AssetLoader::GetInstance();

	instance->SetupPreDefinedAssets();

	for (const auto& dirEntry : std::filesystem::recursive_directory_iterator(_assetsDirectory))
	{
		if (dirEntry.is_directory()) continue;

		std::string extension = dirEntry.path().extension().string();

		if (EqualsAny(extension, ".vert", ".frag"))
		{
			instance->TryLoadShader(dirEntry.path());
		}
		else if (EqualsAny(extension, ".jpg", ".jpeg", ".png", ".tga", ".bmp", ".psd", ".gif", ".hdr", ".pic", ".ppm", ".pgm"))
		{
			instance->TryLoadTexture(dirEntry.path());
		}
		else if (EqualsAny(extension, ".obj")) 
		{
			instance->TryLoadMesh(dirEntry.path());
		}
		else // unknown type
		{
			std::cout << "UNKNOWN ASSET TYPE\n";
			std::cout << std::format("   >> {} ({})!\n", dirEntry.path().string(), extension);
		}
	}

	// some shaders have not been fully loaded
	if (!instance->mInProgressShaders.empty())
	{
		std::cout << "SHADER LOADING INCOMPLETE\n";
		for (const auto& s : instance->mInProgressShaders)
		{
			std::cout << std::format("   >> {} (no matching shader)\n", s.string());
		}
	}
}

void AssetLoader::TryLoadShader(const std::filesystem::path& _filePath)
{
	std::filesystem::path matchingShader = _filePath;
	matchingShader.replace_extension(_filePath.extension() == ".vert" ? ".frag" : ".vert");

	if (mInProgressShaders.contains(matchingShader)) // already loaded matching shader
	{
		mShaders.insert({
			_filePath.stem().string(),
			std::make_shared<Shader>(_filePath, matchingShader)
		});

		mInProgressShaders.erase(matchingShader);
	}
	else // store for later shader loading
	{
		mInProgressShaders.insert(_filePath);
	}
}

void AssetLoader::TryLoadTexture(const std::filesystem::path& _filePath)
{
	mTextures.insert({
		_filePath.stem().string(),
		std::make_shared<Texture>(_filePath)
	});
}

void AssetLoader::TryLoadMesh(const std::filesystem::path& _filePath)
{
	mMeshes.insert({
		_filePath.stem().string(),
		std::make_shared<Mesh>(_filePath)
	});
}

void AssetLoader::SetupPreDefinedAssets()
{
	{
		using namespace ShaderPresets;

		mShaders.insert({ "debug-base-color", std::make_shared<Shader>(DebugBaseColor::vertexSource, DebugBaseColor::fragmentSource) });
		mShaders.insert({ "debug-invalid", std::make_shared<Shader>(DebugInvalid::vertexSource, DebugInvalid::fragmentSource) });
		mShaders.insert({ "debug-uv", std::make_shared<Shader>(DebugUv::vertexSource, DebugUv::fragmentSource) });

		mShaders.insert({ "debug-normals-local", std::make_shared<Shader>(DebugNormals::vertexSource, DebugNormals::fragmentSource) });
		mShaders.insert({ "debug-normals-world", std::make_shared<Shader>(DebugNormals::vertexSource, DebugNormals::fragmentSource) });
		mShaders["debug-normals-world"]->Use();
		mShaders["debug-normals-world"]->SetBool("worldSpace", true);
	}

	{
		using namespace Primitives;

		mMeshes.insert({ "primitive-plane", std::make_shared<Mesh>(Plane::vertices, Plane::indices) });
		mMeshes.insert({ "primitive-multi-plane", std::make_shared<Mesh>(MultiPlane::vertices, MultiPlane::indices) });
		mMeshes.insert({ "primitive-cube", std::make_shared<Mesh>(Cube::vertices, Cube::indices) });
		mMeshes.insert({ "primitive-sphere", std::make_shared<Mesh>(Sphere::vertices, Sphere::indices) });
		mMeshes.insert({ "primitive-capsule", std::make_shared<Mesh>(Capsule::vertices, Capsule::indices) });
	}
}
