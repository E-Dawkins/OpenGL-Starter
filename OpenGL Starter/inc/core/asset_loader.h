#pragma once
#include <map>
#include <set>

class Mesh;
class Shader;
class Texture;

class AssetLoader
{
private:
	std::map<std::string, std::shared_ptr<Mesh>> mMeshes = {};
	std::map<std::string, std::shared_ptr<Shader>> mShaders = {};
	std::map<std::string, std::shared_ptr<Texture>> mTextures = {};

	std::set<std::filesystem::path> mInProgressShaders = {};

public:
	static void Init(const std::filesystem::path& _assetsDirectory);

	template<typename T>
	static std::shared_ptr<T> GetAsset(const std::string& _name);

private:
	static inline std::shared_ptr<AssetLoader> GetInstance()
	{
		static std::shared_ptr<AssetLoader> instance = std::make_shared<AssetLoader>();
		return instance;
	}

	void TryLoadShader(const std::filesystem::path& _filePath);
	void TryLoadTexture(const std::filesystem::path& _filePath);
	void TryLoadMesh(const std::filesystem::path& _filePath);

	void SetupPreDefinedAssets();
};

template<typename T>
inline std::shared_ptr<T> AssetLoader::GetAsset(const std::string& _name)
{
	auto instance = GetInstance();

	if constexpr (std::is_same_v<T, Mesh>)
	{
		return instance->mMeshes[_name];
	}
	else if constexpr (std::is_same_v<T, Shader>)
	{
		if (!instance->mShaders.contains(_name))
			return instance->mShaders["debug-invalid"];

		return instance->mShaders[_name];
	}
	else if constexpr (std::is_same_v<T, Texture>)
	{
		return instance->mTextures[_name];
	}
	else
	{
		static_assert(sizeof(T) == 0, "Invalid asset type!");
	}
}
