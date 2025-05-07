#include <vector>
#include <iostream>
#include "primitives.h"
#include "patchReader.h"

#define _USE_MATH_DEFINES
#include <math.h>


void generatePlane(float size, int divisions, std::vector<float>& vertices){
    float metade = size / 2.0f;
    float tamanho_celula = size / divisions;

    for (int i = 0; i < divisions; i++) {
        for (int j = 0; j < divisions; j++) {
            float x1 = -metade + j * tamanho_celula;
            float z1 = -metade + i * tamanho_celula;
            float x2 = x1 + tamanho_celula;
            float z2 = z1 + tamanho_celula;

            // Primeiro triângulo da célula
            vertices.insert(vertices.end(), {x2, 0.0f, z2});
            vertices.insert(vertices.end(), {x2, 0.0f, z1});
            vertices.insert(vertices.end(), {x1, 0.0f, z1});
            
            
            // Segundo triângulo da célula
            vertices.insert(vertices.end(), {x1, 0.0f, z1});
            vertices.insert(vertices.end(), {x1, 0.0f, z2});
            vertices.insert(vertices.end(), {x2, 0.0f, z2});
        }
    }
}




void generateBox(float size, int divisions, std::vector<float>& vertices) {
    float metade = size / 2.0f;
    float tamanho_celula = size / divisions;

    for (int face = 0; face < 6; face++) {
        for (int i = 0; i < divisions; i++) {
            for (int j = 0; j < divisions; j++) {
                float x1 = -metade + j * tamanho_celula;
                float y1 = -metade + i * tamanho_celula;
                float x2 = x1 + tamanho_celula;
                float y2 = y1 + tamanho_celula;

                switch (face) {
                    case 0:  // Frente (z positivo)
                        vertices.insert(vertices.end(), {x2, y2, metade, x1, y1, metade, x2, y1, metade});
                        vertices.insert(vertices.end(), {x1, y1, metade, x2, y2, metade, x1, y2, metade});
                        break;
                    case 1:  // Trás (z negativo) 
                        vertices.insert(vertices.end(), {x1, y1, -metade, x1, y2, -metade, x2, y2, -metade});
                        vertices.insert(vertices.end(), {x2, y1, -metade, x1, y1, -metade, x2, y2, -metade});
                        break;
                    case 2:  // Direita (x positivo) 
                        vertices.insert(vertices.end(), {metade, y2, x2, metade, y1, x2, metade, y1, x1});
                        vertices.insert(vertices.end(), {metade, y2, x2, metade, y1, x1, metade, y2, x1});
                        break;
                    case 3:  // Esquerda (x negativo) 
                        vertices.insert(vertices.end(), {-metade, y1, x1, -metade, y1, x2, -metade, y2, x2});
                        vertices.insert(vertices.end(), {-metade, y2, x2, -metade, y2, x1, -metade, y1, x1});
                        break;
                    case 4:  // Topo (y positivo)
                        vertices.insert(vertices.end(), {x1, metade, y1, x2, metade, y2, x2, metade, y1});
                        vertices.insert(vertices.end(), {x1, metade, y1, x1, metade, y2, x2, metade, y2});
                        break;
                    case 5:  // Fundo (y negativo)
                        vertices.insert(vertices.end(), {x1, -metade, y1, x2, -metade, y1, x2, -metade, y2});
                        vertices.insert(vertices.end(), {x2, -metade, y2, x1, -metade, y2, x1, -metade, y1});
                        break;
                }
            }
        }
    }
}


void generateSphere(float radius, int slices, int stacks, std::vector<float>& vertices) {
    vertices.clear();
    
    for (int i = 0; i < stacks; i++) {
        float theta1 = M_PI * (float(i) / stacks);
        float theta2 = M_PI * (float(i + 1) / stacks);

        for (int j = 0; j < slices; j++) {
            float phi1 = 2.0f * M_PI * (float(j+0.5) / slices);
            float phi2 = 2.0f * M_PI * (float(j + 1.5) / slices);

            float x1 = radius * sin(theta1) * cos(phi1);
            float y1 = radius * cos(theta1);
            float z1 = radius * sin(theta1) * sin(phi1);

            float x2 = radius * sin(theta1) * cos(phi2);
            float y2 = radius * cos(theta1);
            float z2 = radius * sin(theta1) * sin(phi2);

            float x3 = radius * sin(theta2) * cos(phi1);
            float y3 = radius * cos(theta2);
            float z3 = radius * sin(theta2) * sin(phi1);

            float x4 = radius * sin(theta2) * cos(phi2);
            float y4 = radius * cos(theta2);
            float z4 = radius * sin(theta2) * sin(phi2);
            
            // vértices dos triangulos gerados
            
            vertices.push_back(x1); vertices.push_back(y1); vertices.push_back(z1);
            vertices.push_back(x2); vertices.push_back(y2); vertices.push_back(z2);
            vertices.push_back(x3); vertices.push_back(y3); vertices.push_back(z3);

            vertices.push_back(x3); vertices.push_back(y3); vertices.push_back(z3);
            vertices.push_back(x2); vertices.push_back(y2); vertices.push_back(z2);
            vertices.push_back(x4); vertices.push_back(y4); vertices.push_back(z4);
            
        }
    }
}




