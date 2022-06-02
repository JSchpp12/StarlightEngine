#include "ObjectApp.h"

star::ObjectApp::ObjectApp(common::ConfigFile* configFile, core::ShaderManager* shaderManager, core::ObjectManager* objectManager, core::TextureManager* textureManager) : 
star::common::Application(configFile, shaderManager, objectManager, textureManager) {
    auto vertShaderPath = configFile->GetSetting(star::common::Config_Settings::mediadirectory) + "/shaders/vertShader.vert"; 
    auto fragShaderPath = configFile->GetSetting(star::common::Config_Settings::mediadirectory) + "/shaders/fragShader.frag"; 

    auto vertShaderHandle = this->shaderManager->Add(vertShaderPath); 
    auto fragShaderHandle = this->shaderManager->Add(fragShaderPath); 

    auto objPath = this->configFile->GetSetting(star::common::Config_Settings::mediadirectory) + "/models/lion-statue/source/rapid.obj"; 
    this->objectManager->Add(objPath); 


}

void star::ObjectApp::Load(){

}

void star::ObjectApp::Update(){

}