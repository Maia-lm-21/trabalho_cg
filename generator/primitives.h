#ifndef PRIMITIVES_H
#define PRIMITIVES_H

#include <vector>
#include "generator.h"

void generatePlane(float size, int divisions, std::vector<Vertex>& vertices);
void generateBox(float size, int divisions, std::vector<Vertex>& vertices);
void generateSphere(float radius, int slices, int stacks, std::vector<Vertex>& vertices);
void generateCone(float radius, float height, int slices, int stacks, std::vector<Vertex>& vertices);
void generateBezier(const std::string& patch_file, int num, std::vector<Vertex>& vertices);

#endif