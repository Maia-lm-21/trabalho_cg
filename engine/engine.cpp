#include "modelLoader.h"
#include "xmlParser.h"
#include <GLUT/glut.h>
#include <vector>
#include <iostream>
#include "engine.h"

Config config;               // Armazena a configuração carregada do XML
std::vector<Model> models;   // Lista de modelos carregados

// Calcula ponto na curva Catmull-Rom
void getCatmullRomPoint(float t, float p0[3], float p1[3], float p2[3], float p3[3], float* pos, float* deriv = nullptr) {
    float m[4][4] = {
        {-0.5f,  1.5f, -1.5f, 0.5f},
        { 1.0f, -2.5f,  2.0f, -0.5f},
        {-0.5f,  0.0f,  0.5f, 0.0f},
        { 0.0f,  1.0f,  0.0f, 0.0f}
    };

    for (int i = 0; i < 3; i++) {
        float a[4] = {
            m[0][0]*p0[i] + m[0][1]*p1[i] + m[0][2]*p2[i] + m[0][3]*p3[i],
            m[1][0]*p0[i] + m[1][1]*p1[i] + m[1][2]*p2[i] + m[1][3]*p3[i],
            m[2][0]*p0[i] + m[2][1]*p1[i] + m[2][2]*p2[i] + m[2][3]*p3[i],
            m[3][0]*p0[i] + m[3][1]*p1[i] + m[3][2]*p2[i] + m[3][3]*p3[i]
        };

        pos[i] = a[0]*t*t*t + a[1]*t*t + a[2]*t + a[3];

        if (deriv) {
            deriv[i] = 3*a[0]*t*t + 2*a[1]*t + a[2];
        }
    }
}

void changeSize(int w, int h) {

	// Prevent a divide by zero, when window is too short
	// (you cant make a window with zero width).
	if(h == 0)
		h = 1;

	// compute window's aspect ratio 
	float ratio = w * 1.0 / h;

	// Set the projection matrix as current
	glMatrixMode(GL_PROJECTION);
	// Load Identity Matrix
	glLoadIdentity();
	
	// Set the viewport to be the entire window
    glViewport(0, 0, w, h);

	// Set perspective
	gluPerspective(45.0f ,ratio, 1.0f ,1000.0f);

	// return to the model view matrix mode
	glMatrixMode(GL_MODELVIEW);
}

void setupLights(const std::vector<Light>& lights) {
    glEnable(GL_LIGHTING); // Garante que a iluminação está ligada
    float amb[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, amb);

    int lightIndex = 0;
    for (const auto& light : lights) {
        if (lightIndex >= GL_LIGHT7 - GL_LIGHT0 + 1) {
            std::cerr << "Apenas até 8 luzes são suportadas (GL_LIGHT0 a GL_LIGHT7)." << std::endl;
            break;
        }

        GLenum glLight = GL_LIGHT0 + lightIndex;
        glEnable(glLight);
        

        if (light.type == "point") {
            GLfloat pos[4] = { light.position[0], light.position[1], light.position[2], 1.0f };
            glLightfv(glLight, GL_POSITION, pos);
            float dark[4] = {0.2, 0.2, 0.2, 1.0};
            float white[4] = {1.0, 1.0, 1.0, 1.0};
            float black[4] = {0.0f, 0.0f, 0.0f, 0.0f};
            // light colors
            glLightfv(glLight, GL_AMBIENT, dark);
            glLightfv(glLight, GL_DIFFUSE, white);
            glLightfv(glLight, GL_SPECULAR, white);

        } else if (light.type == "directional") {
            GLfloat dir[4] = { light.direction[0], light.direction[1], light.direction[2], 0.0f };
            glLightfv(glLight, GL_SPOT_DIRECTION, dir);
            float dark[4] = {0.2, 0.2, 0.2, 1.0};
            float white[4] = {1.0, 1.0, 1.0, 1.0};
            float black[4] = {0.0f, 0.0f, 0.0f, 0.0f};
            // light colors
            glLightfv(glLight, GL_AMBIENT, dark);
            glLightfv(glLight, GL_DIFFUSE, white);
            glLightfv(glLight, GL_SPECULAR, white);

        } else if (light.type == "spotlight") {
            GLfloat pos[4] = { light.position[0], light.position[1], light.position[2], 1.0f };
            GLfloat dir[] = { light.direction[0], light.direction[1], light.direction[2] };
            glLightfv(glLight, GL_POSITION, pos);
            glLightfv(glLight, GL_SPOT_DIRECTION, dir);
            glLightf(glLight, GL_SPOT_CUTOFF, light.cutoff);
            float dark[4] = {0.2, 0.2, 0.2, 1.0};
            float white[4] = {1.0, 1.0, 1.0, 1.0};
            float black[4] = {0.0f, 0.0f, 0.0f, 0.0f};
            // light colors
            glLightfv(glLight, GL_AMBIENT, dark);
            glLightfv(glLight, GL_DIFFUSE, white);
            glLightfv(glLight, GL_SPECULAR, white);
        }

        lightIndex++;
    }
}

