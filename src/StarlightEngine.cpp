#include "StarlightEngine.h"
#include "SC/Shader.h"
#include "ShaderManager/ShaderManager.h"

#include <memory>

using namespace star;

int main() {
    // std::unique_ptr<shadermanager::ShaderManager> shaderManager(new shadermanager::ShaderManager()); 
    std::shared_ptr<common::ConfigFile> configFile(new common::ConfigFile("../Engine.cfg")); 

    shadermanager::ShaderManager shaderManager = shadermanager::ShaderManager(configFile); 

    auto vertShaderPath = configFile->GetSetting(star::common::Config_Settings::mediadirectory) + "shaders/vertShader.vert"; 

    shaderManager.AddShader(vertShaderPath); 

    std::cout << "YAY" << std::endl; 
}