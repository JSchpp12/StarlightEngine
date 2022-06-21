#include "LightApp.h"

star::LightApp::LightApp(common::ConfigFile* configFile, std::vector<common::Handle>* objectList, core::ShaderManager* shaderManager, core::ObjectManager* objectManager, core::TextureManager* textureManager, common::Camera* inCamera) :
    star::common::Application<core::ShaderManager, core::ObjectManager, core::TextureManager>(configFile, objectList, shaderManager, objectManager, textureManager, inCamera) { }

void star::LightApp::Load() {
    //load lion 
    auto mediaDirectoryPath = this->configFile->GetSetting(star::common::Config_Settings::mediadirectory); 
    {
        auto objectPath = this->configFile->GetSetting(star::common::Config_Settings::mediadirectory) + "models/lion-statue/source/rapid.obj";
        auto texturePath = this->configFile->GetSetting(star::common::Config_Settings::mediadirectory) + "models/lion-statue/source/material0_basecolor.png";
        auto textureHandle = this->textureManager->Add(texturePath);
        this->objectList->push_back(core::ObjectManager::Builder(this->objectManager)
            .setPath(objectPath)
            .setTexture(textureHandle)
            .setPosition(glm::vec3{ 0.0f, -0.44f, 0.0f })
            .build()
        );
    }
    this->lion = this->objectManager->Get(this->objectList->at(0));
    //this->lion->moveRelative(glm::vec3{ 0.0f, -0.7f, 0.0f });
    this->lion->rotateRelative(-90, glm::vec3{ 1.0f, 0.0f, 0.0f });

    {
        auto objectPath = mediaDirectoryPath + "models/cone/cone.obj";
        auto texturePath = mediaDirectoryPath + "models/cone/ConeTexture.png";
        auto vertShaderPath = mediaDirectoryPath + "shaders/defaultVert.vert";
        auto fragShaderPath = mediaDirectoryPath + "shaders/defaultFrag.frag";

        auto vertShader = this->shaderManager->Add(vertShaderPath);
        auto fragShader = this->shaderManager->Add(fragShaderPath);
        auto textureHandle = this->textureManager->Add(texturePath);
        this->objectList->push_back(core::ObjectManager::Builder(this->objectManager)
            .setPath(objectPath)
            .setTexture(textureHandle)
            .setScale(glm::vec3{ 0.2f, 0.2f, 0.2f })
            .setPosition(glm::vec3{ 1.2f, 0.21f, 0.5f })
            .build());
        this->objectList->push_back(core::ObjectManager::Builder(this->objectManager)
            .setPath(objectPath)
            .setTexture(textureHandle)
            .setScale(glm::vec3{ 0.2f, 0.2f, 0.2f })
            .setPosition(glm::vec3{ -1.3f, 0.21f, -0.5f })
            .build());
    }
    this->cone = this->objectManager->Get(this->objectList->at(1));

    //load quad 
    {
        auto objectPath = this->configFile->GetSetting(star::common::Config_Settings::mediadirectory) + "models/quad/quad.obj";
        this->objectList->push_back(core::ObjectManager::Builder(this->objectManager)
            .setPath(objectPath)
            .setScale(glm::vec3{ 2.0f, 1.0f, 1.0f })
            .setPosition(glm::vec3{ 0.0f, 0.0f, 0.0f })
            .build());
    }
    this->floor = this->objectManager->Get(this->objectList->at(2));
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