#include <vector>
#include <cstring>

#include <GL/glew.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "common/shader.hpp"
#include "common/texture.hpp"

#include "common/text2D.hpp"

unsigned int Text2DTextureID;
unsigned int Text2DVertexBufferID;
unsigned int Text2DUVBufferID;
unsigned int Text2DShaderID;
unsigned int Text2DUniformID;

void initText2D(const char* texturePath)
{
    // initialize texture
    Text2DTextureID = loadDDS(texturePath);

    // initialize VBO
    glGenBuffers(1, &Text2DVertexBufferID);
    glGenBuffers(1, &Text2DUVBufferID);

    // initialize shader
    Text2DShaderID = LoadShaders("shaders/TextVertexShader.vs", "shaders/TextVertexShader.fs");

    // initialize uniforms' IDs
    Text2DUniformID = glGetUniformLocation(Text2DShaderID, "myTextureSampler");
}

void printText2D(const char* text, int x, int y, int size)
{
    unsigned int length = strlen(text);

    // fill buffers
    std::vector<glm::vec2> vertices;
    std::vector<glm::vec2> UVs;
    for (unsigned int i = 0; i < length; i++)
    {
        glm::vec2 vertex_up_left    = glm::vec2(x + i*size,         y + size);
        glm::vec2 vertex_up_right   = glm::vec2(x + i*size + size,  y + size);
        glm::vec2 vertex_down_right = glm::vec2(x + i*size + size,  y);
        glm::vec2 vertex_down_left  = glm::vec2(x + i*size,         y);

        vertices.push_back(vertex_up_left);
        vertices.push_back(vertex_down_left);
        vertices.push_back(vertex_up_right);

        vertices.push_back(vertex_down_right);
        vertices.push_back(vertex_up_right);
        vertices.push_back(vertex_down_left);

        char character = text[i];
        float uv_x = (character % 16) / 16.f;       // funny calculation to get row and col
        float uv_y = (character / 16) / 16.f;

        glm::vec2 uv_up_left    = glm::vec2(uv_x,               uv_y);
        glm::vec2 uv_up_right   = glm::vec2(uv_x + 1.f / 16.f,  uv_y);
        glm::vec2 uv_down_right = glm::vec2(uv_x + 1.f / 16.f,  (uv_y + 1.f / 16.f));
        glm::vec2 uv_down_left  = glm::vec2(uv_x,               (uv_y + 1.f / 16.f));

        UVs.push_back(uv_up_left);
        UVs.push_back(uv_down_left);
        UVs.push_back(uv_up_right);

        UVs.push_back(uv_down_right);
        UVs.push_back(uv_up_right);
        UVs.push_back(uv_down_left);
    }

    glBindBuffer(GL_ARRAY_BUFFER, Text2DVertexBufferID);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec2), &vertices[0], GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, Text2DUVBufferID);
    glBufferData(GL_ARRAY_BUFFER, UVs.size() * sizeof(glm::vec2), &UVs[0], GL_STATIC_DRAW);

    // bind buffer
    glUseProgram(Text2DShaderID);

    // bind texture
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, Text2DTextureID);
    // set our "myTextureSampler" sampler to use Texture Unit 0
    glUniform1i(Text2DUniformID, 0);

    // 1rst attribute buffer : vertices
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, Text2DVertexBufferID);
    glVertexAttribPointer(
        0,          // index
        2,          // size
        GL_FLOAT,   // type
        GL_FALSE,   // normalized?
        0,          // stride
        (void*)0    // ptr to the first vertex attribute in the array
    );

    // 2nd attribute buffer : UVs
    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, Text2DUVBufferID);
    glVertexAttribPointer(
        1,          // index
        2,          // size
        GL_FLOAT,   // type
        GL_FALSE,   // normalized?
        0,          // stride
        (void*)0    // ptr to the first vertex attribute in the array
    );

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // draw call
    glDrawArrays(
        GL_TRIANGLES,       // mode
        0,                  // first?
        vertices.size()     // size
    );

    glDisable(GL_BLEND);

    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
}

void cleanupText2D()
{
    // delete buffers
    glDeleteBuffers(1, &Text2DVertexBufferID);
    glDeleteBuffers(1, &Text2DUVBufferID);

    // delete texture
    glDeleteTextures(1, &Text2DTextureID);

    // delete shader
    glDeleteProgram(Text2DShaderID);
}