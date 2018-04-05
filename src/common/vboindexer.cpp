#include <map>
#include <string>

#include "common/vboindexer.hpp"

struct PackedVertex
{
    glm::vec3 position;
    glm::vec2 uv;
    glm::vec3 normal;

    bool operator<(const PackedVertex that) const
    {
        return memcmp((void*)this, (void*)&that, sizeof(PackedVertex)) > 0;
    }
};

// returns true if v1 can be considered equal to v2
bool is_near(float v1, float v2)
{
    return fabs(v1 - v2) < 0.01f;
}

// searches through all already-exported vertices for a similar one
//  similar := same position + same UVs + same normal
bool getSimilarVertexIndex(
        glm::vec3 &in_vertex,
        glm::vec2 &in_uv,
        glm::vec3 &in_normals,

        std::vector<glm::vec3> &out_vertices,
        std::vector<glm::vec2> &out_uvs,
        std::vector<glm::vec3> &out_normals,
        unsigned short &result
        )
{
    // lame linear search
    for (unsigned int i = 0; i < out_vertices.size(); i++)
    {
        if (is_near(in_vertex.x, out_vertices[i].x) &&
            is_near(in_vertex.y, out_vertices[i].y) &&
            is_near(in_vertex.z, out_vertices[i].z) &&
            is_near(in_uv.x,     out_uvs[i].x)      &&
            is_near(in_uv.y,     out_uvs[i].y)      &&
            is_near(in_normals.x, out_normals[i].x) &&
            is_near(in_normals.y, out_normals[i].y) &&
            is_near(in_normals.z, out_normals[i].z)   )
        {
             result = i;
             return true;
        }
    }

    // no other vertex could be used insted
    // looks like we'll have to add it to the vbo
    return false;
}

void indexVBO_slow(
        std::vector<glm::vec3> &in_vertices,
        std::vector<glm::vec2> &in_uvs,
        std::vector<glm::vec3> &in_normals,

        std::vector<unsigned short> &out_indices,
        std::vector<glm::vec3> &out_vertices,
        std::vector<glm::vec2> &out_uvs,
        std::vector<glm::vec3> &out_normals
        )
{
    // for each input vertex
    for (unsigned int i = 0; i < in_vertices.size(); i++)
    {
        // try to find a similar vertex in out_XXXX
        unsigned short index;
        bool found = getSimilarVertexIndex(
            in_vertices[i], in_uvs[i], in_normals[i], 
            out_vertices, out_uvs, out_normals, index
            );

        if (found)  // a similar vertex is alreasdy in the VBO, use it instead
        {
            out_indices.push_back(index);
        }
        else        // if not, it needs to be added in the output data
        {
            out_vertices.push_back(in_vertices[i]);
            out_uvs.push_back(in_uvs[i]);
            out_normals.push_back(in_normals[i]);
            out_indices.push_back((unsigned short)out_vertices.size() - 1);
        }
    }
}

bool getSimilarVertexIndex_fast(
        PackedVertex &packed,
        std::map<PackedVertex, unsigned short> &VertexToOutIndex,
        unsigned short &result
        )
{
    auto it = VertexToOutIndex.find(packed);
    if (it == VertexToOutIndex.end())
    {
        return false;
    }
    else {
        result = it->second;
        return true;
    }
}

void indexVBO(
        std::vector<glm::vec3> &in_vertices,
        std::vector<glm::vec2> &in_uvs,
        std::vector<glm::vec3> &in_normals,

        std::vector<unsigned short> &out_indices,
        std::vector<glm::vec3> &out_vertices,
        std::vector<glm::vec2> &out_uvs,
        std::vector<glm::vec3> &out_normals
        )
{
    std::map<PackedVertex, unsigned short> VertexToOutIndex;

    // for eacht input vertex
    for (unsigned int i = 0; i < in_vertices.size(); i++)
    {
        PackedVertex packed = {in_vertices[i], in_uvs[i], in_normals[i]};

        // try to find a similar vertex in out_XXXX
        unsigned short index;
        bool found = getSimilarVertexIndex_fast(packed, VertexToOutIndex, index);

        if (found)  // a similar vertex is already in the VBO, use it instead!
        {
            out_indices.push_back(index);
        }
        else        // if not, it needs to be added in the output data
        {
            out_vertices.push_back(in_vertices[i]);
            out_uvs.push_back(in_uvs[i]);
            out_normals.push_back(in_normals[i]);
            unsigned short newindex = (unsigned short)out_vertices.size() - 1;
            out_indices.push_back(newindex);
            VertexToOutIndex[packed] = newindex;
        }
    }
}

void indexVBO_TBN(
    std::vector<glm::vec3> &in_vertices,
    std::vector<glm::vec2> &in_uvs,
    std::vector<glm::vec3> &in_normals,
    std::vector<glm::vec3> &in_tangents,
    std::vector<glm::vec3> &in_bitangents,

    std::vector<unsigned short> &out_indices,
    std::vector<glm::vec3> &out_vertices,
    std::vector<glm::vec2> &out_uvs,
    std::vector<glm::vec3> &out_normals,
    std::vector<glm::vec3> &out_tangents,
    std::vector<glm::vec3> &out_bitangents
)
{
    // for eacht input vertex
    for (unsigned int i = 0; i < in_vertices.size(); i++)
    {
        // try to find a similar vertex in out_XXXX
        unsigned short index;
        bool found = getSimilarVertexIndex(
            in_vertices[i], 
            in_uvs[i], 
            in_normals[i],
            out_vertices,
            out_uvs,
            out_normals,
            index
            );

        if (found)  // a similar vertex is already in the VBO, use it instead!
        {
            out_indices.push_back(index);

            // average the tangents and the bitangents
            out_tangents[index] += in_tangents[i];
            out_bitangents[index] += in_bitangents[i];
        }
        else        // if not, it needs to be added in the output data
        {
            out_vertices.push_back(in_vertices[i]);
            out_uvs.push_back(in_uvs[i]);
            out_normals.push_back(in_normals[i]);
            out_tangents.push_back(in_tangents[i]);
            out_bitangents.push_back(out_bitangents[i]);
            out_indices.push_back((unsigned short)out_vertices.size() - 1);
        }
    }
}