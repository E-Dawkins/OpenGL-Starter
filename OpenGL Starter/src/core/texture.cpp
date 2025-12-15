#include "pch.h"

#include "core/texture.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

Texture::Texture(const std::filesystem::path& _path)
{
	mTextureHandle = LoadTexture(_path);
}

void Texture::Use(unsigned int _unit) const
{
	if (_unit >= 16)
	{
		std::cout << "TEXTURE LIMIT REACHED\n";
		std::cout << "   >> Attempted to use texture with unit: " << _unit << "\n";
	}
	else
	{
		glActiveTexture(GL_TEXTURE0 + _unit);
		glBindTexture(GL_TEXTURE_2D, mTextureHandle);
	}
}

int Texture::LoadTexture(const std::filesystem::path& _path)
{
	// since opengl uses 0,0 as the bottom-left coordinate loaded images need to be flipped
	if (!stbi__vertically_flip_on_load_global)
	{
		stbi_set_flip_vertically_on_load(true);
	}

	int width, height, channels;
	unsigned char* data = stbi_load(_path.string().c_str(), &width, &height, &channels, 0);
	if (!data)
	{
		std::cout << std::format("Failed to load texture: {}\n", _path.string());
		return -1;
	}

	unsigned int texture;
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);

	// set wrapping / filtering options for our texture
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	int format = -1;
	switch (channels)
	{
		case 1: format = GL_RED; break;
		case 2: format = GL_RG; break;
		case 3: format = GL_RGB; break;
		case 4: format = GL_RGBA; break;
	}

	glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
	glGenerateMipmap(GL_TEXTURE_2D);

	stbi_image_free(data);

	return texture;
}
