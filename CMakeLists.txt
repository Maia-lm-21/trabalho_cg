cmake_minimum_required(VERSION 3.10)
project(EngineProject)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -arch arm64")

# Diretórios de código-fonte
set(SRC_ENGINE engine)
set(SRC_GENERATOR generator)
set(SRC_EXTERNAL external)

# Adicionar os arquivos-fonte
set(ENGINE_SOURCES 
    ${SRC_ENGINE}/engine.cpp 
    ${SRC_ENGINE}/modelLoader.cpp 
    ${SRC_ENGINE}/xmlParser.cpp 
    ${SRC_EXTERNAL}/tinyxml2.cpp
)

set(GENERATOR_SOURCES 
    ${SRC_GENERATOR}/generator.cpp 
    ${SRC_GENERATOR}/primitives.cpp
    ${SRC_GENERATOR}/patchReader.cpp
)

# Adicionar os frameworks necessários para macOS
set(APPLE_FRAMEWORKS "-framework OpenGL -framework GLUT -framework Cocoa -framework IOKit -framework CoreVideo")

# ------------------- DevIL config -------------------
# Diretórios do Homebrew (ajustados manualmente)
set(DEVIL_INCLUDE_DIR /opt/homebrew/include)
set(DEVIL_LIBRARY /opt/homebrew/lib/libIL.dylib)
set(ILU_LIBRARY /opt/homebrew/lib/libILU.dylib)
set(ILUT_LIBRARY /opt/homebrew/lib/libILUT.dylib)

include_directories(${DEVIL_INCLUDE_DIR})
# ----------------------------------------------------

# Criar executável do Engine
add_executable(engine_exec ${ENGINE_SOURCES})
target_link_libraries(engine_exec ${APPLE_FRAMEWORKS} ${DEVIL_LIBRARY} ${ILU_LIBRARY} ${ILUT_LIBRARY})

# Criar executável do Generator
add_executable(generator ${GENERATOR_SOURCES})
target_link_libraries(generator ${APPLE_FRAMEWORKS})
