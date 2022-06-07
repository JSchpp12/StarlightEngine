#include "SC/Application.hpp"
#include "StarlightEngine.h"
#include "ShaderManager.h"
#include "ObjectManager.h"
#include "TextureManager.h"
#include "Windowing.h"
#include "VulkanRenderer.h"

#include "ObjectApp.h"
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <memory>

using namespace star;

const uint32_t WIDTH = 800;
const uint32_t HEIGHT = 600;

int main() {
    // std::unique_ptr<shadermanager::ShaderManager> shaderManager(new shadermanager::ShaderManager()); 
    uint32_t numRequiredExtensions; 

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

    //prepare window 
    auto window = star::core::Windowing(WIDTH, HEIGHT, star::ObjectApp::GLFWKeyHandle); 
    auto requiredExtensions = window.getRequiredExtensions(numRequiredExtensions); 
    //prepare renderer 
    //TODO: give main() ownership of object list, not application 
    auto renderer = star::core::VulkanRenderer(configFile.get(), shaderManager.get(), objectManager.get(), textureManager.get(), objectList.get()); 
    renderer.prepareGLFW(requiredExtensions, numRequiredExtensions, window.glfwWindow); 
    renderer.prepare(); 

    try{
        while(!window.shouldCloseWindow()){
            window.pollWindowEvents(); 
            application.Update(); 

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