Model* findModelByFile(const std::string& file) {
    for (Model& m : models) {
        if (m.file == file)
            return &m;
    }
    return nullptr;
}

void drawGroup(const Group& group) {
    glPushMatrix(); // Guarda a matriz de transformação atual

    for (const auto& step : group.transformOrder) {
        if (step.type == TRANSLATE) {
            if (step.hasTime && !step.curvePoints.empty()) {
                float globalTime = glutGet(GLUT_ELAPSED_TIME) / 1000.0f;
                float t = fmod(globalTime / step.time, 1.0f); 

                int pointCount = step.curvePoints.size();
                int index = (int)(t * pointCount);
                float localT = (t * pointCount) - index;

                int indices[4] = {
                    (index + pointCount - 1) % pointCount,
                    (index + 0) % pointCount,
                    (index + 1) % pointCount,
                    (index + 2) % pointCount
                };

                float p0[3], p1[3], p2[3], p3[3];
                for (int i = 0; i < 3; i++) {
                    p0[i] = step.curvePoints[indices[0]][i];
                    p1[i] = step.curvePoints[indices[1]][i];
                    p2[i] = step.curvePoints[indices[2]][i];
                    p3[i] = step.curvePoints[indices[3]][i];
                }

                // Calcular posição atual
                float pos[3], deriv[3];
                getCatmullRomPoint(localT, p0, p1, p2, p3, pos, deriv);
                glTranslatef(pos[0], pos[1], pos[2]);

                if (step.align) {
                    float x[3], y[3] = {0, 1, 0}, z[3];

                    float norm = sqrt(deriv[0]*deriv[0] + deriv[1]*deriv[1] + deriv[2]*deriv[2]);
                    for (int i = 0; i < 3; i++) z[i] = deriv[i] / norm;

                    x[0] = y[1]*z[2] - y[2]*z[1];
                    x[1] = y[2]*z[0] - y[0]*z[2];
                    x[2] = y[0]*z[1] - y[1]*z[0];

                    y[0] = z[1]*x[2] - z[2]*x[1];
                    y[1] = z[2]*x[0] - z[0]*x[2];
                    y[2] = z[0]*x[1] - z[1]*x[0];

                    float m[16] = {
                        x[0], y[0], z[0], 0,
                        x[1], y[1], z[1], 0,
                        x[2], y[2], z[2], 0,
                        0,    0,    0,    1
                    };

                    glMultMatrixf(m);
                }
            } else {
                glTranslatef(step.values[0], step.values[1], step.values[2]);
            }

        } else if (step.type == ROTATE) {
            if (step.hasTime) {
                float angle = fmod((glutGet(GLUT_ELAPSED_TIME) / 1000.0f) * (360.0f / step.time), 360.0f);
                glRotatef(angle, step.values[0], step.values[1], step.values[2]);
            } else {
                glRotatef(step.values[3], step.values[0], step.values[1], step.values[2]);
            }

        } else if (step.type == SCALE) {
            glScalef(step.values[0], step.values[1], step.values[2]);
        }
    }

    // Desenhar os modelos do grupo atual
    /*for (const auto& modelInfo : group.models) {
        Model model;
        model.file = "../models/" + modelInfo.file;
        for (int i = 0; i < 3; i++) {
            model.material.diffuse[i]  = modelInfo.material.diffuse[i];
            model.material.ambient[i]  = modelInfo.material.ambient[i];
            model.material.specular[i] = modelInfo.material.specular[i];
            model.material.emissive[i] = modelInfo.material.emissive[i];
        }
        model.material.shininess = modelInfo.material.shininess;
        model.texture.file = modelInfo.texture.file;
    
        if (model.loadFromFile(model.file)) {
            if (!model.texture.file.empty()) {
                model.loadTexture(); // Aplica a textura lida do XML
            }
            model.draw();
        }
    }*/
   for (const auto& modelInfo : group.models) {
    std::string fullPath = "../models/" + modelInfo.file;
    Model* model = findModelByFile(fullPath);
    if (model) {
        model->applyMaterial(); // Se você tiver essa função para aplicar material antes do draw
        model->draw();
    }
}


    // Chamada recursiva para os subgrupos
    for (const auto& subgroup : group.subgroups) {
        drawGroup(subgroup);
    }

    glPopMatrix(); // Restaura a matriz anterior
}

