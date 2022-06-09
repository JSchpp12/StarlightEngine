#include "SC/Application.hpp"
#include "SC/Time.hpp"

#include "StarlightEngine.h"
#include "ShaderManager.h"
#include "ObjectManager.h"
#include "TextureManager.h"
#include "VulkanRenderer.h"

#include "ObjectApp.h"
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <memory>
#include <chrono>

using namespace star;

//setup timing 
std::chrono::steady_clock::time_point common::Time::lastFrameTime = std::chrono::steady_clock::now(); 

const uint32_t WIDTH = 800;
const uint32_t HEIGHT = 600;

int main() {

    // std::unique_ptr<shadermanager::ShaderManager> shaderManager(new shadermanager::ShaderManager()); 
    std::unique_ptr<common::ConfigFile> configFile(new common::ConfigFile("Engine.cfg")); 
    auto defaultVertShader = configFile->GetSetting(star::common::Config_Settings::mediadirectory) + "/shaders/vertShader.vert";
    auto defaultFragShader = configFile->GetSetting(star::common::Config_Settings::mediadirectory) + "/shaders/fragShader.frag";
    auto defaultCube = configFile->GetSetting(star::common::Config_Settings::mediadirectory) + "/models/cube/cub.obj"; 
    auto defaultCubeTexture = configFile->GetSetting(star::common::Config_Settings::mediadirectory) + "/models/cube/cubeTexture.png"; 
    std::unique_ptr<star::core::ShaderManager> shaderManager(new star::core::ShaderManager(defaultVertShader, defaultFragShader));
    std::unique_ptr<star::core::ObjectManager> objectManager(new star::core::ObjectManager());
    std::unique_ptr<star::core::TextureManager> textureManager(new star::core::TextureManager()); 
    std::unique_ptr<std::vector<star::common::Handle>> objectList(new std::vector<star::common::Handle>());

    auto application = star::ObjectApp(configFile.get(), objectList.get(), shaderManager.get(), objectManager.get(), textureManager.get()); 
    application.Load(); 

    //prepare renderer 
    //TODO: give main() ownership of object list, not application 
    auto renderer = star::core::VulkanRenderer(configFile.get(), shaderManager.get(), objectManager.get(), textureManager.get(), objectList.get()); 
    renderer.prepareGLFW(WIDTH, HEIGHT, star::ObjectApp::GLFWKeyHandle);
    renderer.prepare(); 

    try{
        while(!renderer.shouldCloseWindow()){
            renderer.pollEvents(); 
            application.Update();
            renderer.draw(); 

            common::Time::updateLastFrameTime(); 
        }
    }catch(const std::exception& e){
        std::cerr << e.what() << std::endl;
        do {
            std::cout << "Press a key to exit..." << std::endl;
        } while (std::cin.get() != '\n');
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}