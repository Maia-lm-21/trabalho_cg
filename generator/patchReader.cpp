#include "patchReader.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <algorithm>

bool loadPatchFile(const std::string& filename, std::vector<Patch>& patches, std::vector<Vec3>& controlPoints) {
    std::ifstream file(filename);
    if (!file.is_open()) return false;

    std::string line;

    // Lê número de patches
    if (!std::getline(file, line)) return false;
    int numPatches = std::stoi(line);

    // Lê os patches
    for (int i = 0; i < numPatches; ++i) {
        if (!std::getline(file, line)) return false;

        // Substitui vírgulas por espaços
        std::replace(line.begin(), line.end(), ',', ' ');
        std::istringstream iss(line);

        Patch patch;
        int index;
        for (int j = 0; j < 16; ++j) {
            if (!(iss >> index)) {
                std::cerr << "Erro3 na linha do patch " << i << ": " << line << "\n";
                return false;
            }
            patch.controlPointIndices.push_back(index);
        }
        patches.push_back(patch);
    }

    // Lê número de pontos de controlo
    if (!std::getline(file, line)) return false;
    int numPoints = std::stoi(line);

    // Lê os pontos de controlo
    for (int i = 0; i < numPoints; ++i) {
        if (!std::getline(file, line)) return false;

        std::replace(line.begin(), line.end(), ',', ' ');
        std::istringstream iss(line);

        Vec3 point;
        if (!(iss >> point.x >> point.y >> point.z)) {
            std::cerr << "Erro4 ao ler ponto de controlo " << i << ": " << line << "\n";
            return false;
        }
        controlPoints.push_back(point);
    }

    return true;
}