void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    float aspect_ratio = config.windowWidth / config.windowHeight;
    gluPerspective(config.camera.fov, aspect_ratio, config.camera.near, config.camera.far);
    
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    // Configuração da câmera carregada do XML
    gluLookAt(config.camera.position[0], config.camera.position[1], config.camera.position[2],
          config.camera.lookAt[0], config.camera.lookAt[1], config.camera.lookAt[2],
          config.camera.up[0], config.camera.up[1], config.camera.up[2]);
    
    glDisable(GL_LIGHTING);
    glBegin(GL_LINES);
		// X axis in red
		glColor3f(1.0f, 0.0f, 0.0f);
		glVertex3f(-100.0f, 0.0f, 0.0f);
		glVertex3f( 100.0f, 0.0f, 0.0f);
		// Y Axis in Green
		glColor3f(0.0f, 1.0f, 0.0f);
		glVertex3f(0.0f, -100.0f, 0.0f);
		glVertex3f(0.0f, 100.0f, 0.0f);
		// Z Axis in Blue
		glColor3f(0.0f, 0.0f, 1.0f);
		glVertex3f(0.0f, 0.0f, -100.0f);
		glVertex3f(0.0f, 0.0f, 100.0f);
    glEnd();
    setupLights(config.lights);   
    glEnable(GL_LIGHTING);
    
    drawGroup(config.rootGroup);
    glutSwapBuffers();
}

void Engine::run() {
    glutReshapeFunc(changeSize);
    glutDisplayFunc(display);
    glutIdleFunc(glutPostRedisplay);
    glEnable(GL_RESCALE_NORMAL);
    glEnable(GL_NORMALIZE);
    glEnable(GL_LIGHTING);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glutMainLoop();
}

void printTextures(const Group& group) {
    for (const auto& model : group.models) {
        if (!model.texture.file.empty()) {
            std::cout << "  Modelo: " << model.file << " usa textura: " << model.texture.file << std::endl;
        }
    }
    for (const auto& subgroup : group.subgroups) {
        printTextures(subgroup);
    }
}


bool Engine::init(const char* configFile) {
    // Lê o xml
    XMLParser parser;
    if (!parser.loadConfig(configFile, config)) {
        std::cerr << "Erro ao carregar configuração do XML!" << std::endl;
        return false;
    }

    std::cout << "Li corretamente o XML" << std::endl;

    std::cout << "Texturas dos modelos:\n";
    printTextures(config.rootGroup);

    // Inicialização do OpenGL
    int argc = 1;
    char* argv[1] = {(char*)""};
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DEPTH|GLUT_DOUBLE|GLUT_RGBA);
    glutInitWindowSize(config.windowWidth, config.windowHeight);
    glutCreateWindow("3D Engine");

    ilInit();

    // Carregar os modelos depois do contexto OpenGL estar pronto
    /*for (const auto& modelInfo : config.rootGroup.models) {
        Model modelObj;
        modelObj.file = "../models/" + modelInfo.file; 
        std::cout << modelObj.file;
        if (modelObj.loadFromFile(modelObj.file)) {
            models.push_back(modelObj);
            std::cout << "Carreguei o modelo: " << modelObj.file << std::endl;
        } else {
            std::cerr << "Erro ao carregar o modelo: " << modelObj.file << std::endl;
        }
    }*/
   std::function<void(const Group&)> loadModelsRecursive = [&](const Group& group) {
    for (const auto& modelInfo : group.models) {
        std::string fullPath = "../models/" + modelInfo.file;
        if (!findModelByFile(fullPath)) {
            Model model;
            model.file = fullPath;

            for (int i = 0; i < 3; i++) {
                model.material.diffuse[i]  = modelInfo.material.diffuse[i];
                model.material.ambient[i]  = modelInfo.material.ambient[i];
                model.material.specular[i] = modelInfo.material.specular[i];
                model.material.emissive[i] = modelInfo.material.emissive[i];
            }
            model.material.shininess = modelInfo.material.shininess;

            model.texture.file = modelInfo.texture.file;

            if (model.loadFromFile(model.file)) {
                if (!model.texture.file.empty()) {
                    model.loadTexture();
                }
                models.push_back(model);
                std::cout << "Carreguei modelo: " << model.file << std::endl;
            } else {
                std::cerr << "Erro ao carregar modelo: " << model.file << std::endl;
            }
        }
    }

    for (const auto& subgroup : group.subgroups) {
        loadModelsRecursive(subgroup);
    }
};

loadModelsRecursive(config.rootGroup);

    return true;
}

// Função main
int main(int argc, char* argv[]) {
    Engine engine;

    // Inicializa o engine com o arquivo de configuração XML passado como parâmetro
    if (!engine.init(argv[1])) {
        std::cerr << "Erro ao inicializar a engine!" << std::endl;
        return -1;
    }

    std::cout << "vou iniciar o engine" << std::endl;
    engine.run();

    return 0;
}