void generateCone(float radius, float height, int slices, int stacks, std::vector<float>& vertices) {
    float stackHeight = height / stacks;
    float angleStep = 2 * M_PI / slices;
    float radiusStep = radius / stacks;

    // Base do cone
    for (int i = 0; i < slices; i++) {
        float theta = i * angleStep;
        float nextTheta = (float(i + 1)) * angleStep;

        // Centro da base
        vertices.insert(vertices.end(), {0.0f, 0.0f, 0.0f});
        // Ponto no círculo atual
        vertices.insert(vertices.end(), {radius * cos(theta), 0.0f, radius * sin(theta)});
        // Próximo ponto no círculo
        vertices.insert(vertices.end(), {radius * cos(nextTheta), 0.0f, radius * sin(nextTheta)});
    }

    // Superfícies laterais do cone
    for (int j = 0; j < stacks; j++) {
        float currentHeight = float(j) * stackHeight;
        float nextHeight = float(j + 1) * stackHeight;
        float currentRadius = radius - float(j) * radiusStep;
        float nextRadius = radius - float(j + 1) * radiusStep;

        for (int i = 0; i < slices; i++) {
            float theta = float(i) * angleStep;
            float nextTheta = (float(i + 1)) * angleStep;

            // Vértices da stack atual
            float x1 = currentRadius * cos(theta);
            float z1 = currentRadius * sin(theta);
            float x2 = currentRadius * cos(nextTheta);
            float z2 = currentRadius * sin(nextTheta);

            // Vértices da próxima stack
            float x3 = nextRadius * cos(theta);
            float z3 = nextRadius * sin(theta);
            float x4 = nextRadius * cos(nextTheta);
            float z4 = nextRadius * sin(nextTheta);

            // Triângulo 1 (inferior)
            vertices.insert(vertices.end(), {x1, currentHeight, z1});
            vertices.insert(vertices.end(), {x3, nextHeight, z3});
            vertices.insert(vertices.end(), {x4, nextHeight, z4});

            // Triângulo 2 (superior)
            vertices.insert(vertices.end(), {x2, currentHeight, z2});
            vertices.insert(vertices.end(), {x1, currentHeight, z1});
            vertices.insert(vertices.end(), {x4, nextHeight, z4});
            
            
        }
    }
}

 // Compute Bernstein basis for cubic Bezier
float bernstein(int i, float t) {
    switch (i) {
        case 0: return (1 - t) * (1 - t) * (1 - t);
        case 1: return 3 * t * (1 - t) * (1 - t);
        case 2: return 3 * t * t * (1 - t);
        case 3: return t * t * t;
        default: return 0;
    }
}

// Evaluate position on a patch at (u, v)
Vec3 evaluateBezierPatch(const std::vector<Vec3>& controlPoints, const Patch& patch, float u, float v) {
    Vec3 p = {0, 0, 0};
    for (int i = 0; i < 4; ++i) {
        float bu = bernstein(i, u);
        for (int j = 0; j < 4; ++j) {
            float bv = bernstein(j, v);
            int idx = patch.controlPointIndices[j * 4 + i];
            p.x += bu * bv * controlPoints[idx].x;
            p.y += bu * bv * controlPoints[idx].y;
            p.z += bu * bv * controlPoints[idx].z;
       }
    }
    return p;
}

void generateBezier(const std::string& patch_file, int num, std::vector<float>& vertices) {
    std::vector<Patch> patches;
    std::vector<Vec3> controlPoints;

    //read the patch_file
    if (!loadPatchFile(patch_file, patches, controlPoints)) {
        std::cerr << "Failed to load patch file.\n";
        return;
    }

    //create the vertexs and insert vertexs in the vector "vertices"
    for (const Patch& patch : patches) {
        // Subdivide the patch into num×num grid of quads (each quad = 2 triangles)
        for (int i = 0; i < num; ++i) {
            float u0 = (float)i / num;
            float u1 = (float)(i + 1) / num;
            for (int j = 0; j < num; ++j) {
                float v0 = (float)j / num;
                float v1 = (float)(j + 1) / num;

                Vec3 p00 = evaluateBezierPatch(controlPoints, patch, u0, v0);
                Vec3 p10 = evaluateBezierPatch(controlPoints, patch, u1, v0);
                Vec3 p01 = evaluateBezierPatch(controlPoints, patch, u0, v1);
                Vec3 p11 = evaluateBezierPatch(controlPoints, patch, u1, v1);

                // Triangle 1: p01, p00, p10
                vertices.push_back(p01.x); vertices.push_back(p01.y); vertices.push_back(p01.z);
                vertices.push_back(p00.x); vertices.push_back(p00.y); vertices.push_back(p00.z);
                vertices.push_back(p10.x); vertices.push_back(p10.y); vertices.push_back(p10.z);

                // Triangle 2: p01, p10, p11
                vertices.push_back(p01.x); vertices.push_back(p01.y); vertices.push_back(p01.z);
                vertices.push_back(p10.x); vertices.push_back(p10.y); vertices.push_back(p10.z);
                vertices.push_back(p11.x); vertices.push_back(p11.y); vertices.push_back(p11.z);
            }
        }
    }
}