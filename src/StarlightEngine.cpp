#include "SC/Application.hpp"
#include "SC/Time.hpp"
#include "SC/Camera.hpp"
#include "SC/RenderOptions.hpp"
#include "SC/Handle.hpp"

#include "StarlightEngine.h"
#include "ShaderManager.h"
#include "SceneBuilder.hpp"
#include "ObjectManager.hpp"
#include "TextureManager.hpp"
#include "LightManager.hpp"
#include "BasicVulkanRenderer.h"
#include "InteractionSystem.h"
#include "CameraController.h"
#include "OptionsController.h"
#include "MapManager.hpp"
#include "Star_Window.hpp"

#include "TextureApp.h"
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

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
    std::unique_ptr<star::common::RenderOptions> renderOptions(new star::common::RenderOptions); 
    std::unique_ptr<star::core::ShaderManager> shaderManager(new star::core::ShaderManager(defaultVertShader, defaultFragShader));
    std::unique_ptr<star::core::ObjectManager> objectManager(new star::core::ObjectManager());
    std::unique_ptr<star::core::TextureManager> textureManager(new star::core::TextureManager(configFile->GetSetting(star::common::Config_Settings::mediadirectory) + "images/texture.png"));
    std::unique_ptr<star::core::LightManager> lightManager(new star::core::LightManager()); 
    std::unique_ptr<star::core::MaterialManager> materialManager(new star::core::MaterialManager(std::make_unique<common::Material>(common::Material())));
    //std::unique_ptr<unsigned char> defaultMap(new unsigned char[] {0x0f, 0x0f, 0xff, 0xff});  //texture color for normal map that would not result in any changes to lighting
    std::unique_ptr<unsigned char> defaultMap(new unsigned char[] {0x00, 0x00, 0x00, 0x00});
    std::unique_ptr<star::core::MapManager> mapManager(new star::core::MapManager(std::make_unique<common::Texture>(std::move(defaultMap), 1, 1, 4)));
    std::unique_ptr<std::vector<star::common::Handle>> objectList(new std::vector<star::common::Handle>());
    std::unique_ptr<std::vector<common::Handle>> lightList(new std::vector<star::common::Handle>()); 
    std::unique_ptr<star::CameraController> camera(new star::CameraController());

    SceneBuilder sceneBuilder(*objectManager, *materialManager, *textureManager, *mapManager, *lightManager); 

    auto application = star::TextureApp(configFile.get(), objectList.get(), lightList.get(), 
        shaderManager.get(), textureManager.get(), lightManager.get(), sceneBuilder,
        camera.get());
    application.Load();

    std::unique_ptr<star::OptionsController> optionsController(new star::OptionsController(*renderOptions));

    //TODO: implement better management system 
    std::vector<star::common::Light*> mainLightList(lightList->size());
    for (size_t i = 0; i < lightList->size(); i++) {
        mainLightList.at(i) = &lightManager->resource(lightList->at(i)); 
    }
     
    //prepare renderer 
    auto window = star::core::StarWindow(WIDTH, HEIGHT, "Starlight", star::InteractionSystem::glfwKeyHandle, star::InteractionSystem::glfwMouseButtonCallback, star::InteractionSystem::glfwMouseMovement, star::InteractionSystem::glfwScrollCallback);
    auto renderer = star::core::VulkanRenderer(*configFile, *renderOptions, *shaderManager, *objectManager, *textureManager, *mapManager, *materialManager, *camera, *objectList, mainLightList, window);
    renderer.prepare();

    //register user application callbacks
    std::unique_ptr<std::function<void(int, int, int, int)>> keyCallback = std::make_unique<std::function<void(int, int, int, int)>>(std::bind(&star::TextureApp::Interactivity::keyCallback, application, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4));
    star::InteractionSystem::registerKeyCallback(std::move(keyCallback));

    std::unique_ptr<std::function<void(double, double)>> mouseMovementCallback = std::make_unique<std::function<void(double, double)>>(std::bind(&star::TextureApp::Interactivity::mouseMovementCallback, application, std::placeholders::_1, std::placeholders::_2));
    star::InteractionSystem::registerMouseMovementCallback(std::move(mouseMovementCallback));

    std::unique_ptr<std::function<void(int, int, int)>> mouseButtonCallback = std::make_unique<std::function<void(int, int, int)>>(std::bind(&star::TextureApp::Interactivity::mouseButtonCallback, application, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3)); 
    star::InteractionSystem::registerMouseButtonCallback(std::move(mouseButtonCallback)); 

    std::unique_ptr<std::function<void(double, double)>> mouseScrollCallback = std::make_unique<std::function<void(double, double)>>(std::bind(&star::TextureApp::Interactivity::scrollCallback, application, std::placeholders::_1, std::placeholders::_2)); 
    star::InteractionSystem::registerMouseScrollCallback(std::move(mouseScrollCallback)); 

    std::unique_ptr<std::function<void(int, int, int, int)>> camKeyCallback = std::make_unique<std::function<void(int, int, int, int)>>(std::bind(&star::CameraController::Interactivity::keyCallback, camera.get(), std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4));
    star::InteractionSystem::registerKeyCallback(std::move(camKeyCallback));

    std::unique_ptr<std::function<void()>> camWorldCallback = std::make_unique<std::function<void()>>(std::bind(&star::CameraController::Interactivity::worldUpdate, camera.get())); 
    star::InteractionSystem::registerWorldUpdateCallback(std::move(camWorldCallback)); 

    std::unique_ptr<std::function<void(double, double)>> camMouseMvmtCallback = std::make_unique<std::function<void(double, double)>>(std::bind(&star::CameraController::Interactivity::mouseMovementCallback, camera.get(), std::placeholders::_1, std::placeholders::_2)); 
    star::InteractionSystem::registerMouseMovementCallback(std::move(camMouseMvmtCallback)); 

    std::unique_ptr<std::function<void(int, int, int)>> camMouseButtonCallback = std::make_unique<std::function<void(int, int, int)>>(std::bind(&star::CameraController::Interactivity::mouseButtonCallback, camera.get(), std::placeholders::_1, std::placeholders::_2, std::placeholders::_3)); 
    star::InteractionSystem::registerMouseButtonCallback(std::move(camMouseButtonCallback)); 

    std::unique_ptr<std::function<void(int, int, int, int)>> optionsButtonCallback = std::make_unique<std::function<void(int, int, int, int)>>(std::bind(&star::OptionsController::Interactivity::keyCallback, optionsController.get(), std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4)); 
    star::InteractionSystem::registerKeyCallback(std::move(optionsButtonCallback)); 

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