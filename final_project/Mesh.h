#ifndef MESH_H
#define MESH_H

#include <vector>

class Mesh {
public:
    // Constructor
    Mesh() = default;
    Mesh(const std::vector<float>& vertices, const std::vector<unsigned int>& indices,
         const std::vector<float>& normals = {}, const std::vector<float>& textureCoords = {})
        : vertices(vertices), indices(indices), normals(normals), textureCoords(textureCoords) {}

    // Getters
    const std::vector<float>& getVertices() const { return vertices; }
    const std::vector<unsigned int>& getIndices() const { return indices; }
    const std::vector<float>& getNormals() const { return normals; }
    const std::vector<float>& getTextureCoords() const { return textureCoords; }

    // Setters
    void setVertices(const std::vector<float>& newVertices) { vertices = newVertices; }
    void setIndices(const std::vector<unsigned int>& newIndices) { indices = newIndices; }
    void setNormals(const std::vector<float>& newNormals) { normals = newNormals; }
    void setTextureCoords(const std::vector<float>& newTextureCoords) { textureCoords = newTextureCoords; }

    // Adder
    void addVertex(float x, float y, float z) {
        vertices.push_back(x);
        vertices.push_back(y);
        vertices.push_back(z);
    }
    void addIndex(unsigned int index) {
        indices.push_back(index);
    }
    void addNormal(float nx, float ny, float nz) {
        normals.push_back(nx);
        normals.push_back(ny);
        normals.push_back(nz);
    }
    void addTextureCoord(float u, float v) {
        textureCoords.push_back(u);
        textureCoords.push_back(v);
    }

    // Utility function to clear all data
    void clear() {
        vertices.clear();
        indices.clear();
        normals.clear();
        textureCoords.clear();
    }

    void Mesh::drawModel(const Mesh &modelMesh, const GLuint textureID) {
        glEnable(GL_TEXTURE_2D);
        glEnable(GL_LIGHTING);
        glColor3f(1.0f, 1.0f, 1.0f); // Set color to white for texture mapping
        glBindTexture(GL_TEXTURE_2D, textureID); // <-- bind your texture

        glEnableClientState(GL_VERTEX_ARRAY);
        glVertexPointer(3, GL_FLOAT, 0, modelMesh.getVertices().data());

        if (!modelMesh.getNormals().empty()) {
            glEnableClientState(GL_NORMAL_ARRAY);
            glNormalPointer(GL_FLOAT, 0, modelMesh.getNormals().data());
        }

        if (!modelMesh.getTextureCoords().empty()) {
            glEnableClientState(GL_TEXTURE_COORD_ARRAY);
            glTexCoordPointer(2, GL_FLOAT, 0, modelMesh.getTextureCoords().data()); // <-- setup UVs
        }

        glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(modelMesh.getIndices().size()), GL_UNSIGNED_INT, modelMesh.getIndices().data());

        glDisableClientState(GL_VERTEX_ARRAY);
        if (!modelMesh.getNormals().empty())
            glDisableClientState(GL_NORMAL_ARRAY);
        if (!modelMesh.getTextureCoords().empty())
            glDisableClientState(GL_TEXTURE_COORD_ARRAY);


        glDisable(GL_TEXTURE_2D);
        glDisable(GL_LIGHTING); // Disable lighting after drawing the model
    }

private:
    std::vector<float> vertices; // Stores vertex positions
    std::vector<float> normals; // Stores normal vectors
    std::vector<float> textureCoords; // Stores texture coordinates
    std::vector<unsigned int> indices; // Stores indices for indexed drawing
};

#endif //MESH_H
