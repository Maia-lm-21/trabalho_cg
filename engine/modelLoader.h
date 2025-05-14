// modelLoader.h - Definição da classe Model

#ifndef MODELLOADER_H
#define MODELLOADER_H

#include <vector>
#include <string>
#include <GLUT/glut.h>
#include <IL/il.h>

struct Vertex {
    float x, y, z;       // posição
    float nx, ny, nz;    // normal
    float s, t;          // coordenadas de textura
};

struct Materials {
    float diffuse[3], ambient[3], specular[3], emissive[3];
    float shininess;
};

struct Textures {
    std::string file;
    GLuint texID = 0;
};

class Model {
private:
    std::vector<Vertex> vertices;

public:
    std::string file;
    GLuint vbo = 0;
    GLuint vertexCount = 0;
    Materials material;
    Textures texture;
    bool loadFromFile(const std::string& filename);
    bool loadTexture();
    void applyMaterial()const;
    void draw();
};

#endif // MODELLOADER_H
