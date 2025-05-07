#ifndef ENGINE_H
#define ENGINE_H

#include "xmlParser.h"  // Para ter acesso à estrutura Config

class Engine {
public:
    bool init(const char* configFile);  // Inicializa o engine
    void run();  // Inicia o loop principal do OpenGL
};

#endif // ENGINE_H