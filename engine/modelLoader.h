// modelLoader.h - Definição da classe Model

#ifndef MODELLOADER_H
#define MODELLOADER_H

#include <vector>
#include <string>
#include <GLUT/glut.h>

struct Vertex {
    float x, y, z;
};

class Model {
private:
    std::vector<Vertex> vertices;

public:
    std::string file;
    GLuint vbo = 0;
    GLuint vertexCount = 0;
    bool loadFromFile(const std::string& filename);
    void draw();
};

#endif // MODELLOADER_H
