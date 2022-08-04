#include "LightTypeApp.h"

star::LightTypeApp::LightTypeApp(common::ConfigFile* configFile, std::vector<common::Handle>* objectList, std::vector<common::Handle>* lightList,
    core::ShaderManager* shaderManager, core::TextureManager* textureManager,
    core::LightManager* lightManager, SceneBuilder& sceneBuilder,
    common::Camera* inCamera) :
    lightList(lightList), star::common::Application<core::ShaderManager,
    core::TextureManager, core::LightManager, SceneBuilder>(configFile, objectList, shaderManager, textureManager, lightManager, sceneBuilder, inCamera) { }

int star::LightTypeApp::disabledLightCounter = int(0); 
bool star::LightTypeApp::upCounter = true; 
bool star::LightTypeApp::rotatingCounterClock = true;
bool star::LightTypeApp::pressLeft = false; 
bool star::LightTypeApp::pressRight = false; 
bool star::LightTypeApp::pressUp = false; 
bool star::LightTypeApp::pressDown = false; 
bool star::LightTypeApp::pressU = false; 
bool star::LightTypeApp::pressY = false; 
bool star::LightTypeApp::pressJ = false; 
bool star::LightTypeApp::pressH = false; 

std::chrono::steady_clock::time_point star::LightTypeApp::timeSinceLastUpdate = std::chrono::steady_clock::now();

