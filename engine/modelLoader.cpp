#include "modelLoader.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <GLUT/glut.h>

bool Model::loadTexture() {
    if (texture.file.empty()) return false;

    ILuint t;
    ilGenImages(1, &t);
    ilBindImage(t);
    std::string path = "../test files/test_files_phase_4/" + texture.file;
    if (!ilLoadImage((ILstring)path.c_str())) {
        std::cerr << "Erro ao carregar textura: " << texture.file << std::endl;
        return false;
    }

    ilConvertImage(IL_RGBA, IL_UNSIGNED_BYTE);
    GLuint tw = ilGetInteger(IL_IMAGE_WIDTH);
    GLuint th = ilGetInteger(IL_IMAGE_HEIGHT);
    unsigned char* texData = ilGetData();

    glGenTextures(1, &texture.texID);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, texture.texID);
    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE); // <-- importante
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, tw, th, 0, GL_RGBA, GL_UNSIGNED_BYTE, texData);

    return true;
}


bool Model::loadFromFile(const std::string& filename) {
    std::ifstream file("../models/" + filename);
    if (!file.is_open()) {
        std::cerr << "Erro ao abrir o arquivo do modelo: " << filename << std::endl;
        return false;
    }

    vertices.clear();
    float x, y, z, nx, ny, nz, tu, tv;

    while (file >> x >> y >> z >> nx >> ny >> nz >> tu >> tv) {
        vertices.push_back({x, y, z, nx, ny, nz, tu, tv});
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

void Model::applyMaterial() const {
    glMaterialfv(GL_FRONT, GL_DIFFUSE, material.diffuse);
    glMaterialfv(GL_FRONT, GL_AMBIENT, material.ambient);
    glMaterialfv(GL_FRONT, GL_SPECULAR, material.specular);
    glMaterialfv(GL_FRONT, GL_EMISSION, material.emissive);
    glMaterialf(GL_FRONT, GL_SHININESS, material.shininess);
}


void Model::draw() {
    if (vbo == 0 || vertexCount == 0) {
        std::cerr << "VBO inválido ou vazio ao tentar desenhar.\n";
        return;
    }

    applyMaterial();
    
    if (texture.texID != 0) {
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, texture.texID);
    } else {
        glDisable(GL_TEXTURE_2D);
    }
    
    glBindBuffer(GL_ARRAY_BUFFER, vbo);

    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_NORMAL_ARRAY);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);

    // A estrutura Vertex está organizada em sequência na memória
    glVertexPointer(3, GL_FLOAT, sizeof(Vertex), (void*)offsetof(Vertex, x));
    glNormalPointer(GL_FLOAT, sizeof(Vertex), (void*)offsetof(Vertex, nx));
    glTexCoordPointer(2, GL_FLOAT, sizeof(Vertex), (void*)offsetof(Vertex, s));

    glColor3f(1.0f, 1.0f, 1.0f);
    glDrawArrays(GL_TRIANGLES, 0, vertexCount);

    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_NORMAL_ARRAY);
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);
}
