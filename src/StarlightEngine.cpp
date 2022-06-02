#include "StarlightEngine.h"
#include "ShaderManager.h"
#include "ObjectManager.h"
#include "TextureManager.h"

#include "ObjectApp.h"

#include <memory>

using namespace star;

int main() {
    // std::unique_ptr<shadermanager::ShaderManager> shaderManager(new shadermanager::ShaderManager()); 
    
    std::unique_ptr<common::ConfigFile> configFile(new common::ConfigFile("../Engine.cfg")); 
    std::unique_ptr<star::core::ShaderManager> shaderManager(new star::core::ShaderManager());
    std::unique_ptr<star::core::ObjectManager> objectManager(new star::core::ObjectManager());
    std::unique_ptr<star::core::TextureManager> textureManager(new star::core::TextureManager()); 

    auto application = star::ObjectApp(configFile.get(), shaderManager.get(), objectManager.get(), textureManager.get()); 
    application.Load(); 

    std::cout << "YAY" << std::endl; 
}