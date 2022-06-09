#include "ObjectApp.h"

star::common::Object* star::ObjectApp::currentObject = nullptr; 
bool star::ObjectApp::moveDown = false; 
bool star::ObjectApp::moveUp = false; 
bool star::ObjectApp::moveRight = false; 
bool star::ObjectApp::moveLeft = false; 
bool star::ObjectApp::click = false; 
double star::ObjectApp::zoomDir = 0;
float star::ObjectApp::ammount = 0;
glm::vec2 star::ObjectApp::prevMousePosition = glm::vec2();
glm::vec2 star::ObjectApp::prevScroll = glm::vec2();
glm::vec2 star::ObjectApp::mouseMovement = glm::vec2();

star::ObjectApp::ObjectApp(common::ConfigFile* configFile, std::vector<common::Handle>* objectList, core::ShaderManager* shaderManager, core::ObjectManager* objectManager, core::TextureManager* textureManager) :
star::common::Application<core::ShaderManager, core::ObjectManager, core::TextureManager>(configFile, objectList, shaderManager, objectManager, textureManager) { }

void star::ObjectApp::Load(){
/*    auto vertShaderPath = configFile->GetSetting(star::common::Config_Settings::mediadirectory) + "/shaders/vertShader.vert"; 
    auto fragShaderPath = configFile->GetSetting(star::common::Config_Settings::mediadirectory) + "/shaders/fragShader.frag";*/ 

    //auto vertShaderHandle = this->shaderManager->Add(vertShaderPath); 
    //auto fragShaderHandle = this->shaderManager->Add(fragShaderPath); 

    //auto objectPath = this->configFile->GetSetting(star::common::Config_Settings::mediadirectory) + "/models/cube/cube.obj"; 
    //auto texturePath = this->configFile->GetSetting(star::common::Config_Settings::mediadirectory) + "/models/cube/cubeTexture.png"; 
    //auto textureHandle = this->textureManager->Add(texturePath); 
    auto objectPath = this->configFile->GetSetting(star::common::Config_Settings::mediadirectory) + "/models/lion-statue/source/rapid.obj"; 
    auto texturePath = this->configFile->GetSetting(star::common::Config_Settings::mediadirectory) + "/models/lion-statue/source/material0_basecolor.png";
    auto textureHandle = this->textureManager->Add(texturePath); 

    //auto objPath = this->configFile->GetSetting(star::common::Config_Settings::mediadirectory) + "/models/lion-statue/source/rapid.obj"; 
    this->objectList->push_back(this->objectManager->Add(objectPath, textureHandle));

    this->currentObject = this->objectManager->Get(this->objectList->at(0));
}

void star::ObjectApp::Update(){
    if (!this->moved) {
        this->moved = true; 
    }

    //this->currentObject->rotateRelative(0.1, glm::vec3{ 0.f, 1.f, 0.f });

    auto test = this->currentObject->getModelMatrix(); 

    double timePassed = common::Time::timeElapsedLastFrameSeconds();
    auto movementAmt = 50 * timePassed;

    if (moveUp) {
        currentObject->moveRelative(glm::vec3{ 0.f, 0.f, movementAmt });
        moveUp = false; 
    }
    if (moveDown) {
        currentObject->moveRelative(glm::vec3{ 0.f, 0.f, -movementAmt });
        moveDown = false; 
    }
    if (moveLeft) {
        currentObject->moveRelative(glm::vec3{ -movementAmt, 0.f, 0.f });
        moveLeft = false; 
    }
    if (moveRight) {
        currentObject->moveRelative(glm::vec3{ movementAmt, 0.f, 0.f });
        moveRight = false; 
    }
    if (click) {
        if (ammount != 0)
            currentObject->rotateRelative(ammount, glm::vec3{ mouseMovement.x, mouseMovement.y, 0.f });
    }
    if (zoomDir != 0) {
        auto currScale = currentObject->getScale(); 
        std::cout << "cuyrr scale: " << currScale.x << "," << currScale.y << "," << currScale.z << std::endl;
        currentObject->setScale(glm::vec3{
                currScale.x + (zoomDir * scaleAmt),
                currScale.y + (zoomDir * scaleAmt),
                currScale.z + (zoomDir * scaleAmt)
        });

        //reset
        zoomDir = 0;
    }
}

void star::ObjectApp::GLFWKeyHandle(GLFWwindow* window, int key, int scancode, int action, int mods) {

    if (key == GLFW_KEY_UP) {
        moveUp = true;
    }
    if (key == GLFW_KEY_DOWN) {
        moveDown = true;

    }
    if (key == GLFW_KEY_LEFT) {
        moveLeft = true;

    }
    if (key == GLFW_KEY_RIGHT) {
        moveRight = true;
    }
}

void star::ObjectApp::GLFWMouseMovement(GLFWwindow* window, double xpos, double ypos)
{
    //if clicking and moving the mouse, rotate the object
    if (click) {
        auto time = star::common::Time::timeElapsedLastFrameSeconds(); 

        glm::vec2 currMousePosition = glm::vec2{ xpos, ypos }; 
        glm::vec2 mouseMovementDirection = glm::vec2{
            currMousePosition.x - prevMousePosition.x,
            currMousePosition.y - prevMousePosition.y
        };
            
            
        //currMousePosition - prevMousePosition;

        ammount = glm::distance(currMousePosition, prevMousePosition);
        std::cout << currMousePosition.x << "," << currMousePosition.y << std::endl;
        std::cout << prevMousePosition.x << "," << prevMousePosition.y << std::endl;

        std::cout << ammount << std::endl;
        mouseMovement = mouseMovementDirection * ammount; 
        prevMousePosition = currMousePosition; 
    }
}

void star::ObjectApp::GLFWMouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
{
    if (button == GLFW_MOUSE_BUTTON_LEFT) {
        if (action == GLFW_PRESS) {
            click = true;
        }
        else if (action == GLFW_RELEASE) {
            click = false; 
        }
    }
}

void star::ObjectApp::GLFWScrollCallback(GLFWwindow* window, double xoffset, double yoffset)
{
    std::cout << yoffset << std::endl; 
    zoomDir = yoffset; 
    prevMousePosition = glm::vec2{ xoffset, yoffset }; 
}