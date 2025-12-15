#pragma once
#include <span>

// modified from https://learnopengl.com/code_viewer_gh.php?code=includes/learnopengl/mesh.h

struct Vertex
{
	glm::vec3 position;
	glm::vec2 uv;
	glm::vec3 normal;

	bool operator == (const Vertex& _other) const {
		return position == _other.position 
			&& uv == _other.uv 
			&& normal == _other.normal;
	}
};

// needed to use Vertex in an unordered_map
namespace std {
	template<> struct hash<Vertex> {
		size_t operator()(const Vertex& _v) const {
			size_t h1 = hash<float>()(_v.position.x) ^ (hash<float>()(_v.position.y) << 1) ^ (hash<float>()(_v.position.z) << 2);
			size_t h2 = hash<float>()(_v.uv.x) ^ (hash<float>()(_v.uv.y) << 1);
			size_t h3 = hash<float>()(_v.normal.x) ^ (hash<float>()(_v.normal.y) << 1) ^ (hash<float>()(_v.normal.z) << 2);
			return h1 ^ (h2 << 1) ^ (h3 << 2);
		}
	};
}

class Mesh
{
private:
	unsigned int mVao = 0, mVbo = 0, mEbo = 0;

	std::vector<Vertex> mVertices = {};
	std::vector<unsigned int> mIndices = {};

public:
	Mesh(std::span<const Vertex> _vertices, std::span<const unsigned int> _indices);
	Mesh(const std::filesystem::path& _path);
	~Mesh();

	void Render() const;

private:
	bool TryLoadFromFile(const std::filesystem::path& _path);
	void SetupMesh();
};