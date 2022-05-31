#include "StarlightEngine.h"
#include "SC/Shader.h"
#include "SC/FileHelpers.h"
#include "ShaderManager/ShaderManager.h"

#include <memory>

using namespace star;

int main() {
    // std::unique_ptr<shadermanager::ShaderManager> shaderManager(new shadermanager::ShaderManager()); 
    std::shared_ptr<common::ConfigFile> configFile(new common::ConfigFile("../Engine.cfg")); 

    shadermanager::ShaderManager shaderManager = shadermanager::ShaderManager(configFile); 

    auto vertShaderPath = configFile->GetSetting(star::common::Config_Settings::mediadirectory) + "/shaders/vertShader.vert"; 
    auto fragShaderPath = configFile->GetSetting(star::common::Config_Settings::mediadirectory) + "/shaders/fragShader.frag"; 

    auto test = star::common::FileHelpers::ReadFile(vertShaderPath); 

    auto vertShaderHandle = shaderManager.AddShader(vertShaderPath); 
    auto fragShaderHandle = shaderManager.AddShader(fragShaderPath); 


    std::cout << "YAY" << std::endl; 
}