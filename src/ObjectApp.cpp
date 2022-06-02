#include "ObjectApp.h"

star::ObjectApp::ObjectApp(const std::string& pathConfigFile) : star::common::Application(pathConfigFile) {
    core::ShaderManager shaderManager = core::ShaderManager(); 
    core::ObjectManager objectManager = core::ObjectManager(); 

    auto vertShaderPath = configFile->GetSetting(star::common::Config_Settings::mediadirectory) + "/shaders/vertShader.vert"; 
    auto fragShaderPath = configFile->GetSetting(star::common::Config_Settings::mediadirectory) + "/shaders/fragShader.frag"; 

    auto vertShaderHandle = shaderManager.Add(vertShaderPath); 
    auto fragShaderHandle = shaderManager.Add(fragShaderPath); 

    auto objPath = this->configFile->GetSetting(star::common::Config_Settings::mediadirectory) + "/models/lion-statue/source/rapid.obj"; 
    objectManager.Add(objPath); 
}

void star::ObjectApp::Load(){

}

void star::ObjectApp::Update(){

}