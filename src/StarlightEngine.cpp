#include "SC/Application.hpp"
#include "SC/Time.hpp"
#include "SC/Camera.hpp"

#include "StarlightEngine.h"
#include "ShaderManager.h"
#include "SceneBuilder.hpp"
#include "ObjectManager.hpp"
#include "TextureManager.h"
#include "LightManager.hpp"
#include "BasicVulkanRenderer.h"
#include "InteractionSystem.h"
#include "CameraController.h"
#include "Star_Window.hpp"

#include "LightApp.h"
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
    auto defaultVertShader = configFile->GetSetting(star::common::Config_Settings::mediadirectory) + "shaders/defaultVert.vert";
    auto defaultFragShader = configFile->GetSetting(star::common::Config_Settings::mediadirectory) + "shaders/defaultFrag.frag";
    auto defaultCube = configFile->GetSetting(star::common::Config_Settings::mediadirectory) + "models/cube/cub.obj";
    auto defaultCubeTexture = configFile->GetSetting(star::common::Config_Settings::mediadirectory) + "models/cube/cubeTexture.png";
    std::unique_ptr<star::core::ShaderManager> shaderManager(new star::core::ShaderManager(defaultVertShader, defaultFragShader));
    std::unique_ptr<star::core::ObjectManager> objectManager(new star::core::ObjectManager());
    std::unique_ptr<star::core::TextureManager> textureManager(new star::core::TextureManager());
    std::unique_ptr<star::core::LightManager> lightManager(new star::core::LightManager()); 
    std::unique_ptr<std::vector<star::common::Handle>> objectList(new std::vector<star::common::Handle>());
    std::unique_ptr<std::vector<common::Handle>> lightList(new std::vector<star::common::Handle>()); 
    std::unique_ptr<star::CameraController> camera(new star::CameraController());

    SceneBuilder sceneBuilder(*objectManager); 

    auto application = star::LightApp(configFile.get(), objectList.get(), lightList.get(), 
        shaderManager.get(), textureManager.get(), lightManager.get(), sceneBuilder,
        camera.get());
    application.Load();

    //TODO: implement better management system 
    std::vector<star::common::Light*> mainLightList(lightList->size());
    for (size_t i = 0; i < lightList->size(); i++) {
        mainLightList.at(i) = lightManager->Get(lightList->at(i)); 
    }
     
    //prepare renderer 
    auto window = star::core::StarWindow(WIDTH, HEIGHT, "Starlight", star::InteractionSystem::glfwKeyHandle, star::InteractionSystem::glfwMouseButtonCallback, star::InteractionSystem::glfwMouseMovement, star::InteractionSystem::glfwScrollCallback);
    auto renderer = star::core::VulkanRenderer(configFile.get(), shaderManager.get(), objectManager.get(), textureManager.get(), camera.get(), objectList.get(), mainLightList, window);
    renderer.prepare();

    //register user application callbacks
    std::unique_ptr<std::function<void(int, int, int, int)>> keyCallback = std::make_unique<std::function<void(int, int, int, int)>>(std::bind(&star::LightApp::Interactivity::keyCallback, application, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4));
    star::InteractionSystem::registerKeyCallback(std::move(keyCallback));

    std::unique_ptr<std::function<void(double, double)>> mouseMovementCallback = std::make_unique<std::function<void(double, double)>>(std::bind(&star::LightApp::Interactivity::mouseMovementCallback, application, std::placeholders::_1, std::placeholders::_2));
    star::InteractionSystem::registerMouseMovementCallback(std::move(mouseMovementCallback));

    std::unique_ptr<std::function<void(int, int, int)>> mouseButtonCallback = std::make_unique<std::function<void(int, int, int)>>(std::bind(&star::LightApp::Interactivity::mouseButtonCallback, application, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3)); 
    star::InteractionSystem::registerMouseButtonCallback(std::move(mouseButtonCallback)); 

    std::unique_ptr<std::function<void(double, double)>> mouseScrollCallback = std::make_unique<std::function<void(double, double)>>(std::bind(&star::LightApp::Interactivity::scrollCallback, application, std::placeholders::_1, std::placeholders::_2)); 
    star::InteractionSystem::registerMouseScrollCallback(std::move(mouseScrollCallback)); 

    std::unique_ptr<std::function<void(int, int, int, int)>> camKeyCallback = std::make_unique<std::function<void(int, int, int, int)>>(std::bind(&star::CameraController::Interactivity::keyCallback, camera.get(), std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4));
    star::InteractionSystem::registerKeyCallback(std::move(camKeyCallback));

    std::unique_ptr<std::function<void()>> camWorldCallback = std::make_unique<std::function<void()>>(std::bind(&star::CameraController::Interactivity::worldUpdate, camera.get())); 
    star::InteractionSystem::registerWorldUpdateCallback(std::move(camWorldCallback)); 

    std::unique_ptr<std::function<void(double, double)>> camMouseMvmtCallback = std::make_unique<std::function<void(double, double)>>(std::bind(&star::CameraController::Interactivity::mouseMovementCallback, camera.get(), std::placeholders::_1, std::placeholders::_2)); 
    star::InteractionSystem::registerMouseMovementCallback(std::move(camMouseMvmtCallback)); 

    std::unique_ptr<std::function<void(int, int, int)>> camMouseButtonCallback = std::make_unique<std::function<void(int, int, int)>>(std::bind(&star::CameraController::Interactivity::mouseButtonCallback, camera.get(), std::placeholders::_1, std::placeholders::_2, std::placeholders::_3)); 
    star::InteractionSystem::registerMouseButtonCallback(std::move(camMouseButtonCallback)); 

    try {
        //init time 
        common::Time::updateLastFrameTime();

        while (!window.shouldClose()) {
            renderer.pollEvents();
            star::InteractionSystem::callWorldUpdates(); 
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