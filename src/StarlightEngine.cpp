#include "StarlightEngine.h"
#include "ShaderManager/ShaderManager.h"
#include "ObjectManager/ObjectManager.h"

#include <memory>

using namespace star;

int main() {
    // std::unique_ptr<shadermanager::ShaderManager> shaderManager(new shadermanager::ShaderManager()); 
    std::shared_ptr<common::ConfigFile> configFile(new common::ConfigFile("../Engine.cfg")); 

    core::ShaderManager shaderManager = core::ShaderManager(); 
    core::ObjectManager objectManager = core::ObjectManager(); 

    auto vertShaderPath = configFile->GetSetting(star::common::Config_Settings::mediadirectory) + "/shaders/vertShader.vert"; 
    auto fragShaderPath = configFile->GetSetting(star::common::Config_Settings::mediadirectory) + "/shaders/fragShader.frag"; 

    auto vertShaderHandle = shaderManager.Add(vertShaderPath); 
    auto fragShaderHandle = shaderManager.Add(fragShaderPath); 

    auto objPath = configFile->GetSetting(star::common::Config_Settings::mediadirectory) + "/models/lion-statue/source/rapid.obj"; 
    objectManager.Add(objPath); 

    std::cout << "YAY" << std::endl; 
}