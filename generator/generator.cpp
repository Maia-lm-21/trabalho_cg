#include <iostream>
#include <fstream>
#include <vector>
#include "primitives.h"

int main (int argc, char* argv[]) {
    if (argc < 3) {
        std::cout << "Número inválido de argumentos.";
        return 1;
    }

    std::string primitiva = argv[1];
    std::string nome_ficheiro;

    std::vector<float> vertices;

    if (primitiva == "plane") {
        nome_ficheiro = std::string(argv[1]) + "_" + std::string(argv[2]) + "_" + std::string(argv[3]) + ".3d";
        float size = std::stof(argv[2]);
        int divisions = std::stoi(argv[3]);
        generatePlane(size, divisions, vertices);
    }
    else if (primitiva == "box") {
        nome_ficheiro = std::string(argv[1]) + "_" + std::string(argv[2]) + "_" + std::string(argv[3]) + ".3d";
        float size = std::stof(argv[2]);
        int divisions = std::stoi(argv[3]);
        generateBox(size, divisions, vertices);
    }
    else if (primitiva == "sphere") {
        nome_ficheiro = std::string(argv[1]) + "_" + std::string(argv[2]) + "_" + std::string(argv[3]) + "_" + std::string(argv[4]) + ".3d";
        float radius = std::stof(argv[2]);
        int slices = std::stoi(argv[3]);
        int stacks = std::stoi(argv[4]);
        generateSphere(radius, slices, stacks, vertices);
    }
    else if (primitiva == "cone") {
        nome_ficheiro = std::string(argv[1]) + "_" + std::string(argv[2]) + "_" + std::string(argv[3]) + "_" + std::string(argv[4]) + "_" + std::string(argv[5]) + ".3d";
        float radius = std::stof(argv[2]);
        float height = std::stof(argv[3]);
        int slices = std::stoi(argv[4]);
        int stacks = std::stoi(argv[5]);
        generateCone(radius, height, slices, stacks, vertices);
    }
    else if (primitiva == "patch"){
        //<!-- generator patch teapot.patch 10 bezier_10.3d -->
        nome_ficheiro = std::string("bezier_" + std::string(argv[3]) + ".3d");
        int tesselation_num = std::stoi(argv[3]);
        std::string fich_patch = std::string(argv[2]);
        generateBezier(fich_patch, tesselation_num, vertices);
    }
    else {
        std::cout << "Erro na primitiva";
        return 1;
    }
    
    // Escrita dos vértices gerados no ficheiro
    std::ofstream file("../models/" + nome_ficheiro);
    if (!file.is_open()) {
        std::cerr << "Erro ao abrir o ficheiro: " << nome_ficheiro << std::endl;
        return 1;
    }
    
    
    for (size_t i = 0; i < vertices.size(); i += 3) {
        file << vertices[i] << " " << vertices[i+1] << " " << vertices[i+2] << std::endl;
    }
    
    file.close();
    std::cout << "Ficheiro gerado com sucesso: " << nome_ficheiro << std::endl;
    
    return 0;
}
