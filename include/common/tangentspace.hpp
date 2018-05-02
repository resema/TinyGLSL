#ifndef TANGENTSPACE_HPP
#define TANGENTSPACE_HPP

#include <vector>
#include <glm/glm.hpp>

void computeTangentBasis(
    // inputs
    std::vector<glm::vec3>& vertices,
    std::vector<glm::vec2>& uvs,
    std::vector<glm::vec3>& normals,
    // outputs
    std::vector<glm::vec3>& tangents,
    std::vector<glm::vec3>& bitangents
);

#endif  // TANGENTSPACE_HPP