#pragma once

// modified from https://learnopengl.com/code_viewer_gh.php?code=includes/learnopengl/shader_m.h

class Shader
{
private:
	int mProgramHandle;

public:
	Shader(const std::filesystem::path& _vertexShader, const std::filesystem::path& _fragmentShader);
	Shader(const char* _vertexSrc, const char* _fragmentSrc);

	void Use() const;

	void SetBool(const std::string& _name, bool _value) const;
	void SetInt(const std::string& _name, int _value) const;
	void SetFloat(const std::string& _name, float _value) const;
	void SetVec2(const std::string& _name, const glm::vec2& _value) const;
	void SetVec3(const std::string& _name, const glm::vec3& _value) const;
	void SetVec4(const std::string& _name, const glm::vec4& _value) const;
	void SetMat2(const std::string& _name, const glm::mat2& _value) const;
	void SetMat3(const std::string& _name, const glm::mat3& _value) const;
	void SetMat4(const std::string& _name, const glm::mat4& _value) const;

private:
	// @returns Shader handle, or -1 if we fail
	int LoadShaderFromFile(const std::filesystem::path& _path, unsigned int _shaderType);

	// @returns Shader handle, or -1 if we fail
	int CompileShader(const char* _shaderSrc, unsigned int _shaderType);

	// @returns Program handle, or -1 if we fail
	int LinkShadersToProgram(int _vertShader, int _fragShader);
};