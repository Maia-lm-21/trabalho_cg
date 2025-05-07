#ifndef XMLPARSER_H
#define XMLPARSER_H

#include "modelLoader.h"
#include <string>
#include <vector>
#include "../external/tinyxml2.h"

struct Color {
    int R, G, B;
};

/*struct Material {
    Color diffuse, ambient, specular, emissive;
    float shininess;
};

struct Texture {
    std::string file;
};*/

enum TransformType { TRANSLATE, ROTATE, SCALE };

struct Transform {
    TransformType type;
    float values[4];
    bool hasTime = false;
    float time = 0.0f;
    bool align = false;
    std::vector<std::array<float, 3>> curvePoints;
};

struct ModelInfo {
    std::string file;
    //Texture texture;
    //Material material;
};

struct Group {
    std::vector<Transform> transformOrder;
    std::vector<ModelInfo> models;
    std::vector<Group> subgroups;
};

/*struct Light {
    std::string type;
    float position[3] = {0, 0, 0};
    float direction[3] = {0, 0, 0};
    float cutoff = 0;
};*/

struct Camera {
    float position[3];
    float lookAt[3];
    float up[3] = {0, 1, 0};  // Default
    float fov = 60, near = 1, far = 1000;
};



struct Config {
    int windowWidth, windowHeight;
    Camera camera;
    //std::vector<Light> lights;
    Group rootGroup;
    std::vector<ModelInfo> models;
};



class XMLParser {
public:
    bool loadConfig(const std::string& filename, Config& config);
private:
    void parseCamera(tinyxml2::XMLElement* cameraElem, Config& config);
    //void parseLights(tinyxml2::XMLElement* lightsElem, Config& config);
    void parseGroup(tinyxml2::XMLElement* groupElem, Group& group);
    void parseModels(tinyxml2::XMLElement* modelsElem, Group& group); 
    void parseTransform(tinyxml2::XMLElement* transformElem, std::vector<Transform>& transformOrder);
};

#endif // XMLPARSER_H