#include "ObjectApp.h"

star::ObjectApp::ObjectApp(common::ConfigFile* configFile, std::vector<common::Handle>* objectList, core::ShaderManager* shaderManager, core::ObjectManager* objectManager, core::TextureManager* textureManager) :
star::common::Application<core::ShaderManager, core::ObjectManager, core::TextureManager>(configFile, objectList, shaderManager, objectManager, textureManager) { }

void star::ObjectApp::Load(){
/*    auto vertShaderPath = configFile->GetSetting(star::common::Config_Settings::mediadirectory) + "/shaders/vertShader.vert"; 
    auto fragShaderPath = configFile->GetSetting(star::common::Config_Settings::mediadirectory) + "/shaders/fragShader.frag";*/ 

    //auto vertShaderHandle = this->shaderManager->Add(vertShaderPath); 
    //auto fragShaderHandle = this->shaderManager->Add(fragShaderPath); 

    //auto objectPath = this->configFile->GetSetting(star::common::Config_Settings::mediadirectory) + "/models/cube/cube.obj"; 
    //auto texturePath = this->configFile->GetSetting(star::common::Config_Settings::mediadirectory) + "/models/cube/cubeTexture.png"; 
    //auto textureHandle = this->textureManager->Add(texturePath); 
    auto objectPath = this->configFile->GetSetting(star::common::Config_Settings::mediadirectory) + "/models/lion-statue/source/rapid.obj"; 
    auto texturePath = this->configFile->GetSetting(star::common::Config_Settings::mediadirectory) + "/models/lion-statue/source/material0_basecolor.png";
    auto textureHandle = this->textureManager->Add(texturePath); 

    //auto objPath = this->configFile->GetSetting(star::common::Config_Settings::mediadirectory) + "/models/lion-statue/source/rapid.obj"; 
    this->objectList->push_back(this->objectManager->Add(objectPath, textureHandle));
}

void star::ObjectApp::GLFWKeyHandle(GLFWwindow* window, int key, int scancode, int action, int mods){
    std::cout << "Key call" << std::endl;     
}

void star::ObjectApp::Update(){

}