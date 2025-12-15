#include "pch.h"

#include "core/shader.h"

Shader::Shader(const std::filesystem::path& _vertexShader, const std::filesystem::path& _fragmentShader)
{
    int vertShader = LoadShaderFromFile(_vertexShader, GL_VERTEX_SHADER);
    int fragShader = LoadShaderFromFile(_fragmentShader, GL_FRAGMENT_SHADER);

    mProgramHandle = LinkShadersToProgram(vertShader, fragShader);
}

Shader::Shader(const char* _vertexSrc, const char* _fragmentSrc)
{
    int vertShader = CompileShader(_vertexSrc, GL_VERTEX_SHADER);
    int fragShader = CompileShader(_fragmentSrc, GL_FRAGMENT_SHADER);

    mProgramHandle = LinkShadersToProgram(vertShader, fragShader);
}

void Shader::Use() const
{
    glUseProgram(mProgramHandle);
}

void Shader::SetBool(const std::string& _name, bool _value) const
{
    glUniform1i(glGetUniformLocation(mProgramHandle, _name.c_str()), (int)_value);
}

void Shader::SetInt(const std::string& _name, int _value) const
{
    glUniform1i(glGetUniformLocation(mProgramHandle, _name.c_str()), _value);
}

void Shader::SetFloat(const std::string& _name, float _value) const
{
    glUniform1f(glGetUniformLocation(mProgramHandle, _name.c_str()), _value);
}

void Shader::SetVec2(const std::string& _name, const glm::vec2& _value) const
{
    glUniform2fv(glGetUniformLocation(mProgramHandle, _name.c_str()), 1, glm::value_ptr(_value));
}

void Shader::SetVec3(const std::string& _name, const glm::vec3& _value) const
{
    glUniform3fv(glGetUniformLocation(mProgramHandle, _name.c_str()), 1, glm::value_ptr(_value));
}

void Shader::SetVec4(const std::string& _name, const glm::vec4& _value) const
{
    glUniform4fv(glGetUniformLocation(mProgramHandle, _name.c_str()), 1, glm::value_ptr(_value));
}

void Shader::SetMat2(const std::string& _name, const glm::mat2& _value) const
{
    glUniformMatrix2fv(glGetUniformLocation(mProgramHandle, _name.c_str()), 1, GL_FALSE, glm::value_ptr(_value));
}

void Shader::SetMat3(const std::string& _name, const glm::mat3& _value) const
{
    glUniformMatrix3fv(glGetUniformLocation(mProgramHandle, _name.c_str()), 1, GL_FALSE, glm::value_ptr(_value));
}

void Shader::SetMat4(const std::string& _name, const glm::mat4& _value) const
{
    glUniformMatrix4fv(glGetUniformLocation(mProgramHandle, _name.c_str()), 1, GL_FALSE, glm::value_ptr(_value));
}

int Shader::LoadShaderFromFile(const std::filesystem::path& _path, unsigned int _shaderType)
{
	if (!std::filesystem::exists(_path)) {
        std::cout << std::format("File does not exist at: {}\n", _path.string());
		return -1;
	}

	std::ifstream file(_path);
	if (!file.is_open()) {
        std::cout << std::format("File could not be opened at: {}\n", _path.string());
		return -1;
	}

    const auto fileSize = std::filesystem::file_size(_path);
    std::string fileContents(fileSize, '\0');
    file.read(fileContents.data(), fileSize);

	return CompileShader(fileContents.c_str(), _shaderType);
}

int Shader::CompileShader(const char* _shaderSrc, unsigned int _shaderType)
{
    // create shader from source
    int shaderHandle = glCreateShader(_shaderType);
    glShaderSource(shaderHandle, 1, &_shaderSrc, NULL);

    // check shader compilation status
    int success;
    char infoLog[512];
    glCompileShader(shaderHandle);
    glGetShaderiv(shaderHandle, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(shaderHandle, 512, NULL, infoLog);
        std::cout << std::format("Shader compilation failed:\n{}\n", infoLog);
        return -1;
    }

    return shaderHandle;
}

int Shader::LinkShadersToProgram(int _vertShader, int _fragShader)
{
    int shaderProgram = glCreateProgram();

    // attach shaders to program
    glAttachShader(shaderProgram, _vertShader);
    glAttachShader(shaderProgram, _fragShader);
    glLinkProgram(shaderProgram);

    // check for linking errors
    int success;
    char infoLog[512];
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success)
    {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        std::cout << std::format("Shader program linking failed:\n{}\n", infoLog);
        return -1;
    }

    // cleanup stand-alone shader handles
    glDeleteShader(_vertShader);
    glDeleteShader(_fragShader);

    return shaderProgram;
}
