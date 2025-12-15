#include "pch.h"

#include "core/mesh.h"
#include "utils/math_helpers.h"

#include <tiny_obj_loader.h>
#include <unordered_map>

Mesh::Mesh(std::span<const Vertex> _vertices, std::span<const unsigned int> _indices)
    : mVertices(_vertices.begin(), _vertices.end()), mIndices(_indices.begin(), _indices.end())
{
    SetupMesh();
}

Mesh::Mesh(const std::filesystem::path& _path)
{
    if (TryLoadFromFile(_path)) 
    {
        SetupMesh();
    }
}

Mesh::~Mesh()
{
    glDeleteBuffers(1, &mEbo);
    glDeleteBuffers(1, &mVbo);
    glDeleteVertexArrays(1, &mVao);
}

void Mesh::Render() const
{
    glBindVertexArray(mVao);
    glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(mIndices.size()), GL_UNSIGNED_INT, 0);
}

bool Mesh::TryLoadFromFile(const std::filesystem::path& _path)
{
    tinyobj::attrib_t attrib;
    std::vector<tinyobj::shape_t> shapes;
    std::string warning, error;

    if (!tinyobj::LoadObj(&attrib, &shapes, nullptr, &warning, &error, _path.string().c_str()))
    {
        if (!error.empty())
        {
            std::cerr << std::format("ERROR: {}\n", error);
        }

        return false;
    }

    if (!warning.empty())
    {
        std::cerr << std::format("WARNING: {}\n", warning);
    }

    std::unordered_map<Vertex, uint32_t> uniqueVertices = {};

    for (const auto& shape : shapes)
    {
        for (const auto& index : shape.mesh.indices)
        {
            Vertex v = {};

            v.position = {
                attrib.vertices[3 * index.vertex_index + 0],
                attrib.vertices[3 * index.vertex_index + 1],
                attrib.vertices[3 * index.vertex_index + 2]
            };

            if (index.texcoord_index >= 0) {
                v.uv = {
                    attrib.texcoords[2 * index.texcoord_index + 0],
                    attrib.texcoords[2 * index.texcoord_index + 1],
                };
            }

            if (index.normal_index >= 0) {
                v.normal = {
                    attrib.normals[3 * index.normal_index + 0],
                    attrib.normals[3 * index.normal_index + 1],
                    attrib.normals[3 * index.normal_index + 2]
                };
            }

            // Check for duplicate vertices
            if (uniqueVertices.count(v) == 0) {
                uniqueVertices[v] = static_cast<uint32_t>(mVertices.size());
                mVertices.push_back(v);
            }

            mIndices.push_back(uniqueVertices[v]);
        }
    }

    return true;
}

void Mesh::SetupMesh()
{
    // create buffers/arrays
    glGenVertexArrays(1, &mVao);
    glGenBuffers(1, &mVbo);
    glGenBuffers(1, &mEbo);

    glBindVertexArray(mVao);

    glBindBuffer(GL_ARRAY_BUFFER, mVbo);
    glBufferData(GL_ARRAY_BUFFER, mVertices.size() * sizeof(Vertex), &mVertices[0], GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mEbo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, mIndices.size() * sizeof(unsigned int), &mIndices[0], GL_STATIC_DRAW);

    // set the vertex attribute pointers
    // Vertex::position
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, position));
    // Vertex::uv
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, uv));
    // Vertex::norm
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));

    // un-bind this meshes vao
    glBindVertexArray(0);
}
