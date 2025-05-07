#include "xmlParser.h"
#include "modelLoader.h"
#include "../external/tinyxml2.h"
#include <iostream>

using namespace tinyxml2;

bool XMLParser::loadConfig(const std::string& filename, Config& config) {
    XMLDocument doc;
    if (doc.LoadFile(filename.c_str()) != XML_SUCCESS) {
        std::cerr << "Erro ao carregar o ficheiro XML: " << filename << std::endl;
        return false;
    }

    XMLElement* root = doc.FirstChildElement("world");
    if (!root) return false;

    XMLElement* window = root->FirstChildElement("window");
    if (window) {
        window->QueryIntAttribute("width", &config.windowWidth);
        window->QueryIntAttribute("height", &config.windowHeight);
    }

    XMLElement* camera = root->FirstChildElement("camera");
    if (camera) {
        parseCamera(camera, config);
    }

    // Ler luzes
    /*XMLElement* lights = root->FirstChildElement("lights");
    if (lights) {
        parseLights(lights, config);
    }*/

    // Ler grupo principal
    XMLElement* group = root->FirstChildElement("group");
    if (group) {
        parseGroup(group, config.rootGroup);  
    }

    // Carregar modelos
    XMLElement* modelsElement = root->FirstChildElement("models");
    if (modelsElement) {
        parseModels(modelsElement, config.rootGroup);  
    }

    return true;
}

void XMLParser::parseCamera(XMLElement* cameraElem, Config& config) {
    XMLElement* pos = cameraElem->FirstChildElement("position");
    if (pos) {
        pos->QueryFloatAttribute("x", &config.camera.position[0]);
        pos->QueryFloatAttribute("y", &config.camera.position[1]);
        pos->QueryFloatAttribute("z", &config.camera.position[2]);
    }

    XMLElement* lookAt = cameraElem->FirstChildElement("lookAt");
    if (lookAt) {
        lookAt->QueryFloatAttribute("x", &config.camera.lookAt[0]);
        lookAt->QueryFloatAttribute("y", &config.camera.lookAt[1]);
        lookAt->QueryFloatAttribute("z", &config.camera.lookAt[2]);
    }

    XMLElement* up = cameraElem->FirstChildElement("up");
    if (up) {
        up->QueryFloatAttribute("x", &config.camera.up[0]);
        up->QueryFloatAttribute("y", &config.camera.up[1]);
        up->QueryFloatAttribute("z", &config.camera.up[2]);
    }

    XMLElement* proj = cameraElem->FirstChildElement("projection");
    if (proj) {
        proj->QueryFloatAttribute("fov", &config.camera.fov);
        proj->QueryFloatAttribute("near", &config.camera.near);
        proj->QueryFloatAttribute("far", &config.camera.far);
    }
}

/*void XMLParser::parseLights(XMLElement* lightsElem, Config& config) {
    for (XMLElement* light = lightsElem->FirstChildElement("light"); light; light = light->NextSiblingElement("light")) {
        Light l;
        l.type = light->Attribute("type");

        if (light->Attribute("posX")) {
            light->QueryFloatAttribute("posX", &l.position[0]);
            light->QueryFloatAttribute("posY", &l.position[1]);
            light->QueryFloatAttribute("posZ", &l.position[2]);
        }

        if (light->Attribute("dirX")) {
            light->QueryFloatAttribute("dirX", &l.direction[0]);
            light->QueryFloatAttribute("dirY", &l.direction[1]);
            light->QueryFloatAttribute("dirZ", &l.direction[2]);
        }

        if (light->Attribute("cutoff")) {
            light->QueryFloatAttribute("cutoff", &l.cutoff);
        }

        config.lights.push_back(l);
    }
}*/


void XMLParser::parseGroup(XMLElement* groupElem, Group& group) {
    for (XMLElement* elem = groupElem->FirstChildElement(); elem; elem = elem->NextSiblingElement()) {
        if (strcmp(elem->Name(), "transform") == 0) {
            parseTransform(elem, group.transformOrder); 
        }
        else if (strcmp(elem->Name(), "models") == 0) {
            parseModels(elem, group);
        }
        else if (strcmp(elem->Name(), "group") == 0) {
            Group sub;
            parseGroup(elem, sub);
            group.subgroups.push_back(sub);
        }
    }
}


void XMLParser::parseTransform(XMLElement* transformElement, std::vector<Transform>& transformOrder) {
    for (XMLElement* elem = transformElement->FirstChildElement(); elem; elem = elem->NextSiblingElement()) {
        Transform step;

        if (strcmp(elem->Name(), "translate") == 0) {
            step.type = TRANSLATE;
            const char* alignAttr = elem->Attribute("align");
            elem->QueryFloatAttribute("time", &step.time);
            step.hasTime = (step.time > 0.0f);
            step.align = (alignAttr && std::string(alignAttr) == "True");
        
            if (step.hasTime) {
                for (tinyxml2::XMLElement* point = elem->FirstChildElement("point"); point; point = point->NextSiblingElement("point")) {
                    float x, y, z;
                    point->QueryFloatAttribute("x", &x);
                    point->QueryFloatAttribute("y", &y);
                    point->QueryFloatAttribute("z", &z);
                    step.curvePoints.push_back({x, y, z});
                }
            } else {
                elem->QueryFloatAttribute("x", &step.values[0]);
                elem->QueryFloatAttribute("y", &step.values[1]);
                elem->QueryFloatAttribute("z", &step.values[2]);
            }
        }
        else if (strcmp(elem->Name(), "rotate") == 0) {
            step.type = ROTATE;
            if (elem->QueryFloatAttribute("time", &step.time) == tinyxml2::XML_SUCCESS) {
                step.hasTime = true;
            } else {
                step.hasTime = false;
                elem->QueryFloatAttribute("angle", &step.values[3]);
            }
            elem->QueryFloatAttribute("x", &step.values[0]);
            elem->QueryFloatAttribute("y", &step.values[1]);
            elem->QueryFloatAttribute("z", &step.values[2]);
        }
        else if (strcmp(elem->Name(), "scale") == 0) {
            step.type = SCALE;
            elem->QueryFloatAttribute("x", &step.values[0]);
            elem->QueryFloatAttribute("y", &step.values[1]);
            elem->QueryFloatAttribute("z", &step.values[2]);
        }

        transformOrder.push_back(step);
    }
}

void XMLParser::parseModels(XMLElement* modelsElem, Group& group) {
    if (!modelsElem) {
        std::cerr << "Nenhum modelo encontrado no XML." << std::endl;
        return;
    }

    for (XMLElement* modelElem = modelsElem->FirstChildElement("model"); modelElem; modelElem = modelElem->NextSiblingElement("model")) {
        ModelInfo modelInfo;
        const char* file = modelElem->Attribute("file");
        if (file) {
            modelInfo.file = file;
            std::cout << "Modelo lido: " << modelInfo.file << std::endl;
            group.models.push_back(modelInfo); 
        }
    }

    if (group.models.empty()) {
        std::cerr << "Nenhum modelo foi configurado neste grupo." << std::endl;
    }
}