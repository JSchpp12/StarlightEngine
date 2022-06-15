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
    

}

void star::MultipleObjectApp::Update()
{

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
