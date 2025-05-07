#ifndef PRIMITIVES_H
#define PRIMITIVES_H

#include <vector>

void generatePlane(float size, int divisions, std::vector<float>& vertices);
void generateBox(float size, int divisions, std::vector<float>& vertices);
void generateSphere(float radius, int slices, int stacks, std::vector<float>& vertices);
void generateCone(float radius, float height, int slices, int stacks, std::vector<float>& vertices);
void generateBezier(const std::string& patch_file, int num, std::vector<float>& vertices);

#endif