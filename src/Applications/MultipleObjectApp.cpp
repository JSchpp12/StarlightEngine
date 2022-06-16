#include "MultipleObjectApp.h"

star::MultipleObjectApp::MultipleObjectApp(common::ConfigFile* configFile, std::vector<common::Handle>* objectList, core::ShaderManager* shaderManager, core::ObjectManager* objectManager, core::TextureManager* textureManager, common::Camera* inCamera) :
	star::common::Application<core::ShaderManager, core::ObjectManager, core::TextureManager>(configFile, objectList, shaderManager, objectManager, textureManager, inCamera) { }

void star::MultipleObjectApp::Load()
{
    std::string objectPath, texturePath, vertShaderPath, fragShaderPath; 
    std::string mediaDirectoryPath = this->configFile->GetSetting(star::common::Config_Settings::mediadirectory); 

    {
        auto objectPath = mediaDirectoryPath + "models/cone/cone.obj";
        auto texturePath = mediaDirectoryPath + "models/cone/ConeTexture.png";
        auto vertShaderPath = mediaDirectoryPath + "shaders/defaultVert.vert";
        auto fragShaderPath = mediaDirectoryPath + "shaders/defaultFrag.frag";

        auto vertShader = this->shaderManager->Add(vertShaderPath);
        auto fragShader = this->shaderManager->Add(fragShaderPath);
        auto textureHandle = this->textureManager->Add(texturePath);
        this->objectList->push_back(this->objectManager->Add(objectPath, textureHandle, vertShader, fragShader));
    }
    this->cone = this->objectManager->Get(this->objectList->at(0));
    this->cone->moveRelative(glm::vec3{ 1.0f, 1.0f, 1.0f });
    this->coneCeil = this->cone->getPosition().y + this->moveOscillationAmt; 
    this->coneFloor = this->cone->getPosition().y - this->moveOscillationAmt; 

    {
        auto objectPath = mediaDirectoryPath + "models/cube/cube.obj";
        auto texturePath = mediaDirectoryPath + "models/cube/CubeTexture.png";
        auto vertShaderPath = mediaDirectoryPath + "shaders/defaultVert.vert";
        auto fragShaderPath = mediaDirectoryPath + "shaders/defaultFrag.frag";

        auto vertShader = this->shaderManager->Add(vertShaderPath);
        auto fragShader = this->shaderManager->Add(fragShaderPath);
        auto textureHandle = this->textureManager->Add(texturePath);
        this->objectList->push_back(this->objectManager->Add(objectPath, textureHandle, vertShader, fragShader));
    }
    this->cube = this->objectManager->Get(this->objectList->at(1)); 
    this->cube->moveRelative(glm::vec3{ -3.0f, -3.0f, -3.0f }); 
    auto position = this->cube->getPosition();
    this->cubeCeil = this->cube->getPosition().y + this->moveOscillationAmt; 
    this->cubeFloor = this->cube->getPosition().y - this->moveOscillationAmt;
}

void star::MultipleObjectApp::Update()
{
    auto timePassed = star::common::Time::timeElapsedLastFrameSeconds(); 

    //move cube 
    auto position = this->cube->getPosition(); 

    if (position.y > this->cubeCeil || position.y < this->cubeFloor) {
        this->cubeMovingUp = this->cubeMovingUp ? false : true;
    }

    this->cube->moveRelative(glm::vec3(0.0f, 
        (this->cubeMovingUp ? moveSpeed : -moveSpeed) * timePassed,
        0.f));

    if (this->cone->getPosition().y > this->coneCeil || this->cone->getPosition().y < this->coneFloor) {
        this->coneMovingUp = this->coneMovingUp ? false : true; 
    }

    this->cone->moveRelative(glm::vec3(0.0f,
        (this->coneMovingUp ? moveSpeed : -moveSpeed) * timePassed, 
        0.0f));
}

void star::MultipleObjectApp::keyCallback(int key, int scancode, int action, int mods)
{

}

void star::MultipleObjectApp::mouseMovementCallback(double xpos, double ypos)
{

}

void star::MultipleObjectApp::mouseButtonCallback(int button, int action, int mods)
{

}

void star::MultipleObjectApp::scrollCallback(double xoffset, double yoffset)
{

}
