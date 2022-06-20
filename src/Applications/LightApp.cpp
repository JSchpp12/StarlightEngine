#include "LightApp.h"

star::common::GameObject* star::LightApp::currentObject = nullptr;
bool star::LightApp::moveDown = false;
bool star::LightApp::moveUp = false;
bool star::LightApp::moveRight = false;
bool star::LightApp::moveLeft = false;
bool star::LightApp::click = false;
double star::LightApp::zoomDir = 0;
float star::LightApp::ammount = 0;
glm::vec2 star::LightApp::prevMousePosition = glm::vec2();
glm::vec2 star::LightApp::prevScroll = glm::vec2();
glm::vec2 star::LightApp::mouseMovement = glm::vec2();

star::LightApp::LightApp(common::ConfigFile* configFile, std::vector<common::Handle>* objectList, core::ShaderManager* shaderManager, core::ObjectManager* objectManager, core::TextureManager* textureManager, common::Camera* inCamera) :
    star::common::Application<core::ShaderManager, core::ObjectManager, core::TextureManager>(configFile, objectList, shaderManager, objectManager, textureManager, inCamera) { }

void star::LightApp::Load() {
    auto objectPath = this->configFile->GetSetting(star::common::Config_Settings::mediadirectory) + "models/lion-statue/source/rapid.obj";
    auto texturePath = this->configFile->GetSetting(star::common::Config_Settings::mediadirectory) + "models/lion-statue/source/material0_basecolor.png";
    auto textureHandle = this->textureManager->Add(texturePath);

    this->objectList->push_back(this->objectManager->Add(objectPath, textureHandle));
    this->currentObject = this->objectManager->Get(this->objectList->at(0));
    this->currentObject->moveRelative(glm::vec3{ 0.0f, -0.7f, 0.0f });
    this->currentObject->rotateRelative(-90, glm::vec3{ 1.0f, 0.0f, 0.0f });
}

void star::LightApp::Update() {
    //this->currentObject->moveRelative(glm::vec3{ 0.0f, 0.0f, 0.001f }); 
}

void star::LightApp::keyCallback(int key, int scancode, int action, int mods) {
}

void star::LightApp::mouseMovementCallback(double xpos, double ypos)
{
    ////if clicking and moving the mouse, rotate the object
    //if (click) {
    //    auto time = star::common::Time::timeElapsedLastFrameSeconds(); 

    //    glm::vec2 currMousePosition = glm::vec2{ xpos, ypos }; 
    //    glm::vec2 mouseMovementDirection = glm::vec2{
    //        currMousePosition.x - prevMousePosition.x,
    //        currMousePosition.y - prevMousePosition.y
    //    };

    //    ammount = glm::distance(currMousePosition, prevMousePosition) * 0.1;
    //    //std::cout << currMousePosition.x << "," << currMousePosition.y << std::endl;
    //    //std::cout << prevMousePosition.x << "," << prevMousePosition.y << std::endl;
    //    //std::cout << ammount << std::endl;

    //    mouseMovement = mouseMovementDirection * ammount; 
    //    prevMousePosition = currMousePosition; 
    //}
}

void star::LightApp::mouseButtonCallback(int button, int action, int mods)
{
}

void star::LightApp::scrollCallback(double xoffset, double yoffset)
{

}