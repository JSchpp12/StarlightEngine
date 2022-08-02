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
            .setPosition(glm::vec3{ 0.0f, 0.0f, 0.0f })
            .setScale(glm::vec3{ 0.05f, 0.05f, 0.05f })
            .overrideDiffuse(glm::vec3{ 10.0f, 10.0f, 10.0f })
            .overrideSpecular(glm::vec3{ 10.0f, 10.0f, 10.0f })
            .overrideShiny(512)
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

        //load light
        this->lightList->push_back(SceneBuilder::Lights::Builder(this->sceneBuilder)
            .setType(common::Type::Light::directional)
            .setPosition(glm::vec3{ -2.0f, 2.0f, 0.0f })
            .setAmbient(glm::vec4{ 1.0f, 1.0f, 0.7f, 0.6f })
            .setDiffuse(glm::vec4{ 1.0f, 1.0f, 0.7, 0.6f })
            .setSpecular(glm::vec4{ 1.0f, 1.0f, 0.7f, 0.6f })
            .setDirection(glm::vec4{0.0f, -1.0f, 0.0f, 0.0f})
            .build());
        sun = &sceneBuilder.light(lightList->at(0)); 

        this->lightList->push_back(SceneBuilder::Lights::Builder(this->sceneBuilder)
            .setType(common::Type::Light::point)
            .setPosition(glm::vec3{ 0.4f, 0.4f, 0.0f })
            .setAmbient(glm::vec4{ 1.0f, 0.0f, 0.0f, 0.03f })
            .setDiffuse(glm::vec4{ 1.0f, 0.0f, 0.0f, 20.0f })
            .setSpecular(glm::vec4{ 1.0f, 0.0f, 0.0f, 2.0f })
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
            .setDiffuse(glm::vec4{ 0.0f, 0.0f, 1.0f, 0.5f })
            .setSpecular(glm::vec4{ 0.0f, 0.0f, 1.0f, 1.0f })
            .setLinkedObject(SceneBuilder::GameObjects::Builder(this->sceneBuilder)
                .setPath(objectPath)
                .setScale(glm::vec3{ 0.07f, 0.07f, 0.07f })
                .setColor(glm::vec4{ 0.0f, 0.0f, 1.0f, 1.0f })
                .setVertShader(this->shaderManager->addResource(vertShaderPath, std::make_unique<common::Shader>(vertShaderPath)))
                .setFragShader(this->shaderManager->addResource(fragShaderPath, std::make_unique<common::Shader>(fragShaderPath)))
                .build(false))
            .build());
        this->lightList->push_back(SceneBuilder::Lights::Builder(this->sceneBuilder)
            .setType(common::Type::Light::point)
            .setPosition(glm::vec3{ 1.0f, 0.4f, -0.2f })
            .setAmbient(glm::vec4{ 0.5f, 1.0f, 1.0f, 0.2f })
            .setDiffuse(glm::vec4{ 0.5f, 1.0f, 1.0f, 5.0f })
            .setSpecular(glm::vec4{ 1.0f, 1.0f, 1.0f, 0.2f })
            .setPosition(glm::vec3{ 1.0f, 1.0f, 0.0f })
            .setLinkedObject(SceneBuilder::GameObjects::Builder(this->sceneBuilder)
                .setPath(objectPath)
                .setScale(glm::vec3{ 0.07f, 0.07f, 0.07f })
                .setColor(glm::vec4{ 0.5f, 1.0f, 1.0f, 1.0f })
                .setVertShader(this->shaderManager->addResource(vertShaderPath, std::make_unique<common::Shader>(vertShaderPath)))
                .setFragShader(this->shaderManager->addResource(fragShaderPath, std::make_unique<common::Shader>(fragShaderPath)))
                .build(false))
            .build());

        std::cout << "App Controls" << std::endl; 
        std::cout << "Press M to switch lights off and on in order" << std::endl; 
        std::cout << "Right Arrow: rotate sun light clockwise" << std::endl; 
        std::cout << "Left Arrow: rotate sun light counter clockwise" << std::endl;
    }
}

void star::LightTypeApp::Update() {
    rock->rotateRelative(common::Type::Axis::y, (float)common::Time::timeElapsedLastFrameSeconds() * 20);

    if (pressRight) {
        auto rot = glm::mat4(1.0f);
        float rotationAmt = (float)common::Time::timeElapsedLastFrameSeconds() * sunSpeed;
        rot = glm::rotate(rot, rotationAmt, glm::vec3{ 0.0f, 0.0f, -1.0f });
        sun->direction = rot * sun->direction;
    }
    else if (pressLeft) {
        auto rot = glm::mat4(1.0f);
        float rotationAmt = (float)common::Time::timeElapsedLastFrameSeconds() * sunSpeed;
        rot = glm::rotate(rot, rotationAmt, glm::vec3{ 0.0f, 0.0f, 1.0f });
        sun->direction = rot * sun->direction;
    }
}

//TODO: these callbacks are not able to effect the actual instance of the object as the copy of them is given to the interaction system on init...
//will need own interaction system to make these callbacks
void star::LightTypeApp::keyCallback(int key, int scancode, int action, int mods) {
    if (key == GLFW_KEY_M && action == GLFW_PRESS) {
        auto& light = sceneBuilder.light(lightList->at(disabledLightCounter));
        light.setEnabled(light.enabled ? false : true);
        if (!upCounter && disabledLightCounter == 0) {
            upCounter = true;
        }
        else if (upCounter && disabledLightCounter == lightList->size() - 1) {
            upCounter = false; 
        }
        std::cout << disabledLightCounter << " : " << light.enabled << std::endl;
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
}

void star::LightTypeApp::mouseMovementCallback(double xpos, double ypos) {
}

void star::LightTypeApp::mouseButtonCallback(int button, int action, int mods) {

}

void star::LightTypeApp::scrollCallback(double xoffset, double yoffset) {
}