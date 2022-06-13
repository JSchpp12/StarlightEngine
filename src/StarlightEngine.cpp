#include "SC/Application.hpp"
#include "SC/Time.hpp"
#include "SC/Camera.hpp"

#include "StarlightEngine.h"
#include "ShaderManager.h"
#include "ObjectManager.h"
#include "TextureManager.h"
#include "VulkanRenderer.h"
#include "InteractionSystem.h"

#include "ObjectApp.h"
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <memory>
#include <chrono>
#include <functional>

using namespace star;

//setup timing 
std::chrono::steady_clock::time_point common::Time::lastFrameTime = std::chrono::steady_clock::now(); 

const uint32_t WIDTH = 800;
const uint32_t HEIGHT = 600;

int main() {
    std::unique_ptr<common::ConfigFile> configFile(new common::ConfigFile("Engine.cfg"));
    auto defaultVertShader = configFile->GetSetting(star::common::Config_Settings::mediadirectory) + "shaders/vertShader.vert";
    auto defaultFragShader = configFile->GetSetting(star::common::Config_Settings::mediadirectory) + "shaders/fragShader.frag";
    auto defaultCube = configFile->GetSetting(star::common::Config_Settings::mediadirectory) + "models/cube/cub.obj";
    auto defaultCubeTexture = configFile->GetSetting(star::common::Config_Settings::mediadirectory) + "models/cube/cubeTexture.png";
    std::unique_ptr<star::core::ShaderManager> shaderManager(new star::core::ShaderManager(defaultVertShader, defaultFragShader));
    std::unique_ptr<star::core::ObjectManager> objectManager(new star::core::ObjectManager());
    std::unique_ptr<star::core::TextureManager> textureManager(new star::core::TextureManager());
    std::unique_ptr<std::vector<star::common::Handle>> objectList(new std::vector<star::common::Handle>());
    std::unique_ptr<common::Camera> camera(new common::Camera());

    auto application = star::ObjectApp(configFile.get(), objectList.get(), shaderManager.get(), objectManager.get(), textureManager.get(), camera.get());
    application.Load();

    //prepare renderer 
    //TODO: give main() ownership of object list, not application 
    auto renderer = star::core::VulkanRenderer(configFile.get(), shaderManager.get(), objectManager.get(), textureManager.get(), camera.get(), objectList.get());
    renderer.prepareGLFW(WIDTH, HEIGHT, star::core::InteractionSystem::glfwKeyHandle, star::core::InteractionSystem::glfwMouseButtonCallback, star::core::InteractionSystem::glfwMouseMovement, star::core::InteractionSystem::glfwScrollCallback);
    renderer.prepare();

    //register user application callbacks
    std::unique_ptr<std::function<void(int, int, int, int)>> keyCallback = std::make_unique<std::function<void(int, int, int, int)>>(std::bind(&star::ObjectApp::Interactivity::keyCallback, application, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4));
    star::core::InteractionSystem::registerKeyCallback(std::move(keyCallback));

    std::unique_ptr<std::function<void(double, double)>> mouseMovementCallback = std::make_unique<std::function<void(double, double)>>(std::bind(&star::ObjectApp::Interactivity::mouseMovementCallback, application, std::placeholders::_1, std::placeholders::_2));
    star::core::InteractionSystem::registerMouseMovementCallback(std::move(mouseMovementCallback));

    std::unique_ptr<std::function<void(int, int, int)>> mouseButtonCallback = std::make_unique<std::function<void(int, int, int)>>(std::bind(&star::ObjectApp::Interactivity::mouseButtonCallback, application, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3)); 
    star::core::InteractionSystem::registerMouseButtonCallback(std::move(mouseButtonCallback)); 

    std::unique_ptr<std::function<void(double, double)>> mouseScrollCallback = std::make_unique<std::function<void(double, double)>>(std::bind(&star::ObjectApp::Interactivity::scrollCallback, application, std::placeholders::_1, std::placeholders::_2)); 
    star::core::InteractionSystem::registerMouseScrollCallback(std::move(mouseScrollCallback)); 

    std::unique_ptr<std::function<void(int, int, int, int)>> camKeyCallback = std::make_unique<std::function<void(int, int, int, int)>>(std::bind(&star::common::Camera::Interactivity::keyCallback, camera.get(), std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4));
    star::core::InteractionSystem::registerKeyCallback(std::move(camKeyCallback));

    try {
        while (!renderer.shouldCloseWindow()) {
            renderer.pollEvents();
            application.Update();
            renderer.draw();

            common::Time::updateLastFrameTime();
        }
    }
    catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        do {
            std::cout << "Press a key to exit..." << std::endl;
        } while (std::cin.get() != '\n');
        return EXIT_FAILURE;
    }
    
    return EXIT_SUCCESS;
}