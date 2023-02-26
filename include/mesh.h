#ifndef MESH_H
#define MESH_H
#include <vector>
#include "shader.h"
#include "texture.h"
#include "vertex.h"

class Mesh {
    public:
        std::vector<Vertex>       vertices;
        std::vector<unsigned int> indices;
        std::vector<Texture>      textures;

        Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<Texture> textures);
        void Draw(Shader &shader);
    private:
        unsigned int VAO, VBO, EBO;
        void setupMesh();
};

#endif