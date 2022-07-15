#include "TextureApp.h"

star::TextureApp::TextureApp(common::ConfigFile* configFile, std::vector<common::Handle>* objectList, std::vector<common::Handle>* lightList,
    core::ShaderManager* shaderManager, core::TextureManager* textureManager,
    core::LightManager* lightManager, SceneBuilder& sceneBuilder,
    common::Camera* inCamera) :
    lightList(lightList), star::common::Application<core::ShaderManager,
    core::TextureManager, core::LightManager, SceneBuilder>(configFile, objectList, shaderManager, textureManager, lightManager, sceneBuilder, inCamera) { }

void star::TextureApp::Load() {
    //load lion 
    auto mediaDirectoryPath = this->configFile->GetSetting(star::common::Config_Settings::mediadirectory);
    {
        auto objectPath = this->configFile->GetSetting(star::common::Config_Settings::mediadirectory) + "models/lion-statue/source/rapid.obj";
        //auto materialsPath = mediaDirectoryPath + "models/lion-statue/source";
        this->objectList->push_back(SceneBuilder::GameObjects::Builder(this->sceneBuilder)
            .setPath(objectPath)
            .setPosition(glm::vec3{1.0f, -0.95f, 0.5f})
            .setScale(glm::vec3{ 0.04f, 0.04f, 0.04f })
            .build(true)
        );
    }
    this->lion = &this->sceneBuilder.getObject(this->objectList->at(0));
    this->lion->rotateRelative(-90, glm::vec3{ 1.0f, 0.0f, 0.0f });

    //load plant 
    {
        auto objectPath = this->configFile->GetSetting(star::common::Config_Settings::mediadirectory) + "models/aloevera/aloevera.obj";
        this->objectList->push_back(SceneBuilder::GameObjects::Builder(this->sceneBuilder)
            .setPath(objectPath)
            .setPosition(glm::vec3{ -1.0f, 0.0f, -0.0f })
            .setScale(glm::vec3{1.5f, 1.5f, 1.5f})
            .build());
    }
    //table
    {
        auto objectPath = mediaDirectoryPath + "models/table/Desk OBJ.obj";
        this->objectList->push_back(SceneBuilder::GameObjects::Builder(this->sceneBuilder)
            .setPath(objectPath)
            .setPosition(glm::vec3{ 0.0f, -0.4f, 0.0f })
            .setScale(glm::vec3{0.01f, 0.01f, 0.01f})
            .setMaterialFilePath(mediaDirectoryPath + "models/table/")
            .setTextureDirectory(mediaDirectoryPath + "models/table/textures/")
            .build());
    }

    {
        //load light
        this->lightList->push_back(this->lightManager->Add(common::Type::Light::point, glm::vec3{ 0.0f, 0.4f, 0.0f }, glm::vec4{ 1.0f, 1.0f, 1.0f, 5.0f }));
        this->pointLight = this->lightManager->Get(this->lightList->at(0));
        this->lightList->push_back(this->lightManager->Add(common::Type::Light::directional, glm::vec3{}, glm::vec4{ 1.0f, 1.0f, 1.0f, 0.5f }));

        auto objectPath = mediaDirectoryPath + "models/icoSphere/low_poly_icoSphere.obj";
        auto vertShaderPath = mediaDirectoryPath + "models/icoSphere/icoSphere.vert";
        auto fragShaderPath = mediaDirectoryPath + "models/icoSphere/icoSphere.frag";

        this->pointLight->setLinkedObjectHandle(SceneBuilder::GameObjects::Builder(this->sceneBuilder)
            .setPath(objectPath)
            .setScale(glm::vec3{ 0.07f, 0.07f, 0.07f })
            .setPosition(this->pointLight->getPosition())
            .setVertShader(this->shaderManager->add(vertShaderPath))
            .setFragShader(this->shaderManager->add(fragShaderPath))
            .build(false));
        this->pointLight->setScale(glm::vec3{ 0.07f, 0.07f, 0.07f });
        this->pointLight->setLinkedObject(this->sceneBuilder.getObject(this->pointLight->getLinkedObjectHandle()));
    }
}

void star::TextureApp::Update() {
}

void star::TextureApp::keyCallback(int key, int scancode, int action, int mods) {
}

void star::TextureApp::mouseMovementCallback(double xpos, double ypos) {
}

void star::TextureApp::mouseButtonCallback(int button, int action, int mods) {

}

void star::TextureApp::scrollCallback(double xoffset, double yoffset) {
}