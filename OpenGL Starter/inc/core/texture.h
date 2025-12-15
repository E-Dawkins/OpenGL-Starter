#pragma once

class Texture
{
private:
	int mTextureHandle;

public:
	Texture(const std::filesystem::path& _path);

	void Use(unsigned int _unit) const;

private:
	// @returns Texture handle, or -1 if we fail
	int LoadTexture(const std::filesystem::path& _path);
};