#include "ObjectApp.h"

star::ObjectApp::ObjectApp(common::ConfigFile* configFile, core::ShaderManager* shaderManager, core::ObjectManager* objectManager, core::TextureManager* textureManager) : 
star::common::Application(configFile, shaderManager, objectManager, textureManager) { }

void star::ObjectApp::Load(){
    auto vertShaderPath = configFile->GetSetting(star::common::Config_Settings::mediadirectory) + "/shaders/vertShader.vert"; 
    auto fragShaderPath = configFile->GetSetting(star::common::Config_Settings::mediadirectory) + "/shaders/fragShader.frag"; 

    auto vertShaderHandle = this->shaderManager->Add(vertShaderPath); 
    auto fragShaderHandle = this->shaderManager->Add(fragShaderPath); 

    auto objPath = this->configFile->GetSetting(star::common::Config_Settings::mediadirectory) + "/models/lion-statue/source/rapid.obj"; 
    this->objectManager->Add(objPath); 
}

void star::ObjectApp::GLFWKeyHandle(GLFWwindow* window, int key, int scancode, int action, int mods){
    std::cout << "Key call" << std::endl;     
}

void star::ObjectApp::Update(){

}