void star::LightTypeApp::Load() {
    //load lion 
    auto mediaDirectoryPath = this->configFile->GetSetting(star::common::Config_Settings::mediadirectory);
    {
        auto objectPath = this->configFile->GetSetting(star::common::Config_Settings::mediadirectory) + "models/lion-statue/source/rapid.obj";
        //auto materialsPath = mediaDirectoryPath + "models/lion-statue/source";
        this->objectList->push_back(SceneBuilder::GameObjects::Builder(this->sceneBuilder)
            .setPath(objectPath)
            .setPosition(glm::vec3{ 1.0f, -0.95f, 0.5f })
            .setScale(glm::vec3{ 0.04f, 0.04f, 0.04f })
            .overrideAmbient(glm::vec3{ 0.5f, 0.5f, 0.5f })
            .build(true)
        );
        sceneBuilder.entity(objectList->at(0)).rotateGolbal(common::Type::Axis::x, -90);
    }

    //load plant 
    {
        auto objectPath = this->configFile->GetSetting(star::common::Config_Settings::mediadirectory) + "models/aloevera/aloevera.obj";
        this->objectList->push_back(SceneBuilder::GameObjects::Builder(this->sceneBuilder)
            .setPath(objectPath)
            .setPosition(glm::vec3{ -1.0f, 0.0f, -0.0f })
            .setScale(glm::vec3{ 1.5f, 1.5f, 1.5f })
            .build());
    }
    //table
    {
        auto objectPath = mediaDirectoryPath + "models/table/Desk OBJ.obj";
        this->objectList->push_back(SceneBuilder::GameObjects::Builder(this->sceneBuilder)
            .setPath(objectPath)
            .setPosition(glm::vec3{ 0.0f, -0.4f, 0.0f })
            .setScale(glm::vec3{ 0.01f, 0.01f, 0.01f })
            .setMaterialFilePath(mediaDirectoryPath + "models/table/")
            .setTextureDirectory(mediaDirectoryPath + "models/table/textures/")
            .build());
    }
    //rock
    {
        auto objectPath = mediaDirectoryPath + "models/rock/898927_rock.obj";
        this->objectList->push_back(SceneBuilder::GameObjects::Builder(this->sceneBuilder)
            .setPath(objectPath)
            .setPosition(glm::vec3{ 0.0f, 0.0f, -0.85f })
            .setScale(glm::vec3{ 0.05f, 0.05f, 0.05f })
            .setMaterial(SceneBuilder::Materials::Builder(this->sceneBuilder)
                .setTexture(this->textureManager->addResource(common::FileHelpers::GetBaseFileDirectory(objectPath) + "textures/rock_low_Base_Color.png"))
                .setBumpMap(this->textureManager->addResource(common::FileHelpers::GetBaseFileDirectory(objectPath) + "textures/rock_low_Normal_DirectX.png"))
                .build())
            .build());
        this->rock = &this->sceneBuilder.entity(this->objectList->at(3));
    }

    {
        auto objectPath = mediaDirectoryPath + "models/icoSphere/low_poly_icoSphere.obj";
        auto vertShaderPath = mediaDirectoryPath + "models/icoSphere/icoSphere.vert";
        auto fragShaderPath = mediaDirectoryPath + "models/icoSphere/icoSphere.frag";

        ////load light
        this->lightList->push_back(SceneBuilder::Lights::Builder(this->sceneBuilder)
            .setType(common::Type::Light::directional)
            .setPosition(glm::vec3{ 0.0f, 2.0f, 0.0f })
            .setAmbient(glm::vec4{ 1.0f, 1.0f, 0.7f, 0.4f })
            .setDiffuse(glm::vec4{ 1.0f, 1.0f, 0.7, 1.0f })
            .setSpecular(glm::vec4{ 1.0f, 1.0f, 0.7f, 1.0f })
            .setDirection(glm::vec4{ 0.0f, -1.0f, 0.0f, 0.0f })
            .build());
        sun = &sceneBuilder.light(lightList->at(0));

        this->lightList->push_back(SceneBuilder::Lights::Builder(this->sceneBuilder)
            .setType(common::Type::Light::spot)
            .setPosition(glm::vec3{ -1.0f, 1.0f, 0.0f })
            .setDirection(glm::vec4{ 0.0f, -1.0f, 0.0f, 0.0f })
            .setDiameter(14.0f, 14.0f)
            .setAmbient(glm::vec4{ 1.0f, 1.0f, 1.0f, 0.01f })
            .setDiffuse(glm::vec4{ 1.0f, 1.0f, 1.0f, 1.0f })
            .setSpecular(glm::vec4{ 1.0f, 1.0f, 1.0f, 1.0f })
            .setLinkedObject(SceneBuilder::GameObjects::Builder(this->sceneBuilder)
                .setPath(objectPath)
                .setScale(glm::vec3{ 0.07f, 0.07f, 0.07f })
                .setColor(glm::vec4{ 1.0f, 1.0f, 1.0f, 1.0f })
                .setVertShader(this->shaderManager->addResource(vertShaderPath, std::make_unique<common::Shader>(vertShaderPath)))
                .setFragShader(this->shaderManager->addResource(fragShaderPath, std::make_unique<common::Shader>(fragShaderPath)))
                .build(false))
            .build());
        spot = &sceneBuilder.light(lightList->at(lightList->size() - 1));

        this->lightList->push_back(SceneBuilder::Lights::Builder(this->sceneBuilder)
            .setType(common::Type::Light::point)
            .setPosition(glm::vec3{ 0.4f, 0.4f, 0.0f })
            .setAmbient(glm::vec4{ 1.0f, 0.0f, 0.0f, 0.01f })
            .setDiffuse(glm::vec4{ 1.0f, 0.0f, 0.0f, 0.2f })
            .setSpecular(glm::vec4{ 1.0f, 0.0f, 0.0f, 0.2f })
            .setLinkedObject(SceneBuilder::GameObjects::Builder(this->sceneBuilder)
                .setPath(objectPath)
                .setScale(glm::vec3{ 0.07f, 0.07f, 0.07f })
                .setColor(glm::vec4{ 1.0f, 0.0f, 0.0f, 1.0f })
                .setVertShader(this->shaderManager->addResource(vertShaderPath, std::make_unique<common::Shader>(vertShaderPath)))
                .setFragShader(this->shaderManager->addResource(fragShaderPath, std::make_unique<common::Shader>(fragShaderPath)))
                .build(false))
            .build());
        this->lightList->push_back(SceneBuilder::Lights::Builder(this->sceneBuilder)
            .setType(common::Type::Light::point)
            .setPosition(glm::vec3{ -1.0f, 0.4f, 0.5f })
            .setAmbient(glm::vec4{ 0.0f, 0.0f, 1.0f, 0.15f })
            .setDiffuse(glm::vec4{ 0.0f, 0.0f, 1.0f, 0.2f })
            .setSpecular(glm::vec4{ 0.0f, 0.0f, 1.0f, 0.2f })
            .setLinkedObject(SceneBuilder::GameObjects::Builder(this->sceneBuilder)
                .setPath(objectPath)
                .setScale(glm::vec3{ 0.07f, 0.07f, 0.07f })
                .setColor(glm::vec4{ 0.0f, 0.0f, 1.0f, 1.0f })
                .setVertShader(this->shaderManager->addResource(vertShaderPath, std::make_unique<common::Shader>(vertShaderPath)))
                .setFragShader(this->shaderManager->addResource(fragShaderPath, std::make_unique<common::Shader>(fragShaderPath)))
                .build(false))
            .build());
    }

    std::cout << "App Controls" << std::endl; 
    std::cout << "Press M to switch lights off and on in order" << std::endl; 
    std::cout << "Right Arrow: rotate sun light clockwise" << std::endl; 
    std::cout << "Left Arrow: rotate sun light counter clockwise" << std::endl;
    std::cout << "Up Arrow: increase diameter of spot light" << std::endl; 
    std::cout << "Down Arrow: decrease diameter of spot light" << std::endl;
    std::cout << "U: Increase outer diam of spot light" << std::endl; 
    std::cout << "Y: Decrease outer diam of spot light" << std::endl; 
    std::cout << "J: Increase inner diam of spot light" << std::endl; 
    std::cout << "H: Decrease inner diam of spot light" << std::endl;
    std::cout << "Z: Enable/Disable sun" << std::endl;
}

void star::LightTypeApp::Update() {
    auto now = std::chrono::steady_clock::now(); 
    float elapsedTime = std::chrono::duration<float>(now - timeSinceLastUpdate).count();
    rock->rotateRelative(common::Type::Axis::y, elapsedTime * 30);

    if (pressRight) {
        auto rot = glm::mat4(1.0f);
        float rotationAmt = (float)time.timeElapsedLastFrameSeconds() * sunSpeed;
        rot = glm::rotate(rot, rotationAmt, glm::vec3{ 0.0f, 0.0f, -1.0f });
        sun->direction = rot * sun->direction;
    }
    else if (pressLeft) {
        auto rot = glm::mat4(1.0f);
        float rotationAmt = (float)time.timeElapsedLastFrameSeconds() * sunSpeed;
        rot = glm::rotate(rot, rotationAmt, glm::vec3{ 0.0f, 0.0f, 1.0f });
        sun->direction = rot * sun->direction;
    }

    if (pressU) {
        spot->setOuterDiameter(spot->getOuterDiameter() + (spotSpeed * elapsedTime));
        std::cout << spot->getOuterDiameter() << std::endl; 
    }
    else if (pressY) {
        spot->setOuterDiameter(spot->getOuterDiameter() - (spotSpeed * elapsedTime)); 
        std::cout << spot->getOuterDiameter() << std::endl;
    }

    if (pressJ) {
        spot->setInnerDiameter(spot->getInnerDiameter() + (spotSpeed * elapsedTime)); 
        std::cout << spot->getInnerDiameter() << std::endl;
    }
    else if (pressH) {
        spot->setInnerDiameter(spot->getInnerDiameter() - (spotSpeed * elapsedTime));
        std::cout << spot->getInnerDiameter() << std::endl;
    }
    timeSinceLastUpdate = std::chrono::steady_clock::now(); 
}

//TODO: these callbacks are not able to effect the actual instance of the object as the copy of them is given to the interaction system on init...
//will need own interaction system to make these callbacks
void star::LightTypeApp::keyCallback(int key, int scancode, int action, int mods) {
    if (key == GLFW_KEY_M && action == GLFW_PRESS) {
        auto& light = sceneBuilder.light(lightList->at(disabledLightCounter));
        light.setEnabled();
        if (!upCounter && disabledLightCounter == 0) {
            upCounter = true;
        }
        else if (upCounter && disabledLightCounter == lightList->size() - 1) {
            upCounter = false; 
        }
        disabledLightCounter = upCounter ? disabledLightCounter + 1 : disabledLightCounter - 1; 
    }
    if (action == GLFW_PRESS && key == GLFW_KEY_RIGHT) {
        pressRight = true; 
    }
    else if (action == GLFW_PRESS && key == GLFW_KEY_LEFT) {
        pressLeft = true;
    }
    else if (action == GLFW_RELEASE && key == GLFW_KEY_RIGHT) {
        pressRight = false; 
    }
    else if (action == GLFW_RELEASE && key == GLFW_KEY_LEFT) {
        pressLeft = false; 
    }

    //up/down arrows 
    if (key == GLFW_KEY_UP && action == GLFW_PRESS) {
        pressUp = true; 
    }
    else if (key == GLFW_KEY_DOWN && action == GLFW_PRESS) {
        pressDown = true;
    }
    else if (key == GLFW_KEY_UP && action == GLFW_RELEASE) {
        pressUp = false; 
    }
    else if (key == GLFW_KEY_DOWN && action == GLFW_RELEASE) {
        pressDown = false; 
    }

    if (key == GLFW_KEY_U && action == GLFW_PRESS) {
        pressU = true;
    }
    else if (key == GLFW_KEY_Y && action == GLFW_PRESS) {
        pressY = true;
    }
    else if (key == GLFW_KEY_U && action == GLFW_RELEASE) {
        pressU = false;
    }
    else if (key == GLFW_KEY_Y && action == GLFW_RELEASE) {
        pressY = false;
    }

    if (key == GLFW_KEY_J && action == GLFW_PRESS) {
        pressJ = true;
    }
    else if (key == GLFW_KEY_H && action == GLFW_PRESS) {
        pressH = true;
    }
    else if (key == GLFW_KEY_J && action == GLFW_RELEASE) {
        pressJ = false;
    }
    else if (key == GLFW_KEY_H && action == GLFW_RELEASE) {
        pressH = false;
    }

    if (key == GLFW_KEY_Z && action == GLFW_PRESS) {
        sun->setEnabled(); 
    }
}

void star::LightTypeApp::mouseMovementCallback(double xpos, double ypos) {
}

void star::LightTypeApp::mouseButtonCallback(int button, int action, int mods) {

}

void star::LightTypeApp::scrollCallback(double xoffset, double yoffset) {
}