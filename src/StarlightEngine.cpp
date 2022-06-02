#include "StarlightEngine.h"
#include "ShaderManager.h"
#include "ObjectManager.h"
#include "TextureManager.h"

#include "ObjectApp.h"

#include <memory>

using namespace star;

int main() {
    // std::unique_ptr<shadermanager::ShaderManager> shaderManager(new shadermanager::ShaderManager()); 
    // std::shared_ptr<common::ConfigFile> configFile(new common::ConfigFile("../Engine.cfg")); 

    auto application = star::ObjectApp("../Engine.cfg"); 
    application.Load(); 

    std::cout << "YAY" << std::endl; 
}