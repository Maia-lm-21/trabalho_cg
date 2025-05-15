#include <vector>
#include <iostream>
#include "primitives.h"
#include "patchReader.h"

#define _USE_MATH_DEFINES
#include <math.h>

Vec3 calculateNormal(const Vec3& p1, const Vec3& p2, const Vec3& p3) {
    Vec3 v1 = {p2.x - p1.x, p2.y - p1.y, p2.z - p1.z};  // Vetor de p1 para p2
    Vec3 v2 = {p3.x - p1.x, p3.y - p1.y, p3.z - p1.z};  // Vetor de p1 para p3

    // Produto vetorial de v1 e v2 (normal)
    Vec3 normal = {
        v1.y * v2.z - v1.z * v2.y,
        v1.z * v2.x - v1.x * v2.z,
        v1.x * v2.y - v1.y * v2.x
    };

    return normal;
}

void normalize(Vec3& normal) {
    float length = sqrt(normal.x * normal.x + normal.y * normal.y + normal.z * normal.z);
    if (length > 0.0f) {
        normal.x /= length;
        normal.y /= length;
        normal.z /= length;
    }
}


void generatePlane(float size, int divisions, std::vector<Vertex>& vertices){
    float metade = size / 2.0f;
    float tamanho_celula = size / divisions;

    for (int i = 0; i < divisions; i++) {
        for (int j = 0; j < divisions; j++) {
            float x1 = -metade + j * tamanho_celula;
            float z1 = -metade + i * tamanho_celula;
            float x2 = x1 + tamanho_celula;
            float z2 = z1 + tamanho_celula;

            // Normais (para o plano, todos os vértices terão a mesma normal)
            float nx = 0.0f, ny = 1.0f, nz = 0.0f;

            // Coordenadas de textura (simples mapeamento de coordenadas)
            float u1 = float(j) / divisions, v1 = float(i) / divisions;
            float u2 = float(j+1) / divisions, v2 = float(i+1) / divisions;

            // Primeiro triângulo da célula
            vertices.insert(vertices.end(), {x2, 0.0f, z2, nx, ny, nz, u2, v2});
            vertices.insert(vertices.end(), {x2, 0.0f, z1, nx, ny, nz, u2, v1});
            vertices.insert(vertices.end(), {x1, 0.0f, z1, nx, ny, nz, u1, v1});

            // Segundo triângulo da célula
            vertices.insert(vertices.end(), {x1, 0.0f, z1, nx, ny, nz, u1, v1});
            vertices.insert(vertices.end(), {x1, 0.0f, z2, nx, ny, nz, u1, v2});
            vertices.insert(vertices.end(), {x2, 0.0f, z2, nx, ny, nz, u2, v2});
        }
    }
}




void generateBox(float size, int divisions, std::vector<Vertex>& vertices) {
    float metade = size / 2.0f;
    float tamanho_celula = size / divisions;

    for (int face = 0; face < 6; face++) {
        for (int i = 0; i < divisions; i++) {
            for (int j = 0; j < divisions; j++) {
                float x1 = -metade + j * tamanho_celula;
                float y1 = -metade + i * tamanho_celula;
                float x2 = x1 + tamanho_celula;
                float y2 = y1 + tamanho_celula;

                // Calculando as normais para cada face
                float nx = 0.0f, ny = 0.0f, nz = 0.0f;
                if (face == 0) { nz = 1.0f; }  // Frente
                else if (face == 1) { nz = -1.0f; }  // Trás
                else if (face == 2) { nx = 1.0f; }  // Direita
                else if (face == 3) { nx = -1.0f; }  // Esquerda
                else if (face == 4) { ny = 1.0f; }  // Topo
                else if (face == 5) { ny = -1.0f; }  // Fundo

                // Coordenadas de textura
                float u1 = float(j) / divisions, v1 = float(i) / divisions;
                float u2 = float(j+1) / divisions, v2 = float(i+1) / divisions;


                switch (face) {
                    case 0:  // Frente (z positivo)
                        vertices.insert(vertices.end(), {x2, y2, metade, nx, ny, nz, u2, v2});
                        vertices.insert(vertices.end(), {x1, y1, metade, nx, ny, nz, u1, v1});
                        vertices.insert(vertices.end(), {x2, y1, metade, nx, ny, nz, u2, v1});

                        vertices.insert(vertices.end(), {x1, y1, metade, nx, ny, nz, u1, v1});
                        vertices.insert(vertices.end(), {x2, y2, metade, nx, ny, nz, u2, v2});
                        vertices.insert(vertices.end(), {x1, y2, metade, nx, ny, nz, u1, v2});
                        break;

                    case 1:  // Trás (z negativo)
                        vertices.insert(vertices.end(), {x1, y1, -metade, nx, ny, nz, u1, v1});
                        vertices.insert(vertices.end(), {x1, y2, -metade, nx, ny, nz, u1, v2});
                        vertices.insert(vertices.end(), {x2, y2, -metade, nx, ny, nz, u2, v2});

                        vertices.insert(vertices.end(), {x2, y1, -metade, nx, ny, nz, u2, v1});
                        vertices.insert(vertices.end(), {x1, y1, -metade, nx, ny, nz, u1, v1});
                        vertices.insert(vertices.end(), {x2, y2, -metade, nx, ny, nz, u2, v2});
                        break;

                    case 2:  // Direita (x positivo)
                        vertices.insert(vertices.end(), {metade, y2, x2, nx, ny, nz, u2, v2});
                        vertices.insert(vertices.end(), {metade, y1, x2, nx, ny, nz, u2, v1});
                        vertices.insert(vertices.end(), {metade, y1, x1, nx, ny, nz, u1, v1});

                        vertices.insert(vertices.end(), {metade, y2, x2, nx, ny, nz, u2, v2});
                        vertices.insert(vertices.end(), {metade, y1, x1, nx, ny, nz, u1, v1});
                        vertices.insert(vertices.end(), {metade, y2, x1, nx, ny, nz, u1, v2});
                        break;

                    case 3:  // Esquerda (x negativo)
                        vertices.insert(vertices.end(), {-metade, y1, x1, nx, ny, nz, u1, v1});
                        vertices.insert(vertices.end(), {-metade, y1, x2, nx, ny, nz, u2, v1});
                        vertices.insert(vertices.end(), {-metade, y2, x2, nx, ny, nz, u2, v2});

                        vertices.insert(vertices.end(), {-metade, y2, x1, nx, ny, nz, u1, v2});
                        vertices.insert(vertices.end(), {-metade, y1, x1, nx, ny, nz, u1, v1});
                        vertices.insert(vertices.end(), {-metade, y2, x2, nx, ny, nz, u2, v2});
                        break;

                    case 4:  // Topo (y positivo)
                        vertices.insert(vertices.end(), {x1, metade, y1, nx, ny, nz, u1, v1});
                        vertices.insert(vertices.end(), {x2, metade, y2, nx, ny, nz, u2, v2});
                        vertices.insert(vertices.end(), {x2, metade, y1, nx, ny, nz, u2, v1});

                        vertices.insert(vertices.end(), {x1, metade, y1, nx, ny, nz, u1, v1});
                        vertices.insert(vertices.end(), {x1, metade, y2, nx, ny, nz, u1, v2});
                        vertices.insert(vertices.end(), {x2, metade, y2, nx, ny, nz, u2, v2});
                        break;

                    case 5:  // Fundo (y negativo)
                        vertices.insert(vertices.end(), {x1, -metade, y1, nx, ny, nz, u1, v1});
                        vertices.insert(vertices.end(), {x2, -metade, y1, nx, ny, nz, u2, v1});
                        vertices.insert(vertices.end(), {x2, -metade, y2, nx, ny, nz, u2, v2});
                        
                        vertices.insert(vertices.end(), {x2, -metade, y2, nx, ny, nz, u2, v2});
                        vertices.insert(vertices.end(), {x1, -metade, y2, nx, ny, nz, u1, v2});
                        vertices.insert(vertices.end(), {x1, -metade, y1, nx, ny, nz, u1, v1});
                        break;
                }
            }
        }
    }
}


void generateSphere(float radius, int slices, int stacks, std::vector<Vertex>& vertices) {
    vertices.clear();
    
    for (int i = 0; i < stacks; i++) {
        float theta1 = M_PI * (float(i) / stacks);
        float theta2 = M_PI * (float(i + 1) / stacks);

        for (int j = 0; j < slices; j++) {
            float phi1 = 2.0f * M_PI * float(j+0.5) / slices;
            float phi2 = 2.0f * M_PI * float(j + 1.5) / slices;

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

            // Normais para cada vértice
            float nx1 = x1 / radius, ny1 = y1 / radius, nz1 = z1 / radius;
            float nx2 = x2 / radius, ny2 = y2 / radius, nz2 = z2 / radius;
            float nx3 = x3 / radius, ny3 = y3 / radius, nz3 = z3 / radius;
            float nx4 = x4 / radius, ny4 = y4 / radius, nz4 = z4 / radius;
            
            //coordenadas de textura
            float u1 = 1.0f - (phi1 / (2.0f * M_PI));
            float u2 = 1.0f - (phi2 / (2.0f * M_PI));
            float v1 = theta1 / M_PI;
            float v2 = theta2 / M_PI;


            // vértices dos triangulos gerados
            
            // Triângulos
            vertices.push_back({x1, y1, z1, nx1, ny1, nz1, u1, v1});
            vertices.push_back({x2, y2, z2, nx2, ny2, nz2, u2, v1});
            vertices.push_back({x3, y3, z3, nx3, ny3, nz3, u1, v2});

            vertices.push_back({x3, y3, z3, nx3, ny3, nz3, u1, v2});
            vertices.push_back({x2, y2, z2, nx2, ny2, nz2, u2, v1});
            vertices.push_back({x4, y4, z4, nx4, ny4, nz4, u2, v2});
            
        }
    }
}


void generateCone(float radius, float height, int slices, int stacks, std::vector<Vertex>& vertices) {
    vertices.clear();

    float stackHeight = height / stacks;
    float angleStep = 2.0f * M_PI / slices;
    float radiusStep = radius / stacks;
    float slantHeight = sqrt(height * height + radius * radius);

    // Base do cone (normal para baixo)
    for (int i = 0; i < slices; i++) {
        float theta = i * angleStep;
        float nextTheta = (i + 1) * angleStep;

        // Centro da base
        vertices.push_back({0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.5f, 0.5f});

        // Pontos na borda da base
        vertices.push_back({radius * cos(theta), 0.0f, radius * sin(theta), 0.0f, -1.0f, 0.0f, float(i) / slices, 0.0f});
        vertices.push_back({radius * cos(nextTheta), 0.0f, radius * sin(nextTheta), 0.0f, -1.0f, 0.0f, float(i + 1) / slices, 0.0f});
    }

    // Superfície lateral do cone
    for (int j = 0; j < stacks; j++) {
        float currentHeight = j * stackHeight;
        float nextHeight = (j + 1) * stackHeight;
        float currentRadius = radius - j * radiusStep;
        float nextRadius = radius - (j + 1) * radiusStep;

        for (int i = 0; i < slices; i++) {
            float theta = i * angleStep;
            float nextTheta = (i + 1) * angleStep;

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

            // Normais laterais (suavizadas)
            float nx1 = (height / slantHeight) * cos(theta);
            float ny1 = radius / slantHeight;
            float nz1 = (height / slantHeight) * sin(theta);

            float nx2 = (height / slantHeight) * cos(nextTheta);
            float ny2 = radius / slantHeight;
            float nz2 = (height / slantHeight) * sin(nextTheta);

            // Coordenadas de textura
            float u1 = float(i) / slices;
            float u2 = float(i + 1) / slices;

            // Primeiro triângulo da face lateral
            vertices.push_back({x1, currentHeight, z1, nx1, ny1, nz1, u1, float(j) / stacks});
            vertices.push_back({x3, nextHeight, z3, nx1, ny1, nz1, u1, float(j + 1) / stacks});
            vertices.push_back({x4, nextHeight, z4, nx2, ny2, nz2, u2, float(j + 1) / stacks});

            // Segundo triângulo da face lateral
            vertices.push_back({x2, currentHeight, z2, nx2, ny2, nz2, u2, float(j) / stacks});
            vertices.push_back({x1, currentHeight, z1, nx1, ny1, nz1, u1, float(j) / stacks});
            vertices.push_back({x4, nextHeight, z4, nx2, ny2, nz2, u2, float(j + 1) / stacks});
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

float bernsteinDerivative(int i, float t) {
    switch (i) {
        case 0: return -3 * (1 - t) * (1 - t);
        case 1: return 3 * (1 - t) * (1 - t) - 6 * t * (1 - t);
        case 2: return 6 * t * (1 - t) - 3 * t * t;
        case 3: return 3 * t * t;
        default: return 0;
    }
}

struct Vec3Derivatives {
    Vec3 du;
    Vec3 dv;
};

Vec3Derivatives evaluateBezierPatchDerivatives(const std::vector<Vec3>& controlPoints, const Patch& patch, float u, float v) {
    Vec3 du = {0, 0, 0};
    Vec3 dv = {0, 0, 0};

    for (int i = 0; i < 4; ++i) {
        float bu = bernstein(i, u);
        float dbu = bernsteinDerivative(i, u);
        for (int j = 0; j < 4; ++j) {
            float bv = bernstein(j, v);
            float dbv = bernsteinDerivative(j, v);
            int idx = patch.controlPointIndices[j * 4 + i];

           
            du.x += dbu * bv * controlPoints[idx].x;
            du.y += dbu * bv * controlPoints[idx].y;
            du.z += dbu * bv * controlPoints[idx].z;

            
            dv.x += bu * dbv * controlPoints[idx].x;
            dv.y += bu * dbv * controlPoints[idx].y;
            dv.z += bu * dbv * controlPoints[idx].z;
        }
    }

    return {du, dv};
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

Vec3 cross(const Vec3& a, const Vec3& b) {
    return Vec3 {
        a.y * b.z - a.z * b.y,
        a.z * b.x - a.x * b.z,
        a.x * b.y - a.y * b.x
    };
}

void generateBezier(const std::string& patch_file, int num, std::vector<Vertex>& vertices) {
    std::vector<Patch> patches;
    std::vector<Vec3> controlPoints;

    if (!loadPatchFile(patch_file, patches, controlPoints)) {
        std::cerr << "Failed to load patch file.\n";
        return;
    }

    for (const Patch& patch : patches) {
        for (int i = 0; i < num; ++i) {
            float u0 = float(i) / num;
            float u1 = float(i + 1) / num;
            for (int j = 0; j < num; ++j) {
                float v0 = float(j) / num;
                float v1 = float(j + 1) / num;

                Vec3 p00 = evaluateBezierPatch(controlPoints, patch, u0, v0);
                Vec3 p10 = evaluateBezierPatch(controlPoints, patch, u1, v0);
                Vec3 p01 = evaluateBezierPatch(controlPoints, patch, u0, v1);
                Vec3 p11 = evaluateBezierPatch(controlPoints, patch, u1, v1);

                Vec3Derivatives d00 = evaluateBezierPatchDerivatives(controlPoints, patch, u0, v0);
                Vec3Derivatives d10 = evaluateBezierPatchDerivatives(controlPoints, patch, u1, v0);
                Vec3Derivatives d01 = evaluateBezierPatchDerivatives(controlPoints, patch, u0, v1);
                Vec3Derivatives d11 = evaluateBezierPatchDerivatives(controlPoints, patch, u1, v1);

                Vec3 normal00 = cross(d00.du, d00.dv);
                Vec3 normal10 = cross(d10.du, d10.dv);
                Vec3 normal01 = cross(d01.du, d01.dv);
                Vec3 normal11 = cross(d11.du, d11.dv);

                normalize(normal00);
                normalize(normal10);
                normalize(normal01);
                normalize(normal11);

                // Triângulo 1: p01, p00, p10
                vertices.push_back({p01.x, p01.y, p01.z, normal01.x, normal01.y, normal01.z, u0, v1});
                vertices.push_back({p00.x, p00.y, p00.z, normal00.x, normal00.y, normal00.z, u0, v0});
                vertices.push_back({p10.x, p10.y, p10.z, normal10.x, normal10.y, normal10.z, u1, v0});

                // Triângulo 2: p01, p10, p11
                vertices.push_back({p01.x, p01.y, p01.z, normal01.x, normal01.y, normal01.z, u0, v1});
                vertices.push_back({p10.x, p10.y, p10.z, normal10.x, normal10.y, normal10.z, u1, v0});
                vertices.push_back({p11.x, p11.y, p11.z, normal11.x, normal11.y, normal11.z, u1, v1});
            }
        }
    }
}