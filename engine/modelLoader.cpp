#include "modelLoader.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <GLUT/glut.h>

bool Model::loadFromFile(const std::string& filename) {
    std::ifstream file("../models/" + filename);
    if (!file.is_open()) {
        std::cerr << "Erro ao abrir o arquivo do modelo: " << filename << std::endl;
        return false;
    }
    vertices.clear();
    float x, y, z;

    while (file >> x >> y >> z) {
    vertices.push_back({x, y, z}); 
    }
    file.close();

    vertexCount = vertices.size();

    // Criar o VBO
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);

    // Libertar a memória RAM
    vertices.clear();

    return true;
}

void Model::draw() {
    if (vbo == 0 || vertexCount == 0) {
        std::cerr << "VBO inválido ou vazio ao tentar desenhar.\n";
        return;
    }
    glBindBuffer(GL_ARRAY_BUFFER, vbo);

    glEnableClientState(GL_VERTEX_ARRAY);
    glColor3f(1.0f, 1.0f, 1.0f);
    glVertexPointer(3, GL_FLOAT, 0, 0); // 3 floats por vértice, sem offset

    glDrawArrays(GL_TRIANGLES, 0, vertexCount);

    glDisableClientState(GL_VERTEX_ARRAY